EXEC=meeshell
CC=gcc-12
INCLUDE_DIR=include/
BUILD_DIR=build/
SRC_DIR=src/

BUILD_FLAGS=--std=gnu11 -I $(INCLUDE_DIR)
DEBUG_FLAGS=-Wall -Wextra  -Wpedantic -g -DDEBUG


SRC=$(filter-out $(SRC_DIR)test.c, $(wildcard $(SRC_DIR)*.c))
OBJ=$(patsubst $(SRC_DIR)%.c, $(BUILD_DIR)%.o, $(SRC))


all: clean $(EXEC)
debug: $(EXEC)


debug: BUILD_CFLAGS += $(DEBUG_CFLAGS)
	#  Debug mode enabled

meeshell: $(OBJ)
	#  Building target
	$(CC) $(BUILD_FLAGS) $(OBJ) -o $(EXEC) 
build_dir:
	#  Creating build directory
	if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi

$(BUILD_DIR)%.o: $(SRC_DIR)%.c | build_dir
	$(CC) $(BUILD_FLAGS) -c $< -o $@



clean:
	#  Cleaning up
	if [ -d $(BUILD_DIR) ]; then rm -rf $(BUILD_DIR)/*; else mkdir $(BUILD_DIR); fi
	if [ -f $(EXEC) ]; then rm $(EXEC); fi






