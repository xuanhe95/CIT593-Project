/************************************************************************/
/* File Name : lc4_loader.c		 										*/
/* Purpose   : This file implements the loader (ld) from PennSim		*/
/*             It will be called by main()								*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
//#include "lc4_memory.h"
#include "lc4_loader.h"
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
        unsigned short int type;
        // error checking
        int err;

        for(;;){
            // taking input as two bytes
            err = read_next_half(my_obj_file, &type);
            if(err > 0){
                perror("parse_file() failed");
                delete_linked_list(*memory);
                return 1;
            } else if(err == -1){
                // end of file
                break;
            }

            if(type == 0xCADE || type == 0xDADA){
                int parse_code = code_section (my_obj_file,  memory);
            } else if(type == 0xC3B7){
                int parse_label = label_section (my_obj_file,  memory);
            } else{
                perror("parse_file() failed");
                delete_linked_list(*memory);
                return 1;
            }
        }

        //printf("the address is: %04x \n", (unsigned short int) address_comb);  
        //printf("number of commads is:%04x \n", (unsigned short int) num_commands_comb); 
        //printf("number of commads is:%d \n", signedInt); 




        // do{  
        //     c = fgetc(my_obj_file);
        // // Checking for end of file
        // if ( c  == (unsigned short int)EOF){
        //     break;
        //     }
			 
		// //printf("c is %02x \n", (unsigned short int) c);
        // //parse by code, data or label section  

        // //added by XHZ
        // //read next char
        // unsigned short int next_c = fgetc(my_obj_file);

        // if ((c == 0xCA  && fgetc(my_obj_file) == 0xDE) ||(c == 0xDA  && fgetc(my_obj_file) == 0xDA) ) {
        //      int parse_code = code_section (my_obj_file,  memory); 
        // }  else if((c == 0xC3  && fgetc(my_obj_file) == 0xB7)){
        //      int parse_label = label_section (my_obj_file,  memory); 

        // }
		// //printf("the address is: %04x \n", (unsigned short int) address_comb);  
        // //printf("number of commads is:%04x \n", (unsigned short int) num_commands_comb); 
        // //printf("number of commads is:%d \n", signedInt); 
        // } while(1);
	return 0 ;
}

int code_section (FILE* my_obj_file, row_of_memory** memory){
    int err;
    unsigned short int c ;
    unsigned short int address;
    unsigned short int n ;

    // read 2 byte address field, if eof or err, wrong format
    err = read_next_half(my_obj_file, &address);
    if(err != 0){
        perror("parse_file() failed");
        delete_linked_list(*memory);
        return 1;
    }
    // read 2 byte number field
    err = read_next_half(my_obj_file, &n);
    if(err != 0){
        perror("parse_file() failed");
        delete_linked_list(*memory);
        return 1;
    }
    // parse n * 2 bytes
    for(int i = 0; i < n; i++){
        // parse 2 bytes content
        err = read_next_half(my_obj_file, &c);
        if(err != 0){
            perror("parse_file() failed");
            delete_linked_list(*memory);
            return 1;
        }
        // add content to next address
        add_to_list( memory, address + i, c);
    }

    //short unsigned int address[2]; 
    //short unsigned int num_commands[2];

    // // to add address and number of ops
    // for (int i = 0; i < 2; ++i) { 
    //     c =  fgetc(my_obj_file);
    //     address[i] = c;
    //     //printf("address: %02x \n", (unsigned short int) c );
            
    //     } 
    // for (int j = 0; j < 2; ++j) {  
    //     c =  fgetc(my_obj_file);
    //     num_commands[j] = c;
    //     //printf("num_commands: %02x \n", (unsigned short int) c); 
    // }  
    // unsigned short int address_comb = (address[0]<<8 )  | address[1];  
    // unsigned short int num_commands_comb = (num_commands[0]<<8)  | num_commands[1]; 
    // int signedInt = (int) num_commands_comb ;
    // //loop through contents 
    // int looptimes = signedInt *2 ;
    // short unsigned int contents_unswapped[looptimes];  
    // unsigned short int contents_swap ;
    // for (int k = 0; k< looptimes ; ++k) {  
    //         c =  fgetc(my_obj_file);
    //         contents_unswapped[k] = c;
    //         //printf("contents_unswapped: %02x \n", (unsigned short int) c); 
    //         if(k%2==1){
    //             contents_swap  = (contents_unswapped[k-1]<<8)| contents_unswapped[k]; 
    //             //printf("contents_swapped: %04x \n", (unsigned short int)contents_swap ); 
    //             // Add the values to the linked list
    //             add_to_list( memory, address_comb + (k-1)/2, contents_swap);

    //         }
    //     }   
    return 0;
}


int label_section (FILE* my_obj_file, row_of_memory** memory){ 
    int err;
    unsigned short int c ;
    unsigned short int address;
    unsigned short int n ;

    // read 2 byte address field, if eof or err, wrong format
    err = read_next_half(my_obj_file, &address);
    if(err != 0){
        perror("parse_file() failed");
        delete_linked_list(*memory);
        return 1;
    }
    // read 2 byte number field
    err = read_next_half(my_obj_file, &n);
    if(err != 0){
        perror("parse_file() failed");
        delete_linked_list(*memory);
        return 1;
    }

        // search the node with the address
        row_of_memory* head = *memory;
        row_of_memory* node = search_address(address, head);
        // if the address is not found, return 1
        if(node == NULL){
            add_to_list (memory, address, 0x0000);
            //search node again
            node = search_address(head, address);
        }
        // allocate memory for the label
        node->label = (char*) malloc((sizeof(char) * n + 1));
        // check whether memory allocated successfully
        if (node->label == NULL) {
            perror("Memory allocation failed");
            return 1;
        }


        // read n bytes from the file
        for(int i = 0; i < n; i++){
            // read 1 byte from the file and store it in the label
            int err = read_next_char(my_obj_file, (node->label) + i);
            // if read failed, return 1
            if(err != 0){
                perror("parse_obj() failed");
                return 1;
            } 
        }
        // add null terminator to the label
        node->label[n] = '\0';  



    // unsigned short int c ;
    // short unsigned int address[2]; 
    // short unsigned int num_commands[2];

    // // to add address and number of ops
    // for (int i = 0; i < 2; ++i) { 
    //     c =  fgetc(my_obj_file);
    //     address[i] = c;
    //     //printf("address: %02x \n", (unsigned short int) c );
            
    //     } 
    // for (int j = 0; j < 2; ++j) {  
    //     c =  fgetc(my_obj_file);
    //     num_commands[j] = c;
    //     //printf("num_commands: %02x \n", (unsigned short int) c); 
    // }  
    // unsigned short int address_comb = (address[0]<<8 )  | address[1];  
    // unsigned short int num_commands_comb = (num_commands[0]<<8)  | num_commands[1]; 
    // int signedInt = (int) num_commands_comb ;
    // //printf("address is: %04x \n", address_comb); 
    // //printf("number of elements: %d \n", signedInt ); 
    // char label_letter ;

    // // search the node with the address
    // row_of_memory* head = *memory;
    // //row_of_memory** ptr = memory;
    // row_of_memory* node = search_address(head, address_comb);

    // // if the address is not found, need to create a new node 
    // if(node == NULL){
    //     add_to_list (memory, address_comb, 0x0000);
    //     //search node again
    //     node = search_address(head, address_comb);
    // }
    // // allocate memory for the label
    // node->label = malloc(sizeof(char) * signedInt + 1);
    // // Check if memory allocation was successful
    // if (node->label == NULL) {
    //     perror("Memory allocation failed");
    //     return 1;
    // }
    // //create a string to hold contents
    // char label_string[signedInt];
    
    // //convert hex to characters
    // for (int k = 0; k< signedInt; ++k) {  
    //     c =  fgetc(my_obj_file);
    //     label_letter  = hexToChar(c);
    //     label_string[k] = label_letter;
    //     //printf("contents_unswapped: %02x \n", c); 
    //     //printf("hex to ascii: %c \n", label_letter); 
          
    // }   
    // //printf("char string is: %s \n", label_string); 
    // strcpy(node->label, label_string);
    
    // // add null terminator to the label
    // node->label[signedInt] = '\0';  
    // return 0;
    
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
/************************************************************************/
/* File Name : lc4_loader.c 									        */
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : Xuanhe Zhang										    	*/
/************************************************************************/

//#include "lc4_loader.h"

// FILE* open_file(char* file_name){
//     FILE *fp;
//     // open the file in binary mode
//     fp = fopen(file_name, "rb");
//     // if open failed, return NULL
//     if (fp == NULL)
//     {
//         perror("open_file() failed");
//         return NULL;
//     }
//     return fp;
// }

// int parse_file (FILE* my_obj_file, row_of_memory** memory){
//     int err;
//     // check if the file is open
//     if (my_obj_file == NULL)
//     {
//         perror("file is not open");
//         return 1;
//     }
//     // parse the file
//     for(;;){
//         // read 2 byte header field
//         unsigned short type;

//         err = read_next_half(my_obj_file, &type);
//         if(err > 0){
//             perror("parse_file() failed");
//             delete_nodes(*memory);
//             return 1;
//         } else if(err < 0){
//             // end of file
//             break;
//         }
//         // if the header field is 0xCADE, 0xDADA, or 0xC3B7
//         if(type == 0xCADE || type == 0xDADA || type == 0xC3B7){
//             unsigned short address;
//             unsigned short n;
//             // read 2 byte address field
//             err = read_next_half(my_obj_file, &address);
//             if(err != 0){
//                 perror("parse_file() failed");
//                 delete_linked_list(*memory);
//                 return 1;
//             }
//             // read 2 byte n field
//             err = read_next_half(my_obj_file, &n);
//             if(err != 0){
//                 perror("parse_file() failed");
//                 delete_linked_list(*memory);
//                 return 1;
//             }
//             // parse the obj
//             parse_obj(type, address, n, my_obj_file, memory);
//         }
//         // else the file is not valid
//         else{
//             perror("parse_file() failed");
//             delete_linked_list(*memory);
//             return 1;
//         }
//     }
// }

int read_next_half(FILE* my_obj_file, unsigned short* half){
    // read 2 bytes from the file
    size_t bytes_read = fread(half, sizeof(unsigned short), sizeof(half), my_obj_file);
    // if read failed, return 1
    if(bytes_read != sizeof(half)){
        perror("read_half() failed");
        return 1;
    }
    // if end of file, return -1
    if(feof(my_obj_file)){
        return -1;
    }
    return 0;
}

int read_next_char(FILE* my_obj_file, unsigned char* c){
    // read 1 byte from the file
    size_t bytes_read = fread(c, sizeof(unsigned char), sizeof(c), my_obj_file);
    // if read failed, return 1
    if(bytes_read != sizeof(c)){
        perror("read_char() failed");
        return 1;
    }
    // if end of file, return -1
    if(feof(my_obj_file)){
        return -1;
    }
    return 0;
}

// int parse_obj(unsigned short type, unsigned short address, unsigned short n, FILE* my_obj_file, row_of_memory** head){
//     if(type == 0xCADE){
//         // prev_node is the node before the new node
//         row_of_memory* prev_node = *head;
//         for(int i = 0; i < n; i++){
//             // read 2 byte content field
//             unsigned short content;
//             int err = read_next_half(my_obj_file, &content);
//             // if read failed, return 1
//             if(err != 0){
//                 perror("parse_obj() failed");
//                 return 1;
//             }
//             // add the new node to the linked list, update address
//             add_to_list(prev_node, address++, content);
//             // update prev_node
//             prev_node = prev_node->next;
//         }
//     } else if(type == 0xDADA){

        
//     } else if(type == 0xC3B7){
//         // search the node with the address
//         row_of_memory* node = search_address(address, head);
//         // if the address is not found, return 1
//         if(node == NULL){
//             perror("parse_obj() failed");
//             return 1;
//         }
//         // allocate memory for the label
//         node->label = malloc(sizeof(char) * n + 1);
//         // read n bytes from the file
//         for(int i = 0; i < n; i++){
//             // read 1 byte from the file and store it in the label
//             int err = read_next_char(my_obj_file, (node->label) + i);
//             // if read failed, return 1
//             if(err != 0){
//                 perror("parse_obj() failed");
//                 return 1;
//             } 
//         }
//         // add null terminator to the label
//         node->label[n] = '\0';  
//     } else{
//         // if the type is not valid, return 1
//         perror("parse_obj() failed");
//         delete_nodes(*head);
//         return 1;
//     }
//     return 0;
// }