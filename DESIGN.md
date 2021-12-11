# Huffman Design

## Execution flow / Encoding
- Parsing text
- Count occurences for each character
- Build tree using the frequency for each character
- Go through tree to determine encoding
- Write the new file

## Execution flow / Decoding
- Read the frequency
- Build the tree
- Follow the path in the tree using the 0s and 1s

## Structure needed
- Standard Tree with node empty except the leaves they got a value an a char
- Simple table with a key, and two values, frequency and occurences