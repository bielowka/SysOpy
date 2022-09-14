make:
	gcc -Wall main.c -o main -lpthread -lm

clean:
	rm -f *.so *.o *.a
	rm main

test:
	make
	./main
	make clean
