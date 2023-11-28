/************************************************************************/
/* File Name : lc4_memory.h		 										*/
/* Purpose   : This program contains the structure for the LC4 memory's	*/
/* 			   linked list and the declarations of the helper functions */
/*             															*/
/* Author(s) : tjf 														*/
/************************************************************************/

#ifndef LC4_MEMORY_H
#define LC4_MEMORY_H

/*
 * definition of linked list node
 */

typedef struct row_of_memory_struct {
	short unsigned int address ;
	char * label ;
	short unsigned int contents ;
	char * assembly ;
	struct row_of_memory_struct *next ;
} row_of_memory ;


/*
 * declaration of linked list helper functions
 */

int add_to_list    	      (row_of_memory** head,
			       short unsigned int address,
			       short unsigned int contents) ;

row_of_memory* search_address (row_of_memory* head,
			       short unsigned int address ) ;

row_of_memory* search_opcode  (row_of_memory* head,
			       short unsigned int opcode  ) ;

void print_list		      (row_of_memory* head ) ;

int delete_list    	      (row_of_memory** head ) ;

int code_section (FILE* my_obj_file, row_of_memory** memory);
int label_section (FILE* my_obj_file, row_of_memory** memory);
char hexToChar(unsigned short int  hexValue) ;
int getAdd (unsigned short int instruction, row_of_memory* node);
int getMulSubDiv(unsigned short int instruction, row_of_memory* node, int OPS);
#endif
