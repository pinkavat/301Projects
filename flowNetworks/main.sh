#!/bin/bash

# Runs flow_network_generator and max_flow_generator to generate ten
# random flow networks and their maximal flows. The script also converts
# the resulting .dot files into png files with the aid of the dot utility.


counter=1
while [ $counter -le 10 ]
do

# Generate a new flow network
./flow_network_generator $counter input_graphs/graph_$counter.dot $RANDOM

# Compute the maximal flow for this network
echo Computing Maximal Flow for graph_$counter.dot
./max_flow_generator input_graphs/graph_$counter.dot output_graphs/graph_$counter.dot

# Convert flow and maxflow into .png files
dot -Tpng input_graphs/graph_$counter.dot -o input_graphs/graph_$counter.png
dot -Tpng output_graphs/graph_$counter.dot -o output_graphs/graph_$counter.png

((counter++))
done
