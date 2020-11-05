CC=clang
FLAGS=-Ofast -Wall -std=c99
LIBS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image 

all: main.c
	$(CC) $(FLAGS) main.c -o main $(LIBS)

clean:
	rm main