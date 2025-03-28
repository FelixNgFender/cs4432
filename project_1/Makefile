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
TARGET_FLAGS=RR tests/16.in 4

# directories
TARGET=scheduler
SRC_DIR=src
WORKLOADS_DIR=workloads
TESTS_OUT_DIR=tests-out
BIN_DIR=bin
OBJ_DIR=obj
SRC=$(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c)
HEADERS=$(wildcard $(SRC_DIR)/*.h $(SRC_DIR)/*/*.h)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
WORKLOADS=$(wildcard $(WORKLOADS_DIR)/*.in)

# misc
SUBMITNAME=project_3.zip

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

.PHONY: test
test: build
	@cp $(BIN_DIR)/$(TARGET) ./$(TARGET)
	@echo "Binary moved to expected location. Now run ./run_tests.sh. Remember to clean up after yourself!"

.PHONY: workloads
workloads:
	$(MAKE) --no-print-directory workload-1 && \
	echo "=============================================================================" && \
	$(MAKE) --no-print-directory workload-2 && \
	echo "=============================================================================" && \
	$(MAKE) --no-print-directory workload-3 && \
	echo "=============================================================================" && \
	$(MAKE) --no-print-directory workload-4 && \
	echo "=============================================================================" && \
	$(MAKE) --no-print-directory workload-5 && \
	echo "============================================================================="

.PHONY: workload-1
workload-1: build
workload-1: TARGET_FLAGS=RR $(WORKLOADS_DIR)/workload_1.in 3
workload-1: run

.PHONY: workload-2
workload-2:
	$(MAKE) --no-print-directory workload-2-fifo && \
	echo "=============================================================================" && \
	$(MAKE) --no-print-directory workload-2-sjf

.PHONY: workload-2-fifo
workload-2-fifo: build
workload-2-fifo: TARGET_FLAGS=FIFO $(WORKLOADS_DIR)/workload_2.in 0
workload-2-fifo: run

.PHONY: workload-2-sjf
workload-2-sjf: build
workload-2-sjf: TARGET_FLAGS=SJF $(WORKLOADS_DIR)/workload_2.in 0
workload-2-sjf: run

.PHONY: workload-3
workload-3:
	$(MAKE) --no-print-directory workload-3-fifo && \
	echo "=============================================================================" && \
	$(MAKE) --no-print-directory workload-3-sjf && \
	echo "=============================================================================" && \
	$(MAKE) --no-print-directory workload-3-rr 

.PHONY: workload-3-fifo
workload-3-fifo: build
workload-3-fifo: TARGET_FLAGS=FIFO $(WORKLOADS_DIR)/workload_3.in 0
workload-3-fifo: run

.PHONY: workload-3-sjf
workload-3-sjf: build
workload-3-sjf: TARGET_FLAGS=SJF $(WORKLOADS_DIR)/workload_3.in 0
workload-3-sjf: run

.PHONY: workload-3-rr
workload-3-rr: build
workload-3-rr: TARGET_FLAGS=RR $(WORKLOADS_DIR)/workload_3.in 3
workload-3-rr: run

.PHONY: workload-4
workload-4: build
workload-4: TARGET_FLAGS=RR $(WORKLOADS_DIR)/workload_4.in 3
workload-4: run

.PHONY: workload-5
workload-5: build
workload-5: TARGET_FLAGS=FIFO $(WORKLOADS_DIR)/workload_5.in 0
workload-5: run

.PHONY: debug
debug: build
	$(DEBUGGER) $(BIN_DIR)/$(TARGET) $(TARGET_FLAGS)

.PHONY: clean-tests
clean-tests:
	$(RM) $(TESTS_OUT_DIR)/* ./$(TARGET)

.PHONY: clean
clean:  clean-tests
	$(RM) $(OBJ) $(BIN_DIR)/$(TARGET)

.PHONY: valgrind
valgrind: CFLAGS=$(VALGRIND_CFLAGS)
valgrind: LFLAGS=$(VALGRIND_LFLAGS)
valgrind: build
	valgrind --show-leak-kinds=all --leak-check=full --track-origins=yes --show-reachable=yes $(BIN_DIR)/$(TARGET) $(TARGET_FLAGS)

.PHONY: submit
submit:
	$(RM) $(SUBMITNAME)
	zip $(SUBMITNAME) $(SRC) $(HEADERS) $(WORKLOADS) Makefile README.*
