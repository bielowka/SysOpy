make:
	gcc -Wall -c main.c
	gcc main.o -o main
	gcc -Wall -c producer.c
	gcc producer.o -o producer
	gcc -Wall -c consumer.c
	gcc consumer.o -o consumer

clean:
	rm -f *.so *.o *.a

test:
	make
	./main
	make clean

