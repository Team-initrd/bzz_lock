.PHONY: clean

clean :
	-rm libbuzzlock.so usrbuzzlock.o kernbuzzlock.o tester test

kern : kernbuzzlock.c
	gcc -Wall -fPIC -g -DBZZ_KERNEL_MODE -c kernbuzzlock.c
	gcc -shared -g -W1,-soname,libbuzzlock.so -o libbuzzlock.so kernbuzzlock.o

usr : usrbuzzlock.c
	gcc -Wall -fPIC -g -c usrbuzzlock.c
	gcc -shared -g -W1,-soname,libbuzzlock.so -o libbuzzlock.so usrbuzzlock.o

usrdebug : usrbuzzlock.c
	gcc -Wall -fPIC -g -c usrbuzzlock.c
	gcc -shared -g -W1,-soname,libbuzzlock.so -o libbuzzlock.so usrbuzzlock.o

tester : tester.c kern
	gcc -Wall -L$(shell pwd) -DBZZ_KERNEL_MODE tester.c -lbuzzlock -o tester

usr usrdebug kern tester test test2 : buzzlock.h

runtester : tester
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./tester

test : test.c usr
	gcc -Wall -L$(shell pwd) test.c -g -fopenmp -lbuzzlock -o test

test2 : test.c kern
	gcc -Wall -L$(shell pwd) -DBZZ_KERNEL_MODE test.c -g -fopenmp -lbuzzlock -o test

runtest : test
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./test 100 30 10000

runtest2 : test2
	LD_LIBRARY_PATH=$(shell pwd):$$LD_LIBRARY_PATH ./test 100 30 10000
