make:
	gcc -Wall -c zad2.c
	gcc zad2.o -lm -o zad2

clean:
	rm -f *.so *.o *.a
	
test:
	rm -f *.txt
	make
	./zad2 0.00003 1
	./zad2 0.00003 16
	./zad2 0.00000003 1
	./zad2 0.00000003 6
	./zad2 0.00000003 12
	./zad2 0.00000003 16
	./zad2 0.00000003 64
	./zad2 0.00000003 96
	./zad2 0.000000001 1
	./zad2 0.000000001 6
	./zad2 0.000000001 12
	./zad2 0.000000001 16
	./zad2 0.000000001 64
	./zad2 0.000000001 96
	make clean
