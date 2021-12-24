SHELL := /bin/bash

all: test1 test2 test3 test4

test1:
	time ./../bin/huffman -c test1.txt test1_c.txt
	time ./../bin/huffman -d test1_c.txt test1_d.txt
	diff test1.txt test1_d.txt

test2:
	time ./../bin/huffman -c test2.txt test2_c.txt
	time ./../bin/huffman -d test2_c.txt test2_d.txt
	diff test2.txt test2_d.txt

test3:
	time ./../bin/huffman -c test3.txt test3_c.txt
	time ./../bin/huffman -d test3_c.txt test3_d.txt
	diff test3.txt test3_d.txt

test4:
	time ./../bin/huffman -c test4.txt test4_c.txt
	time ./../bin/huffman -d test4_c.txt test4_d.txt
	diff test4.txt test4_d.txt


clean:
	rm -f test*_c.txt test*_d.txt	
