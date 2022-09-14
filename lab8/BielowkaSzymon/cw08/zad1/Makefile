make:
	gcc -Wall main.c -o main -lpthread -lm

clean:
	rm -f *.so *.o *.a
	rm main

test:
	make
	./main 1 numbers dragon.ascii.pgm dragonreversednumbers.ascii.pgm
	./main 2 numbers dragon.ascii.pgm dragonreversednumbers.ascii.pgm
	./main 4 numbers dragon.ascii.pgm dragonreversednumbers.ascii.pgm
	./main 8 numbers dragon.ascii.pgm dragonreversednumbers.ascii.pgm
	./main 16 numbers dragon.ascii.pgm dragonreversednumbers.ascii.pgm
	./main 32 numbers dragon.ascii.pgm dragonreversednumbers.ascii.pgm
	./main 1 blocks dragon.ascii.pgm dragonreversedblocks.ascii.pgm
	./main 2 blocks dragon.ascii.pgm dragonreversedblocks.ascii.pgm
	./main 4 blocks dragon.ascii.pgm dragonreversedblocks.ascii.pgm
	./main 8 blocks dragon.ascii.pgm dragonreversedblocks.ascii.pgm
	./main 16 blocks dragon.ascii.pgm dragonreversedblocks.ascii.pgm
	./main 32 blocks dragon.ascii.pgm dragonreversedblocks.ascii.pgm
	make clean
