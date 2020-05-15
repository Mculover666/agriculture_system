CC = gcc
INCPATH = -I/usr/include/mysql
LIB = -lmysqlclient -L/usr/lib/mysql

server: server.o cJSON.o
	$(CC) server.o cJSON.o $(LIB) -o server

server.o:server.c
	$(CC) -c server.c $(INCPATH) -o server.o

cJSON.o:cJSON.c
	$(CC) -c cJSON.c -o cJSON.o

clean:
	rm -rf *.o server
