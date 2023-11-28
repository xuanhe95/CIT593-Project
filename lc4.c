/************************************************************************/
/* File Name : lc4.c 													*/
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_memory.h"
#include "lc4_loader.h"
#include "lc4_disassembler.h"

/* program to mimic pennsim loader and disassemble object files */

int main (int argc, char** argv) {
  
/* leave plenty of room for the filename */
  
  char filename[100];

	/**
	 * main() holds the linked list &
	 * only calls functions in other files
	 */

	/* step 1: create head pointer to linked list: memory 	*/
	/* do not change this line - there should no be malloc calls in main() */
	
	row_of_memory* memory = NULL ;


	/* step 2: determine filename, then open it		*/
	/*   TODO: extract filename from argv, pass it to open_file() */
  
	// Check if there is any argument
	if (argc <= 1) { 
	// If no argument was provided, print message 
			fprintf(stderr, "Error1: usage: no arguments. \n" );
			return 1;
	}  
	// If there are arguments, point "filename" to the first argument
	char* filename1 =argv[1]  ;	
	FILE* obj_file = open_file(filename1);  

	if (obj_file == NULL) { 
		// if File reading failed 
		printf("error: open_file() failed. \n");
		return -1;
	}
	

	/* step 3: call function: parse_file() in lc4_loader.c 	*/
	/*   TODO: call function & check for errors		*/
	int parse_result = parse_file (obj_file,  &memory);
	printf("parse_result = %d. \n", parse_result);
	fclose(obj_file); 
  
	//print linkedList
	print_list (memory);
	 
	/* step 4: call function: reverse_assemble() in lc4_disassembler.c */
	/*   TODO: call function & check for errors		*/
  int reverse_assemble_result = reverse_assemble (memory);

	/* step 5: call function: print_list() in lc4_memory.c 	*/
	/*   TODO: call function 				*/


	/* step 6: call function: delete_list() in lc4_memory.c */
	/*   TODO: call function & check for errors		*/


	/* only return 0 if everything works properly */
	return 0 ;
}
