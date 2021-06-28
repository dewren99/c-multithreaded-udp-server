make :
	make clean && make s-talk

s-talk: main.c server.c client.c input_reciever.c message_printer.c
	gcc -g -o s-talk main.c server.c client.c input_reciever.c message_printer.c list.o -lpthread 

clean:
	rm -f s-talk