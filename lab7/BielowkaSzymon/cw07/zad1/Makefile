make:
	gcc -Wall -c main.c
	gcc main.o -o main
clean:
	rm -f main
	rm -f *.so *.o *.a

test:
	make
	./main 3 3
	make clean
