EXEC=meeshell
CC=gcc
INCLUDE_DIR=include/
BUILD_DIR=build/
SRC_DIR=src/

BUILD_FLAGS=--std=gnu11 -I $(INCLUDE_DIR)
DEBUG_FLAGS=-Wall -Wextra  -Wpedantic -g -DDEBUG -fsanitize=address


SRC=$(filter-out $(SRC_DIR)test.c, $(wildcard $(SRC_DIR)*.c))
OBJ=$(patsubst $(SRC_DIR)%.c, $(BUILD_DIR)%.o, $(SRC))

all: clean debug

debug: BUILD_FLAGS += $(DEBUG_FLAGS) 
	#  Debug mode enabled
debug: $(EXEC)

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






