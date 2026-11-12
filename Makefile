CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -Iinclude
LDFLAGS = -lm

SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BIN_DIR = bin

SRCS = $(SRC_DIR)/account.c $(SRC_DIR)/loan.c $(SRC_DIR)/transaction.c
OBJS = $(SRCS:.c=.o)

TARGET = $(BIN_DIR)/banking_system
TEST_TARGET = $(BIN_DIR)/test_runner

.PHONY: all clean test

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS) $(SRC_DIR)/main.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TEST_TARGET): $(OBJS) $(TEST_DIR)/test_bank.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(BIN_DIR)
