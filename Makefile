.PHONY: clean all

all: usrbuzzlock.o tester

clean:
	-rm libbuzzlock.so usrbuzzlock.o tester test

usrbuzzlock.o : usrbuzzlock.c
	gcc -Wall -fPIC -g -c usrbuzzlock.c
	gcc -shared -g -W1,-soname,libbuzzlock.so -o libbuzzlock.so usrbuzzlock.o

tester : tester.c
	gcc -Wall -L$(shell pwd) tester.c -lbuzzlock -o tester

runtester : tester
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./tester

test : test.c usrbuzzlock.o
	gcc -Wall -L$(shell pwd) test.c -g -fopenmp -lbuzzlock -o test

runtest : test
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./test

usrbuzzlock.o tester test : buzzlock.h
