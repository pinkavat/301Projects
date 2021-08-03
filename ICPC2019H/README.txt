Collaborators:
    Thomas Pinkava
    Nathan Williams

Solver for the ICPC 2019 problem H: Hobson Trains

Code written in Java; compile main class HobsonTrains.java with javac or similar utility;
then run resulting HobsonTrains.class, with the input specified by the problem provided to the
program's standard input.


Runtime Analysis:
    Code contains multifarious subordinate functions. The runtime of each function is noted above it header.
    Most of these runtimes are trivially computed. Theta(n) functions, for instance, are those which
    traverse the binomial heap or Station network structures, touching each node once, or repeating nodes
    by some factor unrelated to the number of nodes (branching factor of heap, incident Station edges, etc.).
    In other cases the runtime is already essentially known; the Binomial Heap union is known to run 
    in logarithmic time and our implementation is essentially a transcription of the CLRS pseudocode
    provided in the lecture slides.
    
    The functions of nontrivial time are the five components of HobsonTrains.java's main function:
        1) Input processing is, of course, linear on the size of the input.

        2) Function nazgul() (so called because it seeks out and finds the ring(s) wherever they may be)
            recursively traverses the station graph, starting from the "leaves" and propagating inwards.
            The runtime is computed in the following way: for every tree in the graph, any non-leaf node
            will be added to nazgul()'s internal node-processing queue at most a logarithmic number of times,
            as the nodes are guaranteed to be eliminated each step (i.e. the queue gets smaller, and successors
            that once would have been added are no longer added). Essentially this is an iteration from leaf to
            root of each tree, plus the "error bactracking rate" which varies by graph shape but is guaranteed
            to be less than n.

        3) Function treePropagate() propagates connection values recursively across an entire single tree.
            Essentially, for every node in the tree, it performs Binomial Heap Union (known to be log n) with
            the heaps of its subtrees, which are determined by performing Heap Union with their subtrees, and
            so forth. Unlike nazgul(), this is, independently of the shape of the graph, guaranteed to only 
            occur n times, to the cost is Theta(n log n).

        4) Function ringPropagate() is a little more finicky. Once treePropagate has run, each node in a ring now
            has up-to-date connection data on its tree. Then, it recursively unites its heap with every ancestor
            in the ring save itself (the structure of the graph guarantees that members of a ring are their own
            ancestors). This is a linear traversal on the size of the ring, times the logarithmic cost of
            performing heap union. The function generates a lot of redundant paths; in a better implementation
            we would come up with a cleverer scheme to mitigate this, but, as time pressed and the Output function
            performs perfectly adequate redundancy checking, we chose to generate these redundancies as the overhead
            is not onerous. 

        5) Outputting is a traversal of every station in the graph (Theta(n)) with redundancy checking; which is
            essentially a heap walk. Since redundancy is checked through Theta(1) array accesses, this part is
            linear on the size of all Binomial Heaps of all Stations.

    The runtime of our algorithm is thus the sum of the costs of these subalgorithms, so:
        Theta(n) + Theta(n) + Theta(nlogn) + Theta(nlogn) + Theta(n) = Theta(n log n)
    which falls into the acceptable category of runtimes for this assignment.
