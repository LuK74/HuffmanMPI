SHELL := /bin/bash

all: test1 test2 test3 test4 test5

random_test:
	python3 file_generator.py 10 > rtest1.txt
	python3 file_generator.py 100 > rtest2.txt
	python3 file_generator.py 1000 > rtest3.txt
	python3 file_generator.py 10000 > rtest4.txt
	python3 file_generator.py 100000 > rtest5.txt
	python3 file_generator.py 1000000 > rtest6.txt
	python3 file_generator.py 10000000 > rtest7.txt
	python3 file_generator.py 100000000 > rtest8.txt
	python3 file_generator.py 200000000 > rtest9.txt
	python3 file_generator.py 400000000 > rtest10.txt

rtest: rtest1 rtest2 rtest3 rtest4 rtest5 rtest6 rtest7 rtest8 rtest9 rtest10

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

rtest%:
	$(RUN) -c $@.txt $@_c.txt
	$(RUN) -d $@_c.txt $@_d.txt
	diff $@.txt $@_d.txt

test1:
	$(RUN) -c test1.txt test1_c.txt
	$(RUN) -d test1_c.txt test1_d.txt
	diff test1.txt test1_d.txt

test2:
	$(RUN) -c test2.txt test2_c.txt
	$(RUN) -d test2_c.txt test2_d.txt
	diff test2.txt test2_d.txt

test3:
	$(RUN) -c test3.txt test3_c.txt
	$(RUN) -d test3_c.txt test3_d.txt
	diff test3.txt test3_d.txt

test4:
	$(RUN) -c test4.txt test4_c.txt
	$(RUN) -d test4_c.txt test4_d.txt
	diff test4.txt test4_d.txt

test5:
	$(RUN) -c test5.txt test5_c.txt

tree:
	$(RUN) -t test3.txt

clean:
	rm -f test*_c.txt test*_d.txt
	rm -f rtest*_c.txt rtest*_d.txt
