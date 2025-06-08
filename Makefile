# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -fPIC
LDFLAGS = -shared
LDLIBS = -L. -lmemory_manager -pthread

# Targets
MEM_MANAGER_SRC = memory_manager.c
MEM_MANAGER_LIB = libmemory_manager.so
MEM_MANAGER_OBJ = memory_manager.o

LIST_SRC = linked_list.c main.c
LIST_OBJ = linked_list.o main.o
LIST_EXE = list_app

INCLUDES = -I.

.PHONY: all clean mmanager list

# Default target
all: mmanager list

# Build dynamic memory manager library
mmanager: $(MEM_MANAGER_SRC)
	$(CC) $(CFLAGS) -c $(MEM_MANAGER_SRC)
	$(CC) $(LDFLAGS) -o $(MEM_MANAGER_LIB) $(MEM_MANAGER_OBJ)

# Build the linked list executable
list: $(LIST_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(LIST_SRC)
	$(CC) -o $(LIST_EXE) $(LIST_OBJ) $(LDLIBS)

# Clean build artifacts
clean:
	rm -f *.o *.so $(LIST_EXE)
