all: battleship.o server.o client.o
	gcc -o server server.o battleship.o
	gcc -o client client.o battleship.o

battleship.o: battleship.c battleship.h
	gcc -c battleship.c

server.o: server.c battleship.c
	gcc -c server.c

client.o: client.c battleship.c
	gcc -c client.c

clean:
	rm *.o
