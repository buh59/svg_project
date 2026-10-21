CC = gcc
CFLAGS = -std=c99 -Wall -Werror -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = obj

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/scene.c $(SRC_DIR)/export.c $(SRC_DIR)/storage.c $(SRC_DIR)/group.c $(SRC_DIR)/geometry.c $(SRC_DIR)/cli.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC = editor_svg

build: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(EXEC)

.PHONY: build clean
