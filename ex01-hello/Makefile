LANGUAGE=FRENCH
all: hello-world fancy-hello-world international-hello-world

hello-world: hello-world.o
	cc -g -o hello-world hello-world.o

hello-world.o: hello-world.c
	cc -c -Wall -g hello-world.c -D$(LANGUAGE)

fancy-hello-world: fancy-hello-world.o
	cc -g -o fancy-hello-world fancy-hello-world.o
fancy-hello-world.o: fancy-hello-world.c
	cc -c -Wall -g fancy-hello-world.c -D$(LANGUAGE)

international-hello-world: international-hello-world.o
	cc -g -o international-hello-world fancy-hello-world.o
international-hello-world.o: fancy-hello-world.c
	cc -c -Wall -g international-hello-world.c -D$(LANGUAGE)

clean:
	rm hello-world.o hello-world 
	rm fancy-hello-world.o fancy-hello-world
