.PHONY: clean all

all : usrbuzzlock tester

clean :
	-rm libbuzzlock.so usrbuzzlock.o kernbuzzlock.o tester test

kernbuzzlock : kernbuzzlock.c
	gcc -Wall -fPIC -g -DBZZ_KERNEL_MODE -c kernbuzzlock.c
	gcc -shared -g -W1,-soname,libbuzzlock.so -o libbuzzlock.so kernbuzzlock.o

usrbuzzlock : usrbuzzlock.c
	gcc -Wall -fPIC -g -c usrbuzzlock.c
	gcc -shared -g -W1,-soname,libbuzzlock.so -o libbuzzlock.so usrbuzzlock.o

tester : tester.c usrbuzzlock
	gcc -Wall -L$(shell pwd) tester.c -lbuzzlock -o tester

usrbuzzlock kernbuzzlock tester test test2 : buzzlock.h

runtester : tester
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./tester

test : test.c usrbuzzlock
	gcc -Wall -L$(shell pwd) test.c -g -fopenmp -lbuzzlock -o test

test2 : test.c kernbuzzlock
	gcc -Wall -L$(shell pwd) test.c -g -fopenmp -lbuzzlock -o test

runtest : test
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./test

runtest2 : test2
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./test
