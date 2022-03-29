static:
	gcc -Wall -c labonelib.c
	ar rcs labonelib.a labonelib.o
	gcc -Wall -c main.c
	gcc labonelib.o main.o -o main

shared:
	gcc -Wall -c -fPIC labonelib.c
	gcc -shared -fPIC -o labonelib.so labonelib.o

clean:
	rm -f *.so *.o *.a
