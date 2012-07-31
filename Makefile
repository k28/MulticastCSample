all:
	make mcastSend
	make mcastReceve

mcastSend:	mcastSend.c
	gcc -o sender mcastSend.c

mcastReceve:	mcastReceve.c
	gcc -o recever mcastReceve.c

clean:
	rm -f sender
	rm -f recever

