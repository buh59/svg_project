CC = gcc
CFLAGS = -std=c99 -Wall -Werror -Wextra -Iinclude
SRC = src/main.c src/scene.c src/export.c src/storage.c src/group.c src/geometry.c src/shape.c
OBJ = $(SRC:.c=.o)
EXEC = editor_svg

build: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(EXEC)

.PHONY: build clean
