#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* max_flow_generator.c
 *
 * Written November 30 2019 by Thomas Pinkava for CSC 301
 * An exercise in the Edmonds-Karp Algorithm
 */

// The maximum number of characters we would encounter in reading the input file
#define DOT_LINE_BUFFER_LENGTH 32

// The longest graph name we would expect plus the maximum flow report length
#define OUTPUT_NAME_BUFFER_LENGTH 32


// An Adjacency Matrix
typedef struct adjmatrix {

    size_t vertices;            // The number of vertices in the graph
    unsigned int *matrix;       // matrix[(u * vertices) + v] is the capacity of edge u->v (0 == no edge)

} adjmatrix_t;


// Simple Linked-List queue node
typedef struct qnode{

    unsigned int value;
    struct qnode *next;    

} qnode_t;

// Associated queue helpers
void push(qnode_t** queue, unsigned int val){
    qnode_t* newNode = (qnode_t *)malloc(sizeof(qnode_t));
    newNode->value = val;
    newNode->next = *queue;
    *queue = newNode;
}

unsigned int pop(qnode_t** queue){
        unsigned int temp = (*queue)->value;
        qnode_t *tempNode = *queue;
        *queue = tempNode->next;
        free(tempNode);
        return temp;
}

bool isEmpty(qnode_t* queue){
    return (queue == NULL);
}



// Breadth-First Search
// Parameters: g, a graph to search for paths. We assume that vertex 0 is the source and vertex |V| - 1 is the sink (for simplicity)
//             p, a "parent list"; an array of size |V| such that p[j] == i <==> (i, j) is and edge in the path
// Returns true iff a path exists from source to sink.
// Modifies p to reflect the path.
// O(|E|) operation
bool breadthFirstSearch(adjmatrix_t *g, unsigned int* p){
    // Define a "visited" flag set
    bool visited[g->vertices];
    memset(visited, 0, g->vertices * sizeof(bool));

    // Clear parenthood list
    memset(p, 0, g->vertices * sizeof(unsigned int));
    
    // Prepare a queue
    qnode_t *queue = NULL;

    // Visit and push source node
    push(&queue, 0);
    visited[0] = true;

    // Run BFS traversal
    while(!isEmpty(queue)){
        unsigned int vertex = pop(&queue);
        // Mark as visited, enqueue, and reparent to "vertex" all unvisited adjacent vertices to "vertex"
        for(unsigned int i = 0; i < g->vertices; i++){
            if(g->matrix[(vertex * g->vertices) + i] != 0 && !visited[i]){
                visited[i] = true;
                push(&queue, i);
                p[i] = vertex;
            }
        }
    }   

    // Return whether we touched the sink
    return visited[g->vertices - 1];
}



// Edmonds-Karp helper function: finds cf(p), the maximum flow through a path (i.e. the capacity of its weakest edge)
// Parameters: g, p; these are identical to the parameters for BFS above and p should have been produced by an invocation of BFS.
// O(|p|) operation
unsigned int pathCapacity(adjmatrix_t *g, unsigned int* p){
    unsigned int pathCap;
    memset(&pathCap, 0xFF, sizeof(unsigned int));    // A cheap and perhaps incorrect approximation of infinite capacity

    unsigned int vertex = g->vertices - 1;  // Start at the sink
    while(vertex != 0){     // Repeat until we hit the source
        unsigned int edgeCost = g->matrix[(p[vertex] * g->vertices) + vertex];        
        pathCap = (edgeCost < pathCap) ? edgeCost : pathCap;                    // Store the minimum capacity along the path
        vertex = p[vertex];
    }
    return pathCap;
}



// Edmonds-Karp Algorithm
// Parameters: g, the flow network to be operated upon; this will be overwritten with the residual graph.
//             f, a space to store the resultant flow graph
// Returns the maximal flow capacity.
// O(|V||E|^2) algorithm
unsigned int edmondsKarp(adjmatrix_t *g, adjmatrix_t *f){
    // Initialize various things
    unsigned int maxFlow = 0;
    unsigned int p[g->vertices];

    while(breadthFirstSearch(g, p)){
        // While we can find an augmenting path

        // Get the cost of said path
        unsigned int pathCap = pathCapacity(g, p);

        // Increase our maximum flow by this capacity
        maxFlow += pathCap;

        // Residualize the graph / augment the flow
        unsigned int vertex = g->vertices - 1;  // Start at the sink
        while(vertex != 0){     // Repeat until we hit the source
            
            // Recompute the residual
            g->matrix[(p[vertex] * g->vertices) + vertex] -= pathCap;     // parent->self -= path flow
            g->matrix[(vertex * g->vertices) + p[vertex]] += pathCap;     // self->parent += path flow

            // Augment the flow
            f->matrix[(p[vertex] * f->vertices) + vertex] += pathCap;

            vertex = p[vertex]; // Move back along the path
        }

    }
     
    return maxFlow;
}





// Main Function
// Program takes two arguments (the file paths of input and output) and computes
// the maximum flow for the given network, storing it as another graph
// (as specified in the assignment description)
// Precondition: Input file is well-formed (if in doubt, use only the output of flow_network_generator.c)
int main(int argc, char **argv){

    // Verify argument
    if (argc != 3){
        fprintf(stderr, "Usage: flow_network_generator <input path> <output path>\n");
        exit(1);
    }

    // Allocate new adjacency matrix describing our Flow Network
    adjmatrix_t flownet;
    flownet.vertices = 5; // Given by spec, we know there are at least 5 vertices
    flownet.matrix = (unsigned int *)calloc(flownet.vertices * flownet.vertices, sizeof(unsigned int));
    
    
    // Parse input into adjacency matrix
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open %s for reading\n", argv[1]); 
        exit(2);
    }

    char buffer[DOT_LINE_BUFFER_LENGTH];
    char outputName[OUTPUT_NAME_BUFFER_LENGTH];

    // Discard the first four lines
    fgets(buffer, DOT_LINE_BUFFER_LENGTH - 1, fp);
    fgets(buffer, DOT_LINE_BUFFER_LENGTH - 1, fp);
    fgets(buffer, DOT_LINE_BUFFER_LENGTH - 1, fp);
    fgets(buffer, DOT_LINE_BUFFER_LENGTH - 1, fp);
    fgets(buffer, DOT_LINE_BUFFER_LENGTH - 1, fp);

    // Read in edges, line by line
    while(strlen(buffer) > 1){
        unsigned int edgeSource, edgeDest, edgeCap;
        sscanf(buffer, "%u -> %u [label = \" %u \"];\n", &edgeSource, &edgeDest, &edgeCap);

        // Check to see if we need to resize the adjacency matrix
        if(edgeSource > flownet.vertices || edgeDest > flownet.vertices){
            // realloc() won't cut it for this problem...
            // Allocate a region of the appropriate maximum size
            unsigned int newVertices = (edgeSource > edgeDest) ? edgeSource : ((edgeDest > flownet.vertices) ? edgeDest : edgeSource);
            unsigned int *newMatrix = (unsigned int *)calloc(newVertices * newVertices, sizeof(unsigned int));
            for(int i = 0; i < flownet.vertices; i++){
                memcpy(&(newMatrix[i * newVertices]), &(flownet.matrix[i * flownet.vertices]), flownet.vertices * sizeof(unsigned int));
            }
            flownet.vertices = newVertices;
            unsigned int *temp = flownet.matrix;
            flownet.matrix = newMatrix;
            free(temp);
        }

        // Add the edge to the Matrix
        flownet.matrix[((edgeSource - 1) * flownet.vertices) + (edgeDest - 1)] = edgeCap;

        // Fetch the next line
        fgets(buffer, DOT_LINE_BUFFER_LENGTH - 1, fp);
    }

    // Read the graph name
    fgets(buffer, DOT_LINE_BUFFER_LENGTH - 1, fp);
    sscanf(buffer, "label = \"%s", outputName);
    // For some reason scanf catches the closing quotation mark, thus:
    outputName[strlen(outputName) - 1] = '\0';

    // Close the Input File
    fclose(fp);
 


    // Duplicate the graph
    adjmatrix_t residual;
    residual.vertices = flownet.vertices;
    residual.matrix = (unsigned int *)calloc(residual.vertices * residual.vertices, sizeof(unsigned int));
    memcpy(residual.matrix, flownet.matrix, residual.vertices * residual.vertices * sizeof(unsigned int));

    // Allocate space for an output flow
    adjmatrix_t flow;
    flow.vertices = flownet.vertices;
    flow.matrix = (unsigned int *)calloc(flow.vertices * flow.vertices, sizeof(unsigned int));


    // Run Edmonds-Karp Algorithm
    unsigned int maxFlow = edmondsKarp(&residual, &flow);
    // "flow" now contains the flow graph.
 
    

    // Convert Flow Network edges to .dot list format string
    // Same technique as in flow_network_generator.c
    char outBuffer[flow.vertices * flow.vertices * DOT_LINE_BUFFER_LENGTH];
    char *bufHead = outBuffer;
    for(int i = 0; i < flow.vertices; i++){
        for(int j = 0; j < flow.vertices; j++){
            // For every edge (i, j)
            unsigned int edgeCapacity = flow.matrix[(i * flow.vertices) + j];
            unsigned int maxCapacity = flownet.matrix[(i * flownet.vertices) + j];
            if (maxCapacity != 0 && edgeCapacity != 0){
                // An edge exists; append it to our output string
                bufHead += snprintf(bufHead, DOT_LINE_BUFFER_LENGTH, "%d -> %d [label = \" %d/%d \"];\n", i+1, j+1, edgeCapacity, maxCapacity);
            }
        }
    }

    // Append the total flow size to the label
    snprintf(outputName + strlen(outputName), OUTPUT_NAME_BUFFER_LENGTH - strlen(outputName), ": maximum flow = %u ", maxFlow); 
    

    // Open the Output File for writing
    fp = fopen(argv[2], "w");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open %s for writing\n", argv[2]); 
        exit(2);
    }


    // Write .dot skeleton fleshed with our graph to the file
    fprintf(fp, "digraph g{\n\nrankdir = LR\n\n%s\nlabel = \"%s\"\n}\n", outBuffer, outputName);


    // Close the Output File
    fclose(fp);


    // Clean up
    free(flownet.matrix);
    free(residual.matrix);
    free(flow.matrix);

}
