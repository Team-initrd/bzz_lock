.PHONY: clean all

all : usrbuzzlock tester

clean :
	-rm libbuzzlock.so usrbuzzlock.o kernbuzzlock.o tester

kernbuzzlock : kernbuzzlock.c
	gcc -Wall -fPIC -c kernbuzzlock.c
	gcc -shared -W1,-soname,libbuzzlock.so -o libbuzzlock.so kernbuzzlock.o

usrbuzzlock : usrbuzzlock.c
	gcc -Wall -fPIC -c usrbuzzlock.c
	gcc -shared -W1,-soname,libbuzzlock.so -o libbuzzlock.so usrbuzzlock.o

tester : tester.c
	gcc -Wall -L$(shell pwd) tester.c -lbuzzlock -o tester

usrbuzzlock kernbuzzlock tester : buzzlock.h
