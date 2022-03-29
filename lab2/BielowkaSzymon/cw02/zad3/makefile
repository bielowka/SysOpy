make:
	gcc -Wall -c mainvar1.c
	gcc mainvar1.o -o mainvar1
	gcc -Wall -c mainvar2.c
	gcc mainvar2.o -o mainvar2

clean:
	rm -f *.so *.o *.a

test:
	make
	./mainvar1 testdir
	./mainvar2 testdir
	make clean

