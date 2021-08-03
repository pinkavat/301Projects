301 Assignment 07
Thomas Pinkava



Problem Overview
=========================

In this Assignment, we were tasked with empirically evaluating the performance of
three String-Matching algorithms: a Naive Traversal, a Deterministic Finite Automaton, and
the Knuth-Morris-Pratt Algorithm. The task was therefore to implement these three algorithms
and run them to match strings in a corpus, measuring statistics about the performance of each
implementation.




Compilation and Execution
=========================

Compile stringMatchStats.c with a C compiler. The executable runs as follows:

    >./stringMatchStats <input pattern file path> <output path>

The pattern file is of the type specified in the Assignment on Pweb. It is a plain
text file consisting of a line of hyphens (of length >= 1 but otherwise arbitrary)
followed by the path to a text to search on the next line, followed by patterns to search
for in that text on the subsequent lines. A final line of hyphens marks the end of the
text to scan, and can be followed by either the end-of-file or another path to another
text to search and so on. See patterns.txt for a working example.

The output path will then be written to with the statistics resulting from these matches,
as specified in the Assignment description. Tabulation is set by the CSV standard, so the
output can be declared to be a .csv file and viewed with any application that can handle
CSV (Any competent spreadsheet application can do so). The table is broken into segments,
each headed by the file path of the text from which it draws. Each line's leftmost column
is the pattern searched for, followed by the statistics for Naive Traversal, DFA, and KMP
in that order in the subsequent columns. Each statistic is of the format

    [list of matching shifts] <Preprocessing cost, Number of Shifts, Total Character Comparisons>

N.B.: The pattern matcher matches ALL ASCII chars save \0, so it is case-sensitive.




Sample IO
=========================

Text File: ozymandias.txt

I met a traveller from an antique land,
Who said-"Two vast and trunkless legs of stone
Stand in the desert. Near them, on the sand,
Half sunk a shattered visage lies, whose frown,
And wrinkled lip, and sneer of cold command,
Tell that its sculptor well those passions read
Which yet survive, stamped on these lifeless things,
The hand that mocked them, and the heart that fed;
And on the pedestal, these words appear:
My name is Ozymandias, King of Kings;
Look on my Works, ye Mighty, and despair!
Nothing beside remains. Round the decay
Of that colossal Wreck, boundless and bare
The lone and level sands stretch far away."
    -- Percy Bysshe Shelley


Pattern File: testPattern.txt

-------------------
ozymandias.txt
on
Nothing
Shelley
King
-------------------


Execution:

    >./stringMatchStats testPattern.txt output.txt

    File ozymandias.txt open for scanning.
        Scanning for 'on'
        Scanning for 'Nothing'
        Scanning for 'Shelley'
        Scanning for 'King'
    Statistics stored in output.txt 


Output File: output.txt

ozymandias.txt
on, [83, 119, 264, 300, 381, 461, 586] <0, 652, 683>, [83, 119, 264, 300, 381, 461, 586] <5, 29, 0>, [83, 119, 264, 300, 381, 461, 586] <1, 29, 676>
nothing, [] <0, 647, 692>, [] <36, 38, 0>, [] <7, 38, 692>
Shelley, [645] <0, 647, 661>, [645] <35, 2, 0>, [645] <6, 2, 655>
king, [] <0, 650, 661>, [] <14, 7, 0>, [] <3, 7, 661>




Execution on the provided Corpus
=========================

We were provided a corpus of news stories relating to the Deepwater Horizon spill to test the algorithm. The patterns chosen are available in patterns.txt
and the resulting statistics are available in output.txt.

As can be seen from the data, the Naive Method has zero preprocessing cost, but compares every character in the pattern for every shift, and shifts for every
character in the text, resulting in O(mn) runtime. The DFA and KMP algorithms have similar shifts and compares, as they are both essentially the same 
mechanism, but KMP's streamlining of the automaton structure grants it a vastly cheaper preprocessing cost (in this implementation, all ASCII characters were
made available as symbols for the DFA, so the O(m|sigma|) preprocess cost is exacerbated for visibility).

The algorithms have an extremely fast practical runtime, mostly due to the direct memory-mapping of the text files, which amortizes I/O cost.