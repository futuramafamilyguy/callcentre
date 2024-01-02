server: server.o ccp.o
	gcc -o server server.o ccp.o

client: client.o ccp.o
	gcc -o client client.o ccp.o

server.o: server.c
	gcc -c -g server.c

client.o: client.c
	gcc -c -g client.c

ccp.o: ccp/ccp.c ccp/ccp.h
	gcc -c -g ccp/ccp.c

clean: 
	rm *.o *.exe a.out
	