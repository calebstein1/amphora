TARGET = bin/untitled-platformer
SRC = $(wildcard src/*.c src/**/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
CC = cc
CFLAGS=-O2 -std=gnu99 -Werror=all -Werror=pedantic -Wextra -Wno-gnu-empty-initializer -fsanitize=address,undefined -Iinclude
LDFLAGS=-lSDL2

all: clean $(TARGET)

clean:
	rm -f obj/*.o
	rm -f obj/**/*.o
	rm -f bin/*

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) -o $@ $? $(CFLAGS) $(LDFLAGS)

obj/%.o: src/%.c
	mkdir -p obj/engine
	$(CC) -c $< -o $@ $(CFLAGS)
