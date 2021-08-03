import java.io.*;

// A binomial heap
public class BinomialHeap {


    // Internal Node Class
    private static class Node {
        
        Node parent;        // N.B. As well as serving as a parent pointer in tree traversal, this serves as a backpointer for roots
        Node sibling;
        Node child;
        int key, degree;
        Station station;        // For our Hobson's Trains use case
    
        public Node(int key){
            this.key = key;
            parent = null;
            sibling = null;
            child = null;
            this.degree = 0;
            this.station = null;
        }

        public String toString(){
            return key + " :"+(child == null?"":" c( "+child.toString()+" ) ") + (sibling == null?"":" s[ "+sibling.toString()+" ] ");
        }

        // Creates a fresh copy of this node and its entire subtree
        // Used in Heap Union operations so that the output is separate from its inputs
        public Node uniquize(){
            Node newNode = new Node(this.key);
            newNode.degree = this.degree;
            newNode.station = this.station;
            newNode.parent = this.parent;
            newNode.sibling = (this.sibling == null)?null:this.sibling.uniquize();
            newNode.child = (this.child == null)?null:this.child.uniquize();
            return newNode;
        }
    }




    // The first node in the Heap
    Node head;

    // Theta(1) operation: overloaded constructor for our use case
    // "Data" is the station number. Since we can reach the station properly
    // for zero cost, key is zero.
    public BinomialHeap(Station station){
        head = new Node(0);
        head.station = station;
        head.key = 0;
    } 

    // Theta(1) operation: construct a new blank heap
    public BinomialHeap(){
        head = null;
    }


    public String toString(){
        return head==null?"EMPTY":head.toString();
    }

    // Theta(n) operation:
    // Make a Binomial Heap copy
    public BinomialHeap copy(){
        BinomialHeap out = new BinomialHeap();
        out.head = this.head.uniquize();
        return out;
    }


    // Theta(1) operation:
    // Link two Binomial Trees of same order
    // Internal operation used in Union of Heaps
    private static void binomialLink(Node firstTree, Node secondTree){
        firstTree.parent = secondTree;
        firstTree.sibling = secondTree.child;
        secondTree.child = firstTree;
        secondTree.degree++;
    }



    
    // O(log n) operation:
    // Merge but do not collapse two Binomial Heaps;
    // sorted by degree of tree root nodes. Return the head of the new heap.
    // Internal operation used in Union of Heaps
    private static Node binomialHeapMerge(BinomialHeap firstHeap, BinomialHeap secondHeap){
        Node newHead = null;
        if(firstHeap.head == null && secondHeap.head == null) return null;
        if(firstHeap.head == null) return secondHeap.head.uniquize();
        if(secondHeap.head == null) return firstHeap.head.uniquize();
        
        // Make some pointers into copies of the first and second Heaps
        Node fHeap = firstHeap.head.uniquize();
        Node sHeap = secondHeap.head.uniquize();

        if (fHeap.degree < sHeap.degree){
            // Append tree from first heap and move first heap pointer
            newHead = fHeap;
            fHeap = fHeap.sibling;
        } else {
            // Append tree from second heap and move second heap pointer
            newHead = sHeap;
            sHeap = sHeap.sibling;
        }

        // A pointer to the "tail" of the new heap
        Node tail = newHead;
        
        while (fHeap != null && sHeap != null){
            if (fHeap.degree < sHeap.degree){
                // Append tree from first heap and move first heap pointer
                tail.sibling = fHeap;
                tail = tail.sibling;
                fHeap = fHeap.sibling;
            } else {
                // Append tree from second heap and move second heap pointer
                tail.sibling = sHeap;
                tail = tail.sibling;
                sHeap = sHeap.sibling;
            }
        } // One or both heaps have run out, append what remains of the non-null one(s) in
          // its/their entirety.
        if (fHeap != null) tail.sibling = fHeap;
        if (sHeap != null) tail.sibling = sHeap; 

        return newHead;
    }



    
    // Theta(log n) operation:
    // Generate the union of two Binomial Heaps
    // Merge the heaps, then collapse all redundant trees to maintain Binomial Heap Invariants.
    public static BinomialHeap binomialHeapUnion(BinomialHeap heapOne, BinomialHeap heapTwo){
        BinomialHeap united = new BinomialHeap();
        united.head = binomialHeapMerge(heapOne, heapTwo);
        if (united.head == null) return united;

        // Operation translated from pseudocode provided in class slides (CLRS, apparently, although it's been removed from our edition)
        Node prevX = null;
        Node x = united.head;
        Node nextX = x.sibling;

        while (nextX != null){
            if((x.degree != nextX.degree) || (nextX.sibling != null && (nextX.sibling.degree == x.degree))) {
                prevX = x;
                x = nextX;
            } else {
                // The Lone Key Comparison Step; invert this to change the heap sorting order
                // Right now it's a maxHeap
                if (x.key >= nextX.key){
                    x.sibling = nextX.sibling;
                    binomialLink(nextX, x);
                } else {
                    if (prevX == null){
                        united.head = nextX;
                    } else {
                        prevX.sibling = nextX;
                    }
                    binomialLink(x, nextX);
                    x = nextX;
                }
            }
            nextX = x.sibling;
        }
        
        // Final Theta(log n) traversal to link up root backpointers
        Node ptrTo = null;
        Node cur = united.head;
        while (cur != null){
            cur.parent = ptrTo;
            ptrTo = cur;
            cur = cur.sibling;
        }
        return united;
    }



    // Theta(log n) operation:
    // Locate the maximum value via iteration
    private static Node findMax(BinomialHeap heap){
        Node max = heap.head;
        Node cur = heap.head;
        while (cur != null){
            if (cur.key > max.key) max = cur;
            cur = cur.sibling;
        }
        return max;
    }



    // Theta(log n) operation:
    // Extract the maximum value of the heap iff its key is greater than k;
    // otherwise return -1. Used for our use case (Hobson Trains) and coded here
    // to make the operation more efficient.
    public int selectiveExtractMax(int k){
        // Locate the maximum value  (Theta(log n))      
        Node max = findMax(this);

        // k-check
        if (max == null || max.key <= k){
            return -1;
        }

        // Decouple from heap
        if (max.parent == null) {
            this.head = max.sibling;
        } else {
            max.parent.sibling = max.sibling;
        }
        
        // Reunite orphaned subtrees with main heap (O(log n))
        // Reverse orphans
        Node prev = null;
        Node cur = max.child;
        while (cur != null){
            Node next = cur.sibling;
            cur.sibling = prev;
            prev = cur;
            cur = next; 
        }
        
        BinomialHeap orphans = new BinomialHeap();
        orphans.head = prev;

        // Reunite orphans with original heap (Theta(log n))
        this.head = binomialHeapUnion(this, orphans).head;

        return max.key;
    }


    
    // Recursive Kernel for below operation
    private static void incrementKeysKernel(Node cur){
        if (cur != null){
            cur.key++;
            incrementKeysKernel(cur.sibling);
            incrementKeysKernel(cur.child);
        }
    }

    // Theta(n) operation:
    // Copy the heap, then...
    // Perform a heap traversal, incrementing the key value of every node.
    // This does not (obviously) cause violations of the heap property.
    public BinomialHeap incrementKeys(){
        Node newHead = this.head.uniquize();
        incrementKeysKernel(newHead);
        BinomialHeap newHeap = new BinomialHeap();
        newHeap.head = newHead;
        return newHeap;
    }



    // Recursive Kernel for below operation
    private static int countNodesKernel(Node cur, boolean[] stationSeen){
        if(cur != null){
            int redundancy = 1;
            if (stationSeen[cur.station.id - 1]) redundancy = 0;
            stationSeen[cur.station.id - 1] = true;
            return redundancy + countNodesKernel(cur.sibling, stationSeen) + countNodesKernel(cur.child, stationSeen);
        } else {
            return 0;
        }
    }
    
    // Theta(n) operation:
    // Count the number of nodes in a heap, ignoring repeat Stations (for ring Hobson network)
    public int countNodes(int totalStations){
        boolean[] stationSeen = new boolean[totalStations];
        for(int i = 0; i < totalStations; i++){
            stationSeen[i] = false;
        }
        return countNodesKernel(this.head, stationSeen);
    }

}
