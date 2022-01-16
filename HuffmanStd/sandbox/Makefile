SHELL := /bin/bash

all: test1 test2 test3 test4 test5


gtest:
	python3 file_generator.py 10 > test1.txt
	python3 file_generator.py 100 > test2.txt
	python3 file_generator.py 1000 > test3.txt
	python3 file_generator.py 10000 > test4.txt
	python3 file_generator.py 100000 > test5.txt

test%:
	time ./../bin/huffman -c $@.txt $@_c.txt
	time ./../bin/huffman -d $@_c.txt $@_d.txt
	diff $@.txt $@_d.txt
clean:
	rm -f test*_c.txt test*_d.txt	
