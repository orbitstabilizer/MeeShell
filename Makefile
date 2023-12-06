EXEC=meeshell
CC=gcc
INCLUDE_DIR=include/
BUILD_DIR=build/
SRC_DIR=src/

BUILD_FLAGS=--std=gnu11 -I $(INCLUDE_DIR)
DEBUG_FLAGS=-Wall -Wextra  -Wpedantic -g -DDEBUG


SRC=$(filter-out $(SRC_DIR)test.c, $(wildcard $(SRC_DIR)*.c))
OBJ=$(patsubst $(SRC_DIR)%.c, $(BUILD_DIR)%.o, $(SRC))


debug: $(EXEC)
all: clean $(EXEC)


debug: BUILD_CFLAGS += $(DEBUG_CFLAGS)

meeshell: $(OBJ)
	# Building the executable
	$(CC) $(BUILD_FLAGS) $(OBJ) -o $(EXEC) 

	# Creating a symbolic link to the executable
	# if [ -L main ]; then rm main; fi
	# ln -s $(BUILD_DIR)$(EXEC) $(EXEC)

build_dir:
	if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi

$(BUILD_DIR)%.o: $(SRC_DIR)%.c | build_dir
	$(CC) $(BUILD_FLAGS) -c $< -o $@



clean:
	if [ -d $(BUILD_DIR) ]; then rm -rf $(BUILD_DIR)/*; else mkdir $(BUILD_DIR); fi
	if [ -f $(EXEC) ]; then rm $(EXEC); fi






