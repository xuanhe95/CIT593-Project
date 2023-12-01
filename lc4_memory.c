/************************************************************************/
/* File Name : lc4_memory.c		 										*/
/* Purpose   : This file implements the linked_list helper functions	*/
/* 			   to manage the LC4 memory									*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_memory.h"


/*
 * adds a new node to a linked list pointed to by head
 */
int add_to_list (row_of_memory** head,
		 short unsigned int address,
		 short unsigned int contents)
{

    /* check to see if there is already an entry for this address and update the contents.  no additional steps required in this case */
    row_of_memory* current = *head;
		row_of_memory* previous = *head;
     
    while (current != NULL && current->address <= address) {
        if (current->address == address ) {
            // if there is already an entry for this address, then update the contents.
            current->contents = contents;
            return 0;
        }
				previous = current;
        current = current->next;
         
    } 
    /* allocate memory for a single node */
		row_of_memory* new_node =(row_of_memory*) malloc(sizeof(row_of_memory)); 

	/* populate fields in newly allocated node w/ address&contents, NULL for label and assembly */
	/* do not malloc() storage for label and assembly here - do it in parse_file() and reverse_assemble() */
	  if (new_node == NULL) {
        // Memory allocation failed
        printf("Error: Memory allocation failed.\n");
        return -1;
    }
		new_node->address = address;
		new_node->contents = contents;
		new_node->label = NULL;
		new_node->assembly = NULL;
		new_node->next = current; //if current is null, new node is the last node; if not, new node connects to the current, which is larger than new node's address

	/* if *head is NULL, node created is the new head of the list! */
	  if (*head == NULL) {
				*head = new_node;
		}

	/* otherwise, insert node into the list in address ascending order */
	  else{
					 previous->next = new_node; 
           //new_node->next = current;
		} 
	/* return 0 for success, -1 if malloc fails */

	return 0 ;
}



/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
			       short unsigned int address )
{
	/* traverse linked list, searching each node for "address"  */
  while(head != NULL){
      /* return pointer to node in the list if item is found */
			if(head->address == address){
					return head;
			}
			// else, go to the next node
			head = head->next;
    }
  /* return NULL if list is empty or if "address" isn't found */
	return NULL ;
}

/*
 * search linked list by opcode field, returns node if found
 */
row_of_memory* search_opcode  (row_of_memory* head,
				      short unsigned int opcode  )
{
    /* opcode parameter is in the least significant 4 bits of the short int and ranges from 0-15 */
		/* see assignment instructions for a detailed description */
    
    /* traverse linked list until node is found with matching opcode in the most significant 4 bits
	   AND "assembly" field of node is NULL */
     while(head != NULL){
      /* return pointer to node in the list if item is found */
			if(((head->contents >>12  ) == opcode) && (head->assembly == NULL)){ //&15 makes sure that it reads the last 4 bits only
					return head;
			}
			// else, go to the next node
			head = head->next;
    } 
	/* return NULL if list is empty or if no matching nodes */
	return NULL ;
}
 
void print_list (row_of_memory* head )
{
	/* make sure head isn't NULL */
  if(head == NULL){
		printf("head of the linkedList is missing.");
		return;
	} 
	/* print out a header */  
	printf("<label>  \t %-20s  %-20s  %-20s \n", "<address>           ", "<contents>          ", "<assembly>         "); 
	 
  /* don't print assembly directives for non opcode 1 instructions if you are doing extra credit */
	/* traverse linked list, print contents of each node */
  row_of_memory* current = head; 
	while (current != NULL) {
		    printf("%s \t  %-20.04x %-20.04x %-20s \n", current->label, current->address,current->contents, current->assembly); 
	 
        //printf("current's address is %04x  \n", current->address);
				//printf("current's contents: %04x \n",  current->contents); 
        //printf("current's label is %s \n", current->label);
        current = current->next;
    } 
	 
}

/*
 * delete entire linked list
 */
int delete_list    (row_of_memory** head )
{
	/* delete entire list node by node */
	/* set the list head pointer to NULL upon deletion */ 

    row_of_memory* current = *head;
    row_of_memory* next = *head;
 

    while(current != NULL){
        next = current->next;
        // free the memory allocated for the node
        free(current->label);
        free(current->assembly);
        free(current);
        // go to the next node
        current = next;
    } 
    // set the list head pointer to NULL upon deletion
    *head = NULL;
  
	return 0 ;
}
