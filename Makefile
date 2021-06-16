make :
	make clean && make main

main: main.c server.c client.c input_reciever.c message_printer.c
	gcc -o main main.c server.c client.c input_reciever.c message_printer.c list.o -lpthread 

clean:
	rm -f main