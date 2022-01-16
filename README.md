# Advanced Parralel System

## How to use
Usage : ./huffman option [input_filename] [output_filename]

Option :
-c/--compression = Compress [input_filename] and outputs it as [output_filename] if furnished, if not as result_decompression.txt

-d/--decompression = Decompress [input_filename] and outputs it as [output_filename] if furnished, if not as result_decompression.txt

-t/--test = Will compress and decompress the file named [input_filename], and output a file representing the tree used for the compression

## Sandbox
The sandbox folder is used to test and to do some performance measurement.
Using the Makefile default rule, it will try to compress and decompress 5 files named "test1.txt, test2.txt, test3.txt, test4.txt, test5.txt".

The compressed file will be named "testx_c.txt" and the decompressed file "testx_d.txt" with x equal to the number of the original file.

You can also the rule "make gtest" to generate 5 random test files (test1.txt, test2.txt, etc...) 

## Few things to know
- Incomplete perf measurement could be upgrade
- The MPI version has some issue with very big file
- Metadata could be easily reduced but I choosen to let it this way in order to make some easier for a human to read
- Generator of random file can be upgraded