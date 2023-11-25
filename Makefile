
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99 -g
BUILD_DIR=build
SRC_DIR=src


all: clean main

main: main.o utils.o
	# Building the executable
	$(CC) $(CFLAGS)  $(BUILD_DIR)/main.o $(BUILD_DIR)/utils.o -o $(BUILD_DIR)/main

	# Creating a symbolic link to the executable
	if [ -L main ]; then rm main; fi
	ln -s $(BUILD_DIR)/main main

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

utils.o: $(SRC_DIR)/utils.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

dictionary.o: $(SRC_DIR)/dictionary.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/dictionary.c -o $(BUILD_DIR)/dictionary.o

clean:
	if [ -d $(BUILD_DIR) ]; then rm -rf $(BUILD_DIR)/*; else mkdir $(BUILD_DIR); fi
	if [ -L main ]; then rm main; fi






