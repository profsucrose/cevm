TARGET = cevm

CC = cc
CFLAGS = -g -Wall -std=c99 -fshort-enums
OBJ = obj
SRC = src

SOURCES = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

.PHONY clean:
clean:
	rm -f $(TARGET) $(OBJECTS)