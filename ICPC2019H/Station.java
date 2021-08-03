import java.util.ArrayList;

public class Station {
    
    int id;

    public Station outgoingEdge;
    public ArrayList<Station> incomingEdges;
    public BinomialHeap heap;

    public boolean isTree;

    // Theta(1) operation; create a new Station
    public Station(int id){
        this.id = id;
        outgoingEdge = null;
        incomingEdges = new ArrayList<Station>();
        heap = new BinomialHeap(this);    // Start with a single self-pointing connection
        isTree = false;
    }

    // Theta(1) operation; point self's leg at target Station
    public void link(Station target){
        this.outgoingEdge = target;
        target.incomingEdges.add(this);
    }

}
