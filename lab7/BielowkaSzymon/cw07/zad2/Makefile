make:
	gcc -Wall -g3 -c main.c
	gcc main.o -o main -lrt -lpthread
clean:
	rm -f main
	rm -f *.so *.o *.a

test:
	make
	./main 3 3
	make clean
