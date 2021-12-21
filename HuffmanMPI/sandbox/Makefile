SHELL := /bin/bash

all: test1 test2 test3

# Temporary fix the "--mca opal_warn_on_missing_libcuda 0"
RUN = time mpirun -n 4 --mca opal_warn_on_missing_libcuda 0 ./../bin/huffman

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

tree:
	$(RUN) -t test3.txt

clean:
	rm -f test*_c.txt test*_d.txt
