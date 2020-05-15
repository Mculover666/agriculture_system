CC = gcc
INCPATH = -I/usr/include/mysql
LIB = -lmysqlclient -L/usr/lib/mysql

client: client.o
	$(CC) client.o $(LIB) -o client

client.o:client.c
	$(CC) -c client.c $(INCPATH) -o client.o

clean:
	rm -rf *.o client
