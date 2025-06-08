#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "linked_list.h"
#include "memory_manager.h"

static pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;

void list_init(Node** head, size_t size) {
    pthread_mutex_lock(&list_lock);
    *head = NULL;
    mem_init(size);
    pthread_mutex_unlock(&list_lock);
}

void list_insert(Node** head, uint16_t data) {
    pthread_mutex_lock(&list_lock);
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("ERROR: Failed to allocate node for value %u\n", data);
        pthread_mutex_unlock(&list_lock);
        return;
    }

    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    pthread_mutex_unlock(&list_lock);
}

void list_insert_after(Node* prev_node, uint16_t data) {
    pthread_mutex_lock(&list_lock);
    if (prev_node == NULL) {
        printf("Error: Provided previous node is NULL.\n");
        pthread_mutex_unlock(&list_lock);
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Error: Memory allocation failed.\n");
        pthread_mutex_unlock(&list_lock);
        return;
    }

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
    pthread_mutex_unlock(&list_lock);
}

void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    pthread_mutex_lock(&list_lock);
    if (head == NULL || *head == NULL || next_node == NULL) {
        printf("Error: Invalid input.\n");
        pthread_mutex_unlock(&list_lock);
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Error: Allocation failed.\n");
        pthread_mutex_unlock(&list_lock);
        return;
    }

    new_node->data = data;

    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
        pthread_mutex_unlock(&list_lock);
        return;
    }

    Node* current_node = *head;
    while (current_node != NULL && current_node->next != next_node) {
        current_node = current_node->next;
    }

    if (current_node == NULL) {
        printf("Error: Target node not found.\n");
        mem_free(new_node);
        pthread_mutex_unlock(&list_lock);
        return;
    }

    new_node->next = next_node;
    current_node->next = new_node;
    pthread_mutex_unlock(&list_lock);
}

void list_delete(Node** head, uint16_t data) {
    pthread_mutex_lock(&list_lock);
    if (head == NULL || *head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return;
    }

    Node* current = *head;
    Node* prev = NULL;

    while (current != NULL && current->data != data) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        pthread_mutex_unlock(&list_lock);
        return;
    }

    if (prev == NULL) {
        *head = current->next;
    } else {
        prev->next = current->next;
    }

    mem_free(current);
    pthread_mutex_unlock(&list_lock);
}

Node* list_search(Node** head, uint16_t data) {
    pthread_mutex_lock(&list_lock);
    if (head == NULL || *head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return NULL;
    }

    Node* current = *head;
    while (current != NULL) {
        if (current->data == data) {
            pthread_mutex_unlock(&list_lock);
            return current;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&list_lock);
    return NULL;
}

void list_display(Node** head) {
    pthread_mutex_lock(&list_lock);
    if (head == NULL || *head == NULL) {
        printf("[]\n");
        pthread_mutex_unlock(&list_lock);
        return;
    }

    printf("[");
    Node* current = *head;
    int first = 1;
    while (current != NULL) {
        if (!first) printf(", ");
        printf("%u", current->data);
        first = 0;
        current = current->next;
    }
    printf("]\n");
    pthread_mutex_unlock(&list_lock);
}

void list_display_range(Node** head, Node* start_node, Node* end_node) {
    pthread_mutex_lock(&list_lock);
    Node* current = start_node != NULL ? start_node : (head ? *head : NULL);
    int first = 1;
    printf("[");
    while (current != NULL && current != end_node->next) {
        if (!first) printf(", ");
        printf("%u", current->data);
        first = 0;
        if (current == end_node) break;
        current = current->next;
    }
    printf("]\n");
    pthread_mutex_unlock(&list_lock);
}

int list_count_nodes(Node** head) {
    pthread_mutex_lock(&list_lock);
    if (head == NULL || *head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return 0;
    }

    int count = 0;
    Node* current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    pthread_mutex_unlock(&list_lock);
    return count;
}

void list_cleanup(Node** head) {
    pthread_mutex_lock(&list_lock);
    if (head == NULL || *head == NULL) {
        pthread_mutex_unlock(&list_lock);
        return;
    }

    Node* walker = *head;
    while (walker != NULL) {
        Node* temp = walker->next;
        mem_free(walker);
        walker = temp;
    }

    *head = NULL;
    mem_deinit();
    pthread_mutex_unlock(&list_lock);
}
