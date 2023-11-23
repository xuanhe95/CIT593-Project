#include <stdio.h>
#include "lc4_memory.h"

row_of_memory* search_address (row_of_memory* head, short unsigned int address){
    // search the node with the address
    while(head != NULL){
        // if found, return the node
        if(head->address == address){
            return head;
        }
        // else, go to the next node
        head = head->next;
    }
    // if not found, return NULL
    return NULL;
}

row_of_memory* search_opcode  (row_of_memory* head, short unsigned int opcode  ) {
    // search the node with the opcode
    while(head != NULL){
        // if found, return the node
        if(head->contents == opcode){
            return head;
        }
        // else, go to the next node
        head = head->next;
    }
    // if not found, return NULL
    return NULL;
}

int add_to_list (row_of_memory** head, short unsigned int address, short unsigned int contents){
    // if the linked list is empty, create a new node and make it the head
    if(*head == NULL){
        *head = create_node(address, contents);
        return 0;
    }
    // else, add the new node to the end of the linked list
    row_of_memory* prev_node = *head;
    while(prev_node->next != NULL){
        prev_node = prev_node->next;
    }
    prev_node->next = create_node(address, contents);
    return 0;
}

row_of_memory* create_node(short unsigned int address, short unsigned int contents){
    // allocate memory for the new node
    row_of_memory* node = malloc(sizeof(row_of_memory));
    // if malloc failed, return NULL
    if(node == NULL){
        perror("malloc() failed");
        return NULL;
    }
    // initialize the new node
    node->address = address;
    node->contents = contents;
    node->label = NULL;
    node->assembly = NULL;
    node->next = NULL;
    // return the new node
    return node;
}

void delete_linked_list(row_of_memory* head){
    // if the linked list is empty, return
    if(head == NULL){
        return;
    }
    // else, delete the linked list recursively
    delete_linked_list(head->next);
    // free the memory allocated for the node
    free(head->label);
    free(head->assembly);
    free(head);
}
