
CC=gcc

trace-replay : file-reader.o HashTable.o trace_replayer.c
	gcc -o trace-replay file-reader.o HashTable.o trace_replayer.c -lnvme -lm

file-reader.o : file_reader.c
	gcc -c -o file-reader.o file_reader.c

HashTable.o : HashTable.c
	gcc -c -o HashTable.o HashTable.c

clean:
	rm *.o trace-replay