make:
	gcc -Wall -c mainlib.c
	gcc mainlib.o -o mainlib
	gcc -Wall -c mainsys.c
	gcc mainsys.o -o mainsys

clean:
	rm -f *.so *.o *.a

test:
	make
	echo "Anwer should be: 4000 3000 \n"
	./mainlib a sample.txt
	./mainsys a sample.txt
	make clean
