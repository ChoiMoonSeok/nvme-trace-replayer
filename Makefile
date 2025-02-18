
CC=gcc

trace-replay : file-reader.o  trace_replayer.c
	gcc -o trace-replay file-reader.o  trace_replayer.c -lnvme -lm

file-reader.o : file_reader.c
	gcc -c -o file-reader.o file_reader.c

clean:
	rm *.o trace-replay