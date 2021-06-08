make :
	make clean && make client && make main

main: main.c server.c
	gcc -o main main.c server.c list.o -lpthread 

client: client.c
	gcc -o client client.c

clean:
	rm -f client
	rm -f main