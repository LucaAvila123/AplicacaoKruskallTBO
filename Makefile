TARGET=trab1
CC=gcc
LD=gcc
OBJS= g.o 
all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS)
 
g.o: g.c
	$(CC) -c $(CCFLAGS) g.c -o g.o
 
clean:
	rm -f *.o