all: client server


common.o: common.c
	cc -Wall -g -c common.c

client: client.o common.o
	cc -g -o client client.o common.o

client.o: client.c
	cc -Wall -g -c client.c

server: server.o common.o
	cc -g -o server server.o common.o
server.o: server.c
	cc -Wall -g -c server.c

clean:
	rm client.o client 
	rm server.o server
	rm common.o common