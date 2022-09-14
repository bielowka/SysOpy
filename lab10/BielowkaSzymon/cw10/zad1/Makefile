make:
	gcc -Wall server.c -o server -lpthread -lm
	gcc -Wall client.c -o client -lpthread -lm

clean:
	rm -f *.so *.o *.a
	rm server
	rm client

serv:
	make
	./server 7001 socket
	make clean

client1:
	make
	./client Alfred unix socket
	make clean

client2:
	make
	./client Bernard unix socket
	make clean

client1inet:
	make
	./client Alfred inet 7001
	make clean

client2inet:
	make
	./client Bernard inet 7001
	make clean