# options
CC=gcc
LINKER=$(CC)
DEBUGGER=gdb
CFLAGS=-std=c99 -g -Wall -Wextra -Werror -pedantic -O2 -fsanitize=address
LFLAGS=-fsanitize=address
RELEASE_CFLAGS=-std=c99 -Wall -O2 -DNDEBUG
RELEASE_LFLAGS=
VALGRIND_CFLAGS=-std=c99 -g -Wall -Wextra -Werror -pedantic -O2 # valgrind does not work together with asan
VALGRIND_LFLAGS=
TARGET_FLAGS=

# directories
TARGET=main
SRC_DIR=src
BIN_DIR=bin
DATA_DIR=$(BIN_DIR)/Project1
OBJ_DIR=obj
SRC=$(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c)
HEADERS=$(wildcard $(SRC_DIR)/*.h $(SRC_DIR)/*/*.h)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# misc
SUBMITNAME=project_1.zip

default: build

.PHONY: build
build: $(BIN_DIR) $(BIN_DIR)/$(TARGET)

.PHONY: release
release: CFLAGS=$(RELEASE_CFLAGS)
release: LFLAGS=$(RELEASE_LFLAGS)
release: clean
release: build

$(BIN_DIR):
	mkdir -p $@

$(DATA_DIR): $(BIN_DIR)
	mkdir -p $@

# compile
$(OBJ): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

# link
$(BIN_DIR)/$(TARGET): $(OBJ)
	$(LINKER) $(OBJ) $(LFLAGS) -o $@

.PHONY: run
run: build
	$(BIN_DIR)/$(TARGET) $(TARGET_FLAGS)

.PHONY: debug
debug: build
	$(DEBUGGER) $(BIN_DIR)/$(TARGET) $(TARGET_FLAGS)

.PHONY: clean
clean:
	$(RM) $(OBJ) $(BIN_DIR)/$(TARGET) $(DATA_DIR)/*

.PHONY: valgrind
valgrind: CFLAGS=$(VALGRIND_CFLAGS)
valgrind: LFLAGS=$(VALGRIND_LFLAGS)
valgrind: build
	valgrind --show-leak-kinds=all --leak-check=full --track-origins=yes --show-reachable=yes $(BIN_DIR)/$(TARGET) $(TARGET_FLAGS)

.PHONY: submit
submit:
	$(RM) $(SUBMITNAME)
	zip $(SUBMITNAME) $(SRC) $(HEADERS) Makefile README.*
