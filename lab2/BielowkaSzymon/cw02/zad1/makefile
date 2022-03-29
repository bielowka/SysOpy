make:
	gcc -Wall -c mainlib.c
	gcc mainlib.o -o mainlib
	gcc -Wall -c mainsys.c
	gcc mainsys.o -o mainsys
clean:
	rm -f *.so *.o *.a

test:
	make
	./mainlib sample1.txt sample2.txt
	./mainsys sample1.txt sample2.txt
	make clean	
