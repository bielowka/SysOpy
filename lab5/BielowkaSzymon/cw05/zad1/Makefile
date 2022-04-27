make:
	gcc -Wall -c main.c
	gcc main.o -o main

clean:
	rm -f *.so *.o *.a

test:
	make
	./main polecenia.txt
	make clean

