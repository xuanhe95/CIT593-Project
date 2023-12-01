/************************************************************************/
/* File Name : lc4_loader.c		 										*/
/* Purpose   : This file implements the loader (ld) from PennSim		*/
/*             It will be called by main()								*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_memory.h"
#include <string.h>
#include <stdlib.h>

/* declarations of functions that must defined in lc4_loader.c */

FILE* open_file(char* file_name)
{
	FILE *file;

	// Open the file for binary reading
	file = fopen(file_name, "rb");
	// Check if the file was opened successfully
	if (file == NULL) {
			fprintf(stderr, "error1: usage: ./lc4 <object_file.obj>\n");
			return NULL;
	} 
	return file;
}



int parse_file (FILE* my_obj_file, row_of_memory** memory){
  
/* remember to adjust 16-bit values read from the file for endiannness
 * remember to check return values from fread() and/or fgetc()
 */  
        // Taking input single character at a time
        unsigned short int c;
        unsigned short int cnext;
        do{  
            c = fgetc(my_obj_file);
        // Checking for end of file
        if ( c  == (unsigned short int)EOF){
            break;
        } else {
            cnext = fgetc(my_obj_file);
            if ( cnext  == (unsigned short int)EOF){
            break;
        }
        } 
		//printf("c is %02x \n", (unsigned short int) c);
        //printf("c next is %02x \n", (unsigned short int) cnext);
        //parse by code, data or label section  
        if ((c == 0xCA  && cnext == 0xDE) ||(c == 0xDA  && cnext == 0xDA) ) {
             int parse_code = code_section (my_obj_file,  memory); 
        }  else if((c == 0xC3  && cnext == 0xB7)){
            //printf("c is %02x \n", (unsigned short int) c);
            //printf("next is %02x \n", (unsigned short int) cnext);
            int parse_label = label_section (my_obj_file,  memory); 

        }  
		//printf("the address is: %04x \n", (unsigned short int) address_comb);  
        //printf("number of commads is:%04x \n", (unsigned short int) num_commands_comb); 
        //printf("number of commads is:%d \n", signedInt); 
        } while(1);
	return 0 ;
}

int code_section (FILE* my_obj_file, row_of_memory** memory){
    unsigned short int c ;
    short unsigned int address[2]; 
    short unsigned int num_commands[2];

    // to add address and number of ops
    for (int i = 0; i < 2; ++i) { 
        c =  fgetc(my_obj_file);
        address[i] = c;
        //printf("address: %02x \n", (unsigned short int) c );
            
        } 
    for (int j = 0; j < 2; ++j) {  
        c =  fgetc(my_obj_file);
        num_commands[j] = c;
        //printf("num_commands: %02x \n", (unsigned short int) c); 
    }  
    unsigned short int address_comb = (address[0]<<8 )  | address[1];  
    unsigned short int num_commands_comb = (num_commands[0]<<8)  | num_commands[1]; 
    int signedInt = (int) num_commands_comb ;
    //loop through contents 
    int looptimes = signedInt *2 ;
    short unsigned int contents_unswapped[looptimes];  
    unsigned short int contents_swap ;
    for (int k = 0; k< looptimes ; ++k) {  
            c =  fgetc(my_obj_file);
            contents_unswapped[k] = c;
            //printf("contents_unswapped: %02x \n", (unsigned short int) c); 
            if(k%2==1){
                contents_swap  = (contents_unswapped[k-1]<<8)| contents_unswapped[k]; 
                //printf("contents_swapped: %04x \n", (unsigned short int)contents_swap ); 
                // Add the values to the linked list
                add_to_list( memory, address_comb + (k-1)/2, contents_swap);

            }
        }   
    return 0;
}


int label_section (FILE* my_obj_file, row_of_memory** memory){ 
    unsigned short int c ;
    short unsigned int address[2]; 
    short unsigned int num_commands[2];

    // to add address and number of ops
    for (int i = 0; i < 2; ++i) { 
        c =  fgetc(my_obj_file);
        address[i] = c;
        //printf("address: %02x \n", (unsigned short int) c );
            
        } 
    for (int j = 0; j < 2; ++j) {  
        c =  fgetc(my_obj_file);
        num_commands[j] = c;
        //printf("num_commands: %02x \n", (unsigned short int) c); 
    }  
    unsigned short int address_comb = (address[0]<<8 )  | address[1];  
    unsigned short int num_commands_comb = (num_commands[0]<<8)  | num_commands[1]; 
    int signedInt = (int) num_commands_comb ;
    //printf("address is: %04x \n", address_comb); 
    //printf("number of elements: %d \n", signedInt ); 
    char label_letter ;

    // search the node with the address
    row_of_memory* head = *memory;
    //row_of_memory * ptr = memory;
    row_of_memory* node = search_address(head, address_comb);

    // if the address is not found, need to create a new node 
    if(node == NULL){
        //printf("node is null\n"  );
        //printf("address is: %04x \n", address_comb); 
        int add_res = add_to_list(memory, address_comb, 0x0000);
        printf("add node result: %d \n",add_res);
        row_of_memory* current = *memory;
		    //row_of_memory* previous = *memory;
        while (current != NULL) {
            if(current->address == address_comb){  
              node = current;
			  } // else, go to the next node
			    current = current->next;
        }
    }
    //printf("searched node \n" );
    //printf("node address is %04x \n" , node->address);  

    // allocate memory for the label
    node->label = (char*)  malloc(sizeof(char) * signedInt + 1);
    // Check if memory allocation was successful
    if (node->label == NULL) {
        perror("Memory allocation failed");
        return 1;
    }
    //printf("node label created.\n" ); 
    //create a string to hold contents
    char label_string[signedInt];
     
    //convert hex to characters
    for (int k = 0; k< signedInt; ++k) {  
        c =  fgetc(my_obj_file);
        //printf("contents_unswapped: %02x \n", c); 
        label_letter  = hexToChar(c);
        label_string[k] = label_letter;
        
        //printf("hex to ascii: %c \n", label_letter); 
          
    }   
    //printf("char string is: %s \n", label_string); 
    strcpy(node->label, label_string);
    
    // add null terminator to the label
    node->label[signedInt] = '\0';  
    return 0;
    
}


char hexToChar(unsigned short int  hexValue) {
    // Ensure the input is a valid hexadecimal digit
    if (hexValue >= 0x30 && hexValue <= 0x39) {
         
        return (char)('0' + (hexValue-0x30));
    } else if (hexValue >= 0x41 && hexValue <= 0x5A) {
        // If it's a letter (A-Z)
        return (char)('A' + (hexValue - 0x41));
    }else if (hexValue >= 0x61 && hexValue <= 0x7A) {
        // If it's a letter (a-z)
        return (char)('a' + (hexValue - 0x61));
    }else if (hexValue == 0x5F) {// If it's "_"
        return (char)('_');
    }
   else {
        // Handle invalid input
        return '\0';  // Null character signifies an error or invalid input
    }
}
