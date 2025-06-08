# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -fPIC
LIB_NAME = libmemory_manager.so

# Source and Object Files
MEM_SRC = memory_manager.c
MEM_OBJ = $(MEM_SRC:.c=.o)


all: mmanager list test_mmanager test_list


$(LIB_NAME): $(MEM_OBJ)
	$(CC) -shared -o $@ $(MEM_OBJ) -pthread


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


mmanager: $(LIB_NAME)


list: linked_list.o


test_mmanager: $(LIB_NAME)
	$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager -lm -pthread


test_list: $(LIB_NAME) linked_list.o
	$(CC) -o test_linked_list linked_list.c test_linked_list.c -L. -lmemory_manager -lm -pthread


run_tests: run_test_mmanager run_test_list


run_test_mmanager:
	LD_LIBRARY_PATH=. ./test_memory_manager 0


run_test_list:
	LD_LIBRARY_PATH=. ./test_linked_list 0

test_threaded: test_threaded.c linked_list.o libmemory_manager.so
	$(CC) -o test_threaded test_threaded.c linked_list.o -L. -lmemory_manager -lpthread

run_test_threaded:
	LD_LIBRARY_PATH=. ./test_threaded


clean:
	rm -f $(MEM_OBJ) $(LIB_NAME) test_memory_manager test_linked_list linked_list.o
