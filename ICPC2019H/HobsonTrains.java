import java.io.*;
import java.util.*;

public class HobsonTrains {

    /* Hobson Trains Problem Solver
     *
     * Binomial Heap-backed solver for ICPC 2019 Problem H (post factum class exercise)
     */
    private static Station[] stations;  // The set of nodes in our graph
    private static int n;               // The number of stations
    private static int k;               // The longest permissible path

    
    // Theta(n) operation
    // Finds the ring(s) in the Hobson network
    private static void nazgul(){
        // Make a queue
        LinkedList<Station> w = new LinkedList<Station>();
        // Add all leaves to queue
        for(Station s: stations){
            if (s.incomingEdges.size() == 0){
                w.add(s);
                s.isTree = true;
            }
        }
        // Run over Queue
        while(w.size() > 0){
            Station x = w.pop();
            boolean makeTree = true;
            for(Station z: x.incomingEdges){
                if (!(z.isTree)) makeTree = false;
            }
            if (makeTree) {
                x.isTree = true;
                if (x.outgoingEdge != null) w.addLast(x.outgoingEdge);
            }
        }
    }

    

    // Kernel for the below function
    // Returns the heap of the indicated station, united with all stations below it,
    // incremented, and pruned appropriately
    private static BinomialHeap treePropagateKernel(Station s){
        // Unite with children
        for(Station child: s.incomingEdges){
            if (child.isTree) {
                s.heap = BinomialHeap.binomialHeapUnion(s.heap, treePropagateKernel(child).incrementKeys());
            }
        }
        // Prune
        while(s.heap.selectiveExtractMax(k)!=-1);
        return s.heap;
    }


    // Theta(n log n) operation:
    // Propagate connection data for Tree-type nodes
    private static void treePropagate(){
        // Step 1: add all "Root" nodes to a queue [Theta(n) operation]
        LinkedList<Station> roots = new LinkedList<Station>();
        for(Station s: stations){
            if (s.outgoingEdge == null || !(s.isTree)){
                // We've hit a root (i.e. it doesn't point to another tree)
                roots.add(s);
            }
        }
        // Now recursively combine the binomial heap data for this root from its children:
        for(Station root: roots){
            root.heap = treePropagateKernel(root);
        }
    }


    // Kernel for the below function
    // Returns the heap of the indicated station, united with all stations below it, unless
    // that station is the initial station
    private static BinomialHeap ringPropagateKernel(Station cur, Station initial){
        Station next = null;
        for(Station z: cur.incomingEdges){
            if (!z.isTree) next = z;
        } // Now next points to the ring ancestor of cur
         
        if(next != initial) cur.heap = BinomialHeap.binomialHeapUnion(cur.heap, ringPropagateKernel(next, initial).incrementKeys());

        // Prune
        while(cur.heap.selectiveExtractMax(k) !=-1);
        return cur.heap;
    }
    
    // Theta(n log n) operation:
    // Propagate connection data for Ring-type nodes
    // Assumes ring nodes have been populated with data from their incoming trees
    private static void ringPropagate(){
        // Step 1: find all Ring Nodes [Theta(n) operation]
        LinkedList<Station> ringStations = new LinkedList<Station>();
        for(Station s: stations){
            if (!s.isTree) ringStations.add(s);
        }
        // Note: we could decrease redundancy here by maintaining clean new heaps instead
        // of overwriting our existing heaps over and over and over. It doesn't mess with
        // our asymptotic bounds, though, so it's not really worth the brainpower for this
        // assignment. 
        
        // Step 3: populate
        // These heaps may contain redundancies; these are taken care of later.
        for(Station s: ringStations){
            s.heap = ringPropagateKernel(s, s);
        }
    }

    

    public static void main(String[] args){
        // Step 1: Parse Input
        // Using Scanner for Getting Input from User
        Scanner in = new Scanner(System.in);
        n = in.nextInt();
        k = in.nextInt();

        stations = new Station[n];
        for (int i = 0; i < n; i++) {
            stations[i] = new Station(i+1);
        }
        for (int i = 0; i < n; i++) {
            stations[i].link(stations[in.nextInt()-1]);
        }
         
        // Step 2: Determine which nodes are Ring nodes (i.e. might point to themselves through some other path) and which are Tree (can't)
        nazgul();
        
        // Step 3: Propagate path lengths for Tree Nodes
        treePropagate();
  
        // Step 4: Propagate path lengths for Ring Nodes
        ringPropagate();
        
        // Step 5: Output (countNodes corrects overcounted Ring Nodes)
        for (int i=0; i < n; i++){
            System.out.println(stations[i].heap.countNodes(n));
        } 
    }
}
