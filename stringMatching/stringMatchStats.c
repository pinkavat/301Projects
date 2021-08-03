#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

/* stringMatchStats.c
 *
 * A demonstration of the relative efficiencies of 
 * Naive String matching, DFA matching, and the KMP algorithm.
 *
 * Written 8 December 2019 by Thomas Pinkava
 */

#define PFBUF_SIZE 1024 // Maximum length of line in the pattern file

// Naive String Matching
// All three matching techniques take the text and pattern as input strings and write their resultant statistics
// directly to the output filestream
// O(mn) algorithm with zero preprocessing cost
void naiveMatch(char *text, size_t textLength, char *pattern, size_t patternLength, FILE *output){
    // Zero statistics and print initial
    unsigned int totalCharCompares = 0;
    bool foundAtLeastOne = false;
    fprintf(output, "[");

    // For every character in the text
    for(size_t i = 0; i < textLength; i++){
        // Check to see if this character is the start character of the pattern
        bool match = true;
        for(size_t j = 0; j < patternLength; j++){
            totalCharCompares++;
            if(text[i + j] != pattern[j]){
                match = false;
                break;
            }
        }
        if(match){
            foundAtLeastOne = true;
            fprintf(output, "%zu, ", i);
        }
    }

    fprintf(output, "%s] <0, %zu, %u>", foundAtLeastOne?"\b\b":"", textLength-patternLength, totalCharCompares);
}




// Helper for DFA Matching; computes [Pk suffixof PqC]? 
bool isSuffix(char *p, unsigned int k, unsigned int q, char c){
    q = q + 1;
    if(k > q) return false;
    if(k == 0) return true;
    char *b = p + (q - k);
    for(unsigned int i = 0; i < k - 1; i++){
        if(p[i] != b[i]) return false;
    }
    if(p[k - 1] != c){
        return false;
    } else {
        return true;
    }
}



// Deterministic Finite Automaton String Matching
// Generates a DFA that recognizes the given pattern and uses the given text as its input
// All three matching techniques take the text and pattern as input strings and write their resultant statistics
// directly to the output filestream
// O(n) algorithm with O(m^3 |symbol set|) preprocessing cost
void DFAMatch(char *text, size_t textLength, char *pattern, size_t patternLength, FILE *output){
    // Zero statistics and print initial
    unsigned int totalCharCompares = 0;
    unsigned int totalShifts = 0;
    unsigned int preprocessingCost = 0;
    bool foundAtLeastOne = false;
    fprintf(output, "[");

    // Compute the Automaton's transition table
    unsigned int transitionFunction[patternLength + 1][255];    // For simplicity, the symbol set is all 2^8 ASCII chars minus '\0'
    for(unsigned int q = 0; q <= (unsigned int) patternLength; q++){
        // For every state
        for(unsigned char c = 255; c > 0; c--){
            // For every symbol in symbolset 
            unsigned int k = ((patternLength + 1) < (q + 2)) ? patternLength + 1: q + 2;
            while(!isSuffix(pattern, k, q, c)) k--;
            transitionFunction[q][c - 1] = k; 

            preprocessingCost += k;
        }
    }

    // Run the Automaton on the text
    unsigned int state = 0;
    for(size_t i = 0; i < textLength; i++){
        totalCharCompares++;
        state = transitionFunction[state][(text[i] - 1)];
        if(state == 1) totalShifts++;   // We've bounced back to trying to start a pattern-match here; so add to the total shifts
        if(state == patternLength){
            fprintf(output,"%zu, ", i - (patternLength - 1));
            foundAtLeastOne = true;
        }
    }
    

    fprintf(output, "%s] <%u, %u, %u>", foundAtLeastOne?"\b\b":"", preprocessingCost, totalShifts, totalCharCompares);
}




// Knuth-Morris-Pratt String Matching
// Isomorphic to DFAMatch but with a streamlined transition function allowing lower preprocessing cost
// All three matching techniques take the text and pattern as input strings and write their resultant statistics
// directly to the output filestream
// O(n) algorithm with O(m) preprocessing cost
void KMPMatch(char *text, size_t textLength, char *pattern, size_t patternLength, FILE *output){
    // Zero statistics and print initial
    unsigned int totalCharCompares = 0;
    unsigned int totalShifts = 0;
    unsigned int preprocessingCost = 0;

    // Compute the prefix function
    unsigned int prefixFunction[patternLength];
    prefixFunction[0] = 0;
    unsigned int k = 0;
    for(unsigned int q = 1; q < (unsigned int)patternLength; q++){
        while(k > 0 && pattern[k] != pattern[q]){
            k = prefixFunction[k - 1];
            preprocessingCost++;
        }
        if(pattern[k] == pattern[q]) k++;
        prefixFunction[q] = k;
        preprocessingCost++;
    }
 
    // Run the KMP Matcher
    bool foundAtLeastOne = false;
    fprintf(output, "[");

    unsigned int q = 0;
    // For every character in the text
    for(unsigned int i = 0; i < (unsigned int) textLength; i++){
        while(q > 0 && pattern[q] != text[i]){
            q = prefixFunction[q - 1];      
            totalCharCompares++;
        }
        totalCharCompares++;
        if(pattern[q] == text[i]) q++;
        if(q == 1) totalShifts++;   // We've bounced back to attempting to match at this position; add one to the total shifts    
        if(q == patternLength){
            fprintf(output,"%u, ", (i + 1) - (unsigned int)patternLength);
            foundAtLeastOne = true;
            q = prefixFunction[q - 1];    
        }
    }

    fprintf(output, "%s] <%u, %u, %u>", foundAtLeastOne?"\b\b":"", preprocessingCost, totalShifts, totalCharCompares);
}




// Main Function
// Takes the path to the "pattern file" as its first argument and the output path
// to which the statistics table will be written as its second
int main(int argc, char** argv){
    // Verify arguments
    if(argc != 3){
        fprintf(stderr, "Usage: stringMatchStats <pattern file path> <output path>\n");
        exit(1);
    }

    // Open output file for writing
    FILE *output = fopen(argv[2], "w");
    if(output == NULL){
        fprintf(stderr, "Failed to open %s\n", argv[2]);
        exit(2);
    } 

    // Open the pattern file
    FILE *pf = fopen(argv[1], "r");
    char pfbuf[PFBUF_SIZE];
    // Discard the leading hyphen line
    fgets(pfbuf, PFBUF_SIZE, pf);
    // Load the text file path into the buffer
    fgets(pfbuf, PFBUF_SIZE, pf);

    do {
        // Strip the trailing newline
        pfbuf[strlen(pfbuf) - 1] = '\0';
        

        // Map the text file into memory
        // 1) Open the file for reading:
        int fd = open(pfbuf, O_RDONLY);
        if(fd == -1){
            fprintf(stderr, "Failed to open %s\n", pfbuf);
            exit(2);
        }
        fprintf(stderr, "File %s open for scanning.\n", pfbuf);
        fprintf(output, "%s\n", pfbuf);

        // 2) determine file's size:
        struct stat fileStats;
        fstat(fd, &fileStats);

        // 3) map file into memory as page(s)
        char *text = mmap(0, fileStats.st_size, PROT_READ, MAP_SHARED, fd, 0);
        // Now we may access text as an array; a requirement given by the naive algorithm.
        // It's Liebig's Law of the Minimum but for computing...


         
        // Now we run line-by-line over the pattern file until we encounter the closing hyphen line
        while(fgets(pfbuf, PFBUF_SIZE, pf)){
            if(pfbuf[0] == '-') break;

            // For every pattern
            size_t patternLength = strlen(pfbuf) - 1;
            pfbuf[patternLength] = '\0';    // Strip newline
            fprintf(output, "%s, ", pfbuf);        
            fprintf(stderr, "\tScanning for '%s'\n", pfbuf);

            // Run Naive Matching
            naiveMatch(text, (size_t)fileStats.st_size, pfbuf, patternLength, output);
            fprintf(output, ", ");

            // Run DFA Matching
            DFAMatch(text, (size_t)fileStats.st_size, pfbuf, patternLength, output);
            fprintf(output, ", ");

            // Run KMP Matching
            KMPMatch(text, (size_t)fileStats.st_size, pfbuf, patternLength, output);
            fprintf(output, "\n");
        }

        fprintf(stderr, "Statistics stored in %s\n", argv[2]);


        // Unmap and close the text file
        munmap(text, fileStats.st_size);
        close(fd);

        // Output some newlines in preparation for next file
        fprintf(output, "\n\n");
    
    // Repeat if there are more texts to be scanned
    } while (fgets(pfbuf, PFBUF_SIZE, pf));
        

    // Close the pattern and output files
    fclose(pf);
    fclose(output);

}
