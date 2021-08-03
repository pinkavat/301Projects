import java.io.*;
import java.util.*;

public class Huffman {
    
    // A node in a Huffman Tree
    private static class HuffNode implements Comparable{
        public int frequency;
        public HuffNode L, R;

        public HuffNode(int frequency){
            this.frequency = frequency;
            this.L = null;
            this.R = null;
        }

        public int compareTo(Object o){
            if (o instanceof HuffNode){
                return this.frequency - ((HuffNode)o).frequency;
            } else {
                return -1;
            }
        }
        
    }

    // An extension of a Huffman node, a leaf with character data
    private static class HuffLeaf extends HuffNode{
        public char letter;
    
        public HuffLeaf(char l, int freq){
            super(freq);
            this.letter = l;
        }
    }
    
    // Join two tree nodes
    private static HuffNode join(HuffNode a, HuffNode b){
        HuffNode newNode = new HuffNode(a.frequency + b.frequency);
        newNode.L = a;
        newNode.R = b;
        return newNode;
    }

   
    // Generate a pseudo-Huffman table for the provided string
    private static HuffNode buildTree(String str){
        
        // Step 1: Count frequencies
        Map<Character, Integer> freqs = new HashMap<Character, Integer>();
         
        for(char c : str.toCharArray()){
            Integer oldval = freqs.get(new Character(c));
            freqs.put(new Character(c), (oldval == null)?new Integer(1):new Integer(oldval.intValue() + 1)); 
        }

        // Now begin repeatedly prio-queueing our Huffman Tree
        PriorityQueue<HuffNode> toFold = new PriorityQueue<HuffNode>();

        // Initial population
        for(Map.Entry<Character, Integer> e: freqs.entrySet()){
            toFold.add(new HuffLeaf(e.getKey().charValue(),e.getValue().intValue()));
        }

        // Repeated folding
        while(toFold.size() > 1){
            // Pop the first two values
            HuffNode first = toFold.poll();
            HuffNode second = toFold.poll();
            toFold.offer(join(first, second));
        }
        return toFold.poll();
    }



    // Helper for below function, performs tree walks
    private static void treeWalkRecurser(HuffNode cur, String code, Map<Character, String> table){
        if (cur instanceof HuffLeaf){
            // We've reached a leaf; append to set 
            table.put(new Character(((HuffLeaf)cur).letter), code);
        } else {
            // Recurse
            treeWalkRecurser(cur.L, code + "1", table);
            treeWalkRecurser(cur.R, code + "0", table);
        }
    }

    
    // From a computed tree, extract a table
    private static Map<Character, String> buildTable(HuffNode tree){
        // Prepare our output table
        Map<Character, String> table = new HashMap<Character, String>();

        // Detect single-char edge case:
        if (tree.L == null && tree.R == null){
            table.put(new Character(((HuffLeaf)tree).letter), "0");
            return table;
        }
        
        // Perform recursive tree walks from the root, adding to our Map as we go
        // Codes are made of zero and one chars for convenience (would this be called Huffman Decompression??)
        treeWalkRecurser(tree, "", table);

        // Return the result
        return table;

    }



    // Huffman "encode" a given string
    public static String encode(String str, Map<Character, String> table){
        String output = "";
        for(char c: str.toCharArray()){
            output = output + table.get(new Character(c));
        }
        return output;
    }

    // Huffman "decode" a given string
    // Easier to do it from tree [O(log n) versus O(nk) operation]
    public static String decode(String str, HuffNode tree){
        // Basically do Binary Search over and over and over
        String output = "";
        for(int i = 0; i < str.length();){
            HuffNode cur = tree;
            if(tree instanceof HuffLeaf){
                i++;
                output = output + ((HuffLeaf)cur).letter;
            } else {
                while(!( cur instanceof HuffLeaf)){
                    if (str.charAt(i) == '1'){
                        cur = cur.L;
                    } else {
                        cur = cur.R;
                    }
                    i++;
                }
                if(cur != null) output = output + ((HuffLeaf)cur).letter;
            }
        }
        return output;
    }






    // Test driver
    public static void main(String[] args){
        try {
            System.out.println("Huffman codes for "+args[0]+":");
            HuffNode tree = buildTree(args[0]);
            Map<Character, String> table = buildTable(tree);
            for(Map.Entry<Character, String> e : table.entrySet()){
                System.out.println(e.getKey()+" : "+e.getValue());
            }
            String encoded = encode(args[0],table);
            System.out.println("\nHuffman encoding of "+args[0]+":\n"+encoded);
            System.out.println("\n"+encoded+" decoded: "+decode(encoded, tree));
        } catch (Exception e){
            System.out.println("Provide a test input string as the first argument");
        }
    }
}
