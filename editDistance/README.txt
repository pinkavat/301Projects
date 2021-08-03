ASSIGNMENT 04: SPELLING CORRECTION
Thomas Pinkava [pinkavat]

Section 1: Problem Description
==============================
This assigment is divided into two parts: Computing the Levenshtein distance between two strings and using this computation
to correct misspelled words. The latter is a simple question of comparing each word to a dictionary of correct words, and
finding those words with a minimum edit Distance from the misspelled words. The former is a more complex problem that
requires a Dynamic approach to run in acceptable time.

Levenshtein distance is the number of Levenshtein operations required to convert one string into another string.
The Levenshtein operations are insertion of a symbol, deletion of a symbol, and substitution of a symbol. The
assignment does not require that we reconstruct the edits employed to convert the strings, so a single Dynamically Programmed
Table containing the number of operations will suffice. The problem can be solved in the same way as LCS; that is, we compute
the Levenshtein Distances between each prefix and memoize them, so that as we expand the prefixes to the full strings, we need
not recompute distances.

The solution, therefore, is construction of a 2D matrix such that cell[x, y] contains the Levenshtein distance between the
prefix of the first string of length x and the prefix of the second string of length y. We can populate the table as follows:
    1) For each row, we can convert the prefix at [x] into the prefix at [x-1] by deleting a symbol. The distance of the prefix,
        therefore, is [x-1, y] + the cost of deletion (which is 1 in the problem's description).
    2) For each column, the same principle applies; to convert the prefix at [y] to the prefix at [y + 1] we need to insert a
        symbol.
    3) Substitution can be simply considered as insertion plus deletion; its cost, therefore, is the cost of the cell at
        [x - 1, y - 1] (both insert and delete) plus the substitution cost, which is 1 if the symbol changes and 0 if it does not.
Once we have populated the entire matrix, we know that the bottom-rightmost cell contains the distance between the prefixes of each
string that are of the length of that string, which are just the strings themselves.



Section 2: Analysis and Description of Code
===========================================
My Code is divided into three parts:
    1) Hash Table Functionality [addNode(), contains(), hash(), minimumEditDistances()]
        This code is elementary Hash Table funcionality for a fixed-width Table of Binary Trees.
        Each word from the dictionary is loaded into this table. In order to check if a word is correct,
        the Hash Table must simply be searched (O(1) operation, at least in theory; the actual factors involved means
        it is closer to a divided O(log n) for a forest of search trees, but it is still fast.).
        In order to compute the minimum Edit Distances, the entire Table must be iterated over (O(n) operation). I
        considered some method for reducing this cost, such as meta-Dynamic programming computation of Levenshtein
        Distances between every two words in the Dictionary, but realized that such a computation might lead to incorrect
        minima. The Hash Function is a trivial shift-and-add on the size of the word and its first character. A future improvement
        would involve linguistic analysis of the data set to tailor a collision-minimizing function.
        Another thing to note is that as the minimumEditDistances() function recurses over every node in each bucket-tree, it has
        linear space complexity. A possible improvement would be iterative traversal, as this would require only constant space
        (but at the cost of adding backpointers and flags and greatly obfuscating the code).

    2) Driver [main()]
        This code reads user input and either invokes an editDistance calculation over two strings, or reads and corrects the
        standard input. The iteration is O(n); if a word is correct its conversion cost is O(1) and if it is incorrect it is
        O(m) * editDistance Cost (see below) where m is the size of the dictionary. Therefore we have a linear best case and
        a polynomial worst case. For sentence correction, the algorithm will print a readout of possible substitutions if in
        complete mode, and will print the original string with the first minimum-distance correct string for each incorrect
        string replaced in noncomplete mode.

    3) Levenshtein Algorithm [editDistance()]
        This function computes the Levenshtein Distance between two strings, using the method described in Section 1.
        As the Table Computation relies only on the current row and the row above it, all other rows can be discarded once
        they pass out of relevant scope. Since the assignment specifies that we must print the table, the function can be
        called in table-printing or non-table-printing mode. By reducing the number of rows to 2, the space complexity of the
        function depends only on the size of one of the input strings (i.e. it is linear and not polynomial space).
        The function iterates only once over every prefix permutation, and there are m * n such permutations (where m and
        n are the lengths of the input strings + 1 for an empty string representation). Thus we can say our edit distance
        calculation is of O(n^2) if the strings are relatively close in size.

    The time cost of the single comparison is just the cost of the editDistance function; O(n^2).
    The time cost of the string replacement is, in the worst case in which all words are misspelled:
        O(words in input) * O(words in dictionary) * editDistance cost
    This would appear to be O(n^4), but, as the input string is probably an order of magnitude or two smaller than the
    English dictionary (in fact, my code only allocates a small buffer for the input string, so this condition is
    enforced by necessity) it is more appropriate to say the algorithm is O(n^3).
    If we could establish a subproblem relationship between Levenshtein distances, then we could reduce this to O(n^2)
    through meta-dynamic programming. However, such an approach would risk the algorithm preferring a precomputed path
    of higher cost than a recomputed path of unknown lower cost, so it does not seem like such an optimization is possible.



Section 3: How to Run
=====================
Compile editDistance.c with a C compiler such as gcc.
Procure a dictionary of English words in plain text format, with one word per line (as in the files provided in the
assigment specification). Call this file words.txt and place it in the same directory as the executable.

1) If you want to measure the Edit Distance between two words, pass them in as the only arguments to the executable.
    (i.e.  editDistance <word1> <word2>)
2) If you want to have a sentence or sentences corrected, run the executable with no arguments, passing the sentences into
    the standard input.
3) If you want to see every minimum alternative to each misspelled word, call the executable as in 2) but with argument
    -c. 



Section 4: Sample Input/Output
==============================

Example of two-word comparison mode:

user: ~/path/$ ./editDistance levenshtein algorithm
        |   a |   l |   g |   o |   r |   i |   t |   h |   m | 
  |   0 |   1 |   2 |   3 |   4 |   5 |   6 |   7 |   8 |   9 | 
l |   1 |   1 |   1 |   2 |   3 |   4 |   5 |   6 |   7 |   8 | 
e |   2 |   2 |   2 |   2 |   3 |   4 |   5 |   6 |   7 |   8 | 
v |   3 |   3 |   3 |   3 |   3 |   4 |   5 |   6 |   7 |   8 | 
e |   4 |   4 |   4 |   4 |   4 |   4 |   5 |   6 |   7 |   8 | 
n |   5 |   5 |   5 |   5 |   5 |   5 |   5 |   6 |   7 |   8 | 
s |   6 |   6 |   6 |   6 |   6 |   6 |   6 |   6 |   7 |   8 | 
h |   7 |   7 |   7 |   7 |   7 |   7 |   7 |   7 |   6 |   7 | 
t |   8 |   8 |   8 |   8 |   8 |   8 |   8 |   7 |   7 |   7 | 
e |   9 |   9 |   9 |   9 |   9 |   9 |   9 |   8 |   8 |   8 | 
i |  10 |  10 |  10 |  10 |  10 |  10 |   9 |   9 |   9 |   9 | 
n |  11 |  11 |  11 |  11 |  11 |  11 |  10 |  10 |  10 |  10 | 
Distance between levenshtein and algorithm: 10



Example of incomplete string correction mode:

user: ~/path/$ ./editDistance
Behold the H'nH'nmptnaaakulorx of ZZavoorezeeeem!
Please wait, loading English Dictionary into hash table...
English Dictionary Loaded.
Behold the noncompensatory of cavo-relievos!



Example of complete string correction mode:

user: ~/path/$ ./editDistance -c
My nobblex motrudes ovest nat yebble ka!
Please wait, loading English Dictionary into hash table...
English Dictionary Loaded.
Checking string: My nobblex motrudes ovest nat yebble ka!

nobblex is not in the dictionary, did you mean nobbled or nobbler or nobbles or nobble?
motrudes is not in the dictionary, did you mean protrudes or obtrudes or intrudes or extrudes or detrudes?
yebble is not in the dictionary, did you mean yabble or pebble?





Section 5: Experience
=====================

This assignment was an excellent illustration of the sunk-cost fallacy. My partner and I wrote Assignment 03 in Java,
because the dual requirement of coding a heap and a graph meant object-orientation was a desirable feature.
My preferred language, however, is C; as I was working on this Assignment alone I decided to use it. The Levenshtein
Algorithm itself is trivial to implement in C. Instead of a library I wrote my own Hash Table (which is simple and
therefore very inefficient). These algorithms were not onerous. Where the overwhelming majority of my work went was
string manipulation.

Since I had invested time in writing the previous algorithms in C, I was reluctant to switch to a more benevolent
language when it came time to write the driver code (which would parse the input sentences). Writing code to detect
the erroneous words was not too difficult (tokenizing the string with strtok was convenient); this is why my algorithm
has the unrequested -c feature. Correcting the input string, on the other hand, proved to be delightfully annoying.
There are few operations more masochistic in C than string manipulation.

The code could have a few conceptual improvements from which it would greatly benefit. In most autocorrect applications,
the strings being corrected are being handed in by a user (i.e. the algorithm is on-line) so loading the dictionary but once
at the beginning of listening for input would suffice. My algorithm loads the entire dictionary every time it is executed;
more egregiously, it reinserts every element into the hash table, where an alternative algorithm could probably save the
table data itself as a memory-mapped chunk.

Another improvement would be practical: employing multithreading for loading and analyzing words. Since the hash table
is static and unchanging it is thread-safe to iterate over, so any multi-core computer could analyze the correctness of 
multiple words at a time.
