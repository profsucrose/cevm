TARGET = cevm

CC = cc
CFLAGS = -g -Wall -std=c99 -fshort-enums -I src/vendor
OBJ = obj
SRC = src
VENDOR = vendor

SOURCES = $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/$(VENDOR)/*/*.c)
OBJECTS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

BUILD_DIRS = $(OBJ) $(patsubst $(SRC)/%, $(OBJ)/%, $(wildcard $(SRC)/$(VENDOR)/*))

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(OBJ)/%.o: $(SRC)/%.c $(BUILD_DIRS)
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

$(BUILD_DIRS):
	mkdir -p $(BUILD_DIRS)

.PHONY clean:
clean:
	rm -rf $(TARGET) $(OBJ)/**