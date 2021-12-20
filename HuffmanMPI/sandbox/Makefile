all: test1 test2 test3

test1:
	./../bin/huffman -c test1.txt test1_c.txt
	./../bin/huffman -d test1_c.txt test1_d.txt
	diff test1.txt test1_d.txt

test2:
	./../bin/huffman -c test2.txt test2_c.txt
	./../bin/huffman -d test2_c.txt test2_d.txt
	diff test2.txt test2_d.txt

test3:
	./../bin/huffman -c test3.txt test3_c.txt
	./../bin/huffman -d test3_c.txt test3_d.txt
	diff test3.txt test3_d.txt

clean:
	rm -f test1/*_c.txt test1/*_d.txt	
	rm -f test2/*_c.txt test2/*_d.txt
	rm -f test3/*_c.txt test3/*_d.txt
