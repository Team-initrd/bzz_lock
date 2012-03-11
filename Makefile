.PHONY: clean all

all: usrbuzzlock.o tester

clean:
	-rm libbuzzlock.so usrbuzzlock.o tester

usrbuzzlock.o : usrbuzzlock.c
	gcc -Wall -fPIC -c usrbuzzlock.c
	gcc -shared -W1,-soname,libbuzzlock.so -o libbuzzlock.so usrbuzzlock.o
	export LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH

tester : tester.c
	gcc -Wall -L$(shell pwd) tester.c -lbuzzlock -o tester

usrbuzzlock.o tester : buzzlock.h
