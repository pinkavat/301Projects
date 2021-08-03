#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* flow_network_generator.c
 *
 * Written November 29 2019 by Thomas Pinkava for CSC 301
 * An exercise in flow networks
 */

// Graph sparsity controls
// Percent probability of forward edge being placed
#define FORWARD_EDGE_PROBABILITY 40

// Percent probability of back edge being placed
#define BACK_EDGE_PROBABILITY 10

// Maximum edge capacity
#define MAX_EDGE_CAPACITY 20


// The number of times the generator will try to make a valid graph before copping out and
// adding an edge from source to sink
#define COP_OUT_LIMIT 5


// The longest .dot description we would expect to see for any given edge of the network
// derived from our precondition that 5 <= |V| <= 15 and assumption that |E| <= |V|^2
// and the MAX_EDGE_CAPACITY (though doing so at compile-time would mean involving logarithms, 
// which would be annoying). Also includes space for null-termination.
#define MAX_EDGE_STRING_LENGTH 29



// An Adjacency Matrix
typedef struct adjmatrix {

    size_t vertices;            // The number of vertices in the graph
    unsigned int *matrix;       // matrix[(u * vertices) + v] is the capacity of edge u->v (0 == no edge)

} adjmatrix_t;



// Helper for generate_flow()
// Recursive BFS, returns true if the sink is found and false otherwise
bool recursiveBFS(unsigned int vertex, adjmatrix_t *flownet, bool* reachable){
    // Flag this vertex as being reachable
    reachable[vertex] = true;
    // Recurse over outgoing edges
    for(int i = 0; i < flownet->vertices; i++){
        if(flownet->matrix[(vertex * flownet->vertices) + i]){
            // There's an edge here
            if(i == flownet->vertices - 1){
                // It's to the sink
                return true;
            } else if(!(reachable[i]) && recursiveBFS(i, flownet, reachable)){
                return true;
            }
        }
    }
    return false;
}



// generate_flow()
// Populates the adjacency matrix it is given with a flow network
// Preconditions as specified in the assigment are assumed
// Source is arbitrarily vertex zero, Sink is arbitrarily vertex n-1
void generate_flow(adjmatrix_t *flownet){
    int z = 0;
    do {
        int e = 0;
        for(int i = 0; i < flownet->vertices - 1; i++){ // No edges departing sink
            for(int j = 1; j < flownet->vertices; j++){ // No edges entering source
                // For every edge (i, j)
                if(i < j &&((rand() % 100) < FORWARD_EDGE_PROBABILITY)){

                    // Forward edge, and RNG says place edge
                    flownet->matrix[(i * flownet->vertices) + j] = (rand() % MAX_EDGE_CAPACITY) + 1;
                    e++;

                } else if(i > j &&((rand() % 100) < BACK_EDGE_PROBABILITY)) {

                    // Back edge, and RNG says place edge
                    flownet->matrix[(i * flownet->vertices) + j] = (rand() % MAX_EDGE_CAPACITY) + 1;
                    e++;

                } // No self-loops
            }
        }
        
        // In order to guarantee that |E| >= |V| - 1 as per our Preconditions, we add edges as necessary.
        if(e < flownet->vertices - 1){
            // Any non-sink vertex is guaranteed at least one output edge.
            for(int i = 0; i < flownet->vertices - 1; i++){ // For all non-sink vertices
                bool addEdge = true;
                for(int j = 0; j < flownet->vertices; j++){
                    // For every edge (i, j)
                    if(flownet->matrix[(i * flownet->vertices) + j] > 0){
                        addEdge = false;
                        break;
                    }
                }
                if (addEdge){
                    // Don't add edges pointing to source to avoid self-loop
                    flownet->matrix[(i*flownet->vertices) + ((rand() % (flownet->vertices - 1)) + 1)] = (rand() % MAX_EDGE_CAPACITY) + 1;
                }
            }
        }


        // Verify that at least one path exists from source to sink
        // Make array of whether a vertex is "accessible" from source and BFS
        bool reachable[flownet->vertices];
        memset(reachable, 0, sizeof(bool) * flownet->vertices);
        if(recursiveBFS(0, flownet, reachable)) return;

    } while(++z < COP_OUT_LIMIT);   // Give up if we can't find a valid flow network in n turns

    // ...and cheat, adding an edge from source to sink
    flownet->matrix[flownet->vertices - 1] = (rand() % MAX_EDGE_CAPACITY) + 1;    

}





// Main Function
// Program takes three arguments (the file path to and name of the output and a seed) and produces 
// a randomly generated flow network as a simple DOT file at the indicated path
// (as specified in the assignment description)
int main(int argc, char **argv){

    // Verify argument
    if (argc != 4){
        fprintf(stderr, "Usage: flow_network_generator <graph name> <path> <seed>\n");
        exit(1);
    }


    // Seed RNG with input parameter (timing is to close to use time seed)
    srand(atoi(argv[3]));
    

    // Allocate new adjacency matrix describing our Flow Network
    adjmatrix_t flownet;
    flownet.vertices = (rand() % 10) + 5;
    fprintf(stderr, "Generating a flow network with %zu vertices\n", flownet.vertices);
    flownet.matrix = (unsigned int *)calloc(flownet.vertices * flownet.vertices, sizeof(unsigned int));
    

    // Generate Flow Network
    generate_flow(&flownet);


    // Convert Flow Network edges to .dot list format string
    // As per our spec, 5 <= |V| <= 15, so we have a manageable
    // upper boundary on the buffer
    // Increment counters so that we have 1-base indexed vertices
    char buffer[flownet.vertices * flownet.vertices * MAX_EDGE_STRING_LENGTH];
    char *bufHead = buffer;
    for(int i = 0; i < flownet.vertices; i++){
        for(int j = 0; j < flownet.vertices; j++){
            // For every edge (i, j)
            unsigned int edgeCapacity = flownet.matrix[(i * flownet.vertices) + j];
            if (edgeCapacity != 0){
                // An edge exists; append it to our output string
                bufHead += snprintf(bufHead, MAX_EDGE_STRING_LENGTH, "%d -> %d [label = \" %d \"];\n", i+1, j+1, edgeCapacity);
            }
        }
    }
    
    

    FILE *fp = fopen(argv[2], "w");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open %s for writing\n", argv[2]); 
        exit(2);
    }


    // Write .dot skeleton fleshed with our graph to the file
    fprintf(fp, "digraph g{\n\nrankdir = LR\n\n%s\nlabel = \"%s\"\n}\n", buffer, argv[1]);


    // Close the file
    fclose(fp);

    
    // Clean up
    free(flownet.matrix);
}
