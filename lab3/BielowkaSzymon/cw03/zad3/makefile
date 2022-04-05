make:
	gcc -Wall -c zad3.c
	gcc zad3.o -lm -o zad3

clean:
	rm -f *.so *.o *.a

test:
	make
	./zad3 testdir asd 3
	./zad3 testdir asd 4
	./zad3 testdir dsa 5
	make clean

