all : psort

psort: psort.c helper.c helper.h
	gcc -Wall -std=gnu99 -g -o psort helper.c psort.c

clean :
	rm psort
