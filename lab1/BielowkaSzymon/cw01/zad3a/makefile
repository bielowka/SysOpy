static:
	gcc -Wall -c labonelib.c
	ar rcs labonelib.a labonelib.o
	gcc -Wall -c main.c
	gcc main.o labonelib.a -o main

shared:
	gcc -Wall -c -fPIC labonelib.c
	gcc -shared labonelib.o -o liblabonelib.so
	gcc -Wall main.c -L. -Wl,-rpath=. -llabonelib -o main

dynamic:
	gcc -Wall -c -fPIC labonelib.c
	gcc -shared labonelib.o -o liblabonelib.so
	gcc -Wall main.c -ldl -D dynamic -o main

clean:
	rm -f *.so *.o *.a

test:
	printf "With static: \n" >> raport2.txt
	make static
	./main $(shell cat polecenia3.txt)
	make clean
	printf "With shared: \n" >> raport2.txt
	make shared
	./main $(shell cat polecenia3.txt)
	make clean
	printf "With dynamic: \n" >> raport2.txt
	make dynamic
	./main $(shell cat polecenia3.txt)
	make clean
	