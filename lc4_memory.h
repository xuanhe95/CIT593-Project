/************************************************************************/
/* File Name : lc4_memory.h		 										*/
/* Purpose   : This program contains the structure for the LC4 memory's	*/
/* 			   linked list and the declarations of the helper functions */
/*             															*/
/* Author(s) : tjf 														*/
/************************************************************************/

#ifndef LC4_MEMORY_H
#define LC4_MEMORY_H

// for clang
#include <stdlib.h>


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



row_of_memory* create_node(short unsigned int address, short unsigned int contents) ;
//void delete_linked_list(row_of_memory* head) ;

int code_section (FILE* my_obj_file, row_of_memory** memory);
int label_section (FILE* my_obj_file, row_of_memory** memory);
char hexToChar(unsigned short int  hexValue) ;
int getAdd (unsigned short int instruction, row_of_memory* node);
int getMulSubDiv(unsigned short int instruction, row_of_memory* node, int OPS);


void print_list_to_file(char* save_filename, row_of_memory* head, row_of_memory** memory);
// added by xhz
//row_of_memory* create_node(short unsigned int address, short unsigned int contents) ;
int process_new_line(FILE* file, int type, const row_of_memory* current, row_of_memory** memory);
int process_new_type(FILE* file, int type);
int process_new_addr(FILE* file, int type, const row_of_memory* current, row_of_memory** memory);
int process_exist_line(FILE* file, int type, const row_of_memory* previous, const row_of_memory* current, row_of_memory** memory);


//added by XHZ
int parse_contents(FILE* file, row_of_memory* current, row_of_memory** memory);
int parse_br(FILE* file, row_of_memory* current, row_of_memory** memory);
int parse_cmp(FILE* file, int contents);
int parse_jsr(FILE* file, row_of_memory* current, row_of_memory** memory);
int parse_logic(FILE* file, int contents);
int parse_ldr(FILE* file, int contents);
int parse_str(FILE* file, int contents);
int parse_rti(FILE* file, int contents);
int parse_const(FILE* file, int contents);
int parse_shift(FILE* file, int contents);
int parse_jmp(FILE* file, row_of_memory* current, row_of_memory** memory);
int parse_hiconst(FILE* file, int contents);
int parse_trap(FILE* file, int contents);



//added by XHZ


int read_next_half(FILE* my_obj_file, unsigned short* half) ;
//int parse_obj(unsigned short type, unsigned short address, unsigned short n, FILE* my_obj_file, row_of_memory** head) ;
int read_next_char(FILE* my_obj_file, unsigned char* c) ;


void handleEndianness(unsigned short int* half);

#endif
