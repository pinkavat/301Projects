#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>

/* editDistance.c
 *
 * Exercise for CSC 301 (Fall 2019) in Dynamic Programming
 * Written by Thomas Pinkava
 */

// Tokenization only responds to certain punctuation characters; a more complete implementation of autocorrect
// would have a more exhaustive token list
#define PUNCTUATION " \n.,:;!?"

// The largest sentence that we will read
#define MAX_SENTENCE_LENGTH 1024

// The longest word we would expect to see in the English Input
#define MAX_WORD_LENGTH 256

// The largest increase in size we would expect to see from a minimum Edit
#define EXPANSION_FACTOR 2


// Hash table data
// The number of length-based buckets -- based on the longest word in our dataset
#define HASH_BUCKETS 512

// A node in our hash table
typedef struct node {
    char* word;
    struct node *L, *R, *N; // Left, Right tree data and N is for storing minima
} node_t;


// Hash function
int hash(char* str){
    return ((strlen(str) << 4) + str[0]) % HASH_BUCKETS;
}


// Add a node to the hash table
void addNode(char *str, node_t **table){
    int index = hash(str);
    node_t* cur = table[index];
    node_t** backPointer = &(table[index]);
    // Perform tree traversal
    while(cur != NULL){
        if (strcmp(str, cur->word) < 0){
            backPointer = &(cur->L);
            cur = cur->L;
        } else {
            backPointer = &(cur->R);
            cur = cur->R;
        }// Discount equality (words do not repeat in the input set)
    }
    *backPointer = (node_t*)malloc(sizeof(node_t));
    (*backPointer)->L = NULL;
    (*backPointer)->R = NULL;
    (*backPointer)->N = NULL;
    (*backPointer)->word = strdup(str);
}


// Check if a word is present in our hash table
bool contains(char *str, node_t **table){
    int index = hash(str);
    node_t *cur = table[index];
    // Perform tree traversal
    while(cur != NULL){
        if(strcmp(str, cur->word) == 0){
            // Match
            return true;
        } else if (strcmp(str, cur->word) < 0){
            cur = cur->L;
        } else {
            cur = cur->R;
        }
    }
    return false;
}





// editDistance
// Returns the Levenshtein distance between strings s1 and s2
// If silent is true, prints nothing. Otherwise, it prints the Dynamic Program Memo table
// line-by-line during its iteration (as, to save on space costs, only two lines of the table
// exist in memory at any time)
int editDistance(char* s1, char* s2, bool silent){
    // Get the sizes of s1 and s2
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    // This algorithm uses a Dynamic Memo Table to store the minimum number of Levenshtein string
    // operations required to convert s1 to s2 or vice versa. For space efficiency, only two rows
    // of the Table actually exist.
    int *prevRow = malloc(sizeof(int) * (len2 + 1));  // Reserve space for an epsilon char
    int *curRow = malloc(sizeof(int) * (len2 + 1));

    // Prepopulate prev with the number of operations required to reduce each prefix to epsilon
    for(int i = 0; i <= len2; i++) prevRow[i] = i;

    // If we're not in silent mode, print out a header row with the characters of s2
    if (!silent){
        printf("        | ");
        for(int i = 0; i < len2; i++) printf("%3c | ", s2[i]);
        printf("\n");
    }

    // Iterate downwards over each row
    for(int i = 0; i < len1; i++){
        // It takes i + 1 operations to convert epsilon to the current prefix (left column)
        *curRow = i + 1;

        // Move across the row
        for(int j = 0; j < len2; j++){
            // Compute the costs for the various Levenshtein Edits
            int costDel = prevRow[j + 1]+1;
            int costIns = curRow[j] + 1;
            int costSub = (s1[i] == s2[j]) ? prevRow[j] : prevRow[j] + 1;

            // Store the cost of the minimum edit series as the cost for editing this prefix
            // I have a fondness of ternary operators for some reason
            curRow[j + 1] = (costDel < costIns) ? ((costDel < costSub) ? costDel : costSub) : ((costIns < costSub) ? costIns : costSub);
        }

        // Move down a row (i.e. prevRow := curRow)
        // If we're not in silent mode, print out prevRow before we throw it away
        if (!silent){
            printf("%c | ", (i == 0) ? ' ' : s1[i - 1]);
            for(int k = 0; k <= len2; k++) printf("%3d | ", prevRow[k]);
            printf("\n");
        }
        // Perform a pointer swap to save on physical copy cost
        int *temp = prevRow;
        prevRow = curRow;
        curRow = temp;

    }
    // Finally, if we're not in silent mode, print out the last row (as the swap has happened, this is still prevRow)
    if (!silent){
        printf("%c | ", s1[len1 - 1]);
        for(int k = 0; k <= len2; k++) printf("%3d | ", prevRow[k]);
        printf("\n");
    }

    // Our Levenshtein distance is the bottom-rightmost element of the matrix
    int totalCost = prevRow[len2];
    free(prevRow);
    free(curRow);
    return totalCost;
}






// Helper for below function
// Returns length of minimum
void minEditDistanceKernel(char *str, node_t *cur, int* minDist, node_t** minHead){
    if (cur != NULL){
        // Reset minimum data
        cur->N = NULL;
        minEditDistanceKernel(str, cur->L, minDist, minHead);
        minEditDistanceKernel(str, cur->R, minDist, minHead);

        int dist = editDistance(str, cur->word, true);
        if(dist < (*minDist)){
            // A new minimum has been found. Orphan the old minima and begin anew.
            *minDist = dist;
            *minHead = cur;
        } else if (dist == (*minDist)){
            // Prepend this alternative to the current list of minima
            cur->N = *minHead;
            *minHead = cur;
        }
    }
}

// Iterate over the entire hash table, seeking words with minimum edit distances
// to the indicated string. Forms a built-in linked list over the minima.
node_t *minimumEditDistances(char *str, node_t **table, int* minDist){
    // Find minima
    node_t *minHead = NULL;
    for(int i = 0; i < HASH_BUCKETS; i++){
        // Perform recursive tree traversal (high stack space cost, I suppose)
        minEditDistanceKernel(str, table[i], minDist, &minHead);
    }
    return minHead;    
} 




// Main function
// If there are two arguments the program prints the table and Levenshtein distance between the arguments
// Otherwise it treats stdin as a set of words to be autocorrected
int main(int argc, char** argv){
    if (argc == 3){
        // Two-word compute mode
        printf("Distance between %s and %s: %d\n", argv[1], argv[2], editDistance(argv[1],argv[2], false));
    } else if (argc == 1 || (argc == 2 && strcmp(argv[1], "-c") == 0)){
        // Stdin autocorrect mode
        // Cheap and easy read of stdin for short test sentences (the assignment is not about parsing input after all!)
        char str[MAX_SENTENCE_LENGTH + 1];
        str[read(STDIN_FILENO, &str, MAX_SENTENCE_LENGTH)] = '\0';

        // Read in the entire English Dictionary
        // TODO: turn this into a memory-mapped custom data structure for efficient loading

        // Make Hash Table
        node_t **table = calloc(HASH_BUCKETS, sizeof(node_t*));
        
        printf("Please wait, loading English Dictionary into hash table...\n");
        // Iterate over Dictionary
        char buffer[MAX_WORD_LENGTH];
        FILE *fp = fopen("words.txt", "r");
        if (fp == NULL) perror("Couldn't get words.txt"); 
        while (fgets(buffer, MAX_WORD_LENGTH, fp)) {
            int len = strlen(buffer);
            if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';    // Trim off trailing newline
            // Convert to Lowercase
            for(int i = 0; buffer[i]; i++) buffer[i] = tolower(buffer[i]);
            // Append the word to the Hash Table
            addNode(buffer, table);
        }
        fclose(fp);
        printf("English Dictionary Loaded.\n");

        if(argc == 1){
            // String Correction Mode
            char output[strlen(str) * EXPANSION_FACTOR];    // Prepare an output buffer
            int o = 0;                                      // Index into the output
            output[o] = '\0';
            
            // Iterate over the input
            for(int w = 0; w < strlen(str); ){
                if(strchr(PUNCTUATION, str[w]) != NULL){
                    // Do not modify punctuation
                    output[o++] = str[w++];
                    output[o] = '\0';
                } else {
                    // Send ahead a tail-reader to read whole words
                    for(int t = w; t < strlen(str); t++){
                        if(strchr(PUNCTUATION, str[t]) != NULL){
                            // We've reached the end of a word
                            char* word = strndup(&str[w], t - w);
                            
                            // Convert to Lowercase
                            for(int i = 0; word[i]; i++) word[i] = tolower(word[i]);

                            if(contains(word, table)){
                                // Append existing word
                                strcat(output, &str[w]);
                                o += t - w; 
                            } else {
                                // Append replacement word (doesn't match case; TODO fix this)
                                int minimumDistance = INT_MAX;
                                char* replacement = minimumEditDistances(word, table, &minimumDistance)->word;
                                strcat(output, replacement);
                                o += strlen(replacement);
                            }

                            free(word);
                            w = t;
                            break;
                        }
                    }
                }
            }
            
            printf("%s\n", output);
        } else {
            // Complete Correction Mode
            // Iterate word-by-word over the input
            printf("Checking string: %s\n", str);
            // Tokenize string into words
            char *word = strtok(str, PUNCTUATION);
            while(word != NULL){
                // For every word

                // Convert to Lowercase
                for(int i = 0; word[i]; i++) word[i] = tolower(word[i]);
                // N.B. the implementation is incomplete for composite number-words like "55th" due to the limitations of the
                // data-set and the fact that we do not treat words as composites of subwords. A more complete implementation
                // would address this problem.

                // Scan the Hash Table to see if the word exists
                if(!contains(word, table)){
                    printf("%s is not in the dictionary, did you mean ", word);
                    // Find possible alternatives
                    int minimumDistance = INT_MAX;
                    node_t* minHead = minimumEditDistances(word, table, &minimumDistance);

                    // Print the found minima
                    while(minHead != NULL){
                        printf("%s", minHead->word);
                        if (minHead->N != NULL) printf(" or ");
                        minHead = minHead->N;
                    }

                    printf("?\n");
                }
                // Next word
                word = strtok(NULL, PUNCTUATION);
            }
        }

        // Leak the Hash Table's memory; as loading is a onetime operation, we can let the OS clear up our mess
        printf("\n");

    } else {
        // Help message
        fprintf(stderr, "Usage:\n\teditDistance <word1> <word2> for Comparison Mode\n\teditDistance [-c] for String Correction mode.\n\tIn String Correction Mode, pass in the sentence to be corrected to stdin. If -c is specified,\n\tprogram will print all relevant correction alternatives. If not it will print only one corrected sentence.\n");
        return 5;
    }
    return 0;
}
