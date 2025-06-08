# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -fPIC
LDFLAGS = -shared
LDLIBS = -L. -lmemory_manager -pthread
INCLUDES = -I.

# Files
MEM_MANAGER_SRC = memory_manager.c
MEM_MANAGER_LIB = libmemory_manager.so
MEM_MANAGER_OBJ = memory_manager.o

LINKED_LIST_SRC = linked_list.c
LINKED_LIST_OBJ = linked_list.o

TEST_LIST_SRC = test_linked_list.c
TEST_LIST_BIN = test_linked_list

.PHONY: all clean mmanager list

# Default build target
all: mmanager list

# Compile the memory manager as a shared library
mmanager: $(MEM_MANAGER_SRC)
	$(CC) $(CFLAGS) -c $(MEM_MANAGER_SRC)
	$(CC) $(LDFLAGS) -o $(MEM_MANAGER_LIB) $(MEM_MANAGER_OBJ)

# Build the test executable for the linked list
list: $(LINKED_LIST_SRC) $(TEST_LIST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(LINKED_LIST_SRC)
	$(CC) $(CFLAGS) -o $(TEST_LIST_BIN) $(TEST_LIST_SRC) $(LINKED_LIST_OBJ) $(LDLIBS)

# Clean up build artifacts
clean:
	rm -f *.o *.so $(TEST_LIST_BIN)
