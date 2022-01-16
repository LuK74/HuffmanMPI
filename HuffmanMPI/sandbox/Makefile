SHELL := /bin/bash

all: test1 test2 test3 test4 test5

gtest:
	python3 file_generator.py 10 > test1.txt
	python3 file_generator.py 100 > test2.txt
	python3 file_generator.py 1000 > test3.txt
	python3 file_generator.py 10000 > test4.txt
	python3 file_generator.py 100000 > test5.txt

# Temporary fix the "--mca opal_warn_on_missing_libcuda 0"
RUN = time mpirun -n 4 --mca opal_warn_on_missing_libcuda 0 ./../bin/huffman

DEBUG = mpirun -n 2 --mca opal_warn_on_missing_libcuda 0 xterm -e gdb --args ./../bin/huffman

VALGRIND =  mpirun -n 2 --mca opal_warn_on_missing_libcuda 0 xterm -e valgrind ./../bin/huffman

valgrind_c:
	$(VALGRIND) -c test1.txt test1_c.txt

debug_c:
	$(DEBUG) -c test5.txt test5_c.txt

debug_d:
	$(DEBUG) -d test1_c.txt test1_d.txt

test%:
	$(RUN) -c $@.txt $@_c.txt
	$(RUN) -d $@_c.txt $@_d.txt
	diff $@.txt $@_d.txt

tree:
	$(RUN) -t test3.txt

clean:
	rm -f test*_c.txt test*_d.txt
	rm -f rtest*_c.txt rtest*_d.txt
