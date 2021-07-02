make :
	make clean && make s-talk

s-talk: main.c server.c client.c input_reciever.c message_printer.c terminate.c
	gcc -o s-talk main.c server.c client.c input_reciever.c message_printer.c terminate.c list.o -lpthread 

clean:
	rm -f s-talk