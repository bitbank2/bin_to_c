CFLAGS=-c -Wall -O0 
LIBS = 

all: bin_to_c 

bin_to_c: main.o 
	$(CC) main.o $(LIBS) -o bin_to_c 

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *o bin_to_c
