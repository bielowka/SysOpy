withfork:
	gcc -Wall -c main_fork.c
	gcc main_fork.o -o main_fork

withexec:
	gcc -Wall -c exec.c
	gcc exec.o -o exec
	gcc -Wall -c main_exec.c
	gcc main_exec.o -o main_exec

clean:
	rm -f *.so *.o *.a

test:
	make withfork
	./main_fork ignore
	./main_fork handler
	./main_fork mask
	./main_fork pending
	make withexec
	./main_exec ignore
	./main_exec handler
	./main_exec mask
	./main_exec pending
	make clean
