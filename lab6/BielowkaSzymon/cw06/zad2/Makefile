make: client server

client: client.c
	gcc -Wall -o client client.c -lrt

server: server.c
	gcc -Wall -o server server.c -lrt

clean:
	rm -f client server
	rm -f *.so *.o *.a
