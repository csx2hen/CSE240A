CC=gcc
OPTS=-g -std=c99 -Werror

all: main.o predictor.o gshare.o tournament.o custom.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o gshare.o tournament.o custom.o

main.o: main.c predictor.h
	$(CC) $(OPTS) -c main.c

predictor.o: gshare.h tournament.h custom.h predictor.h predictor.c
	$(CC) $(OPTS) -c predictor.c

gshare.o: gshare.h gshare.c
	$(CC) $(OPTS) -c gshare.c

tournament.o: tournament.h tournament.c
	$(CC) $(OPTS) -c tournament.c

custom.o: custom.h tournament.c
	$(CC) $(OPTS) -c custom.c

clean:
	rm -f *.o predictor;