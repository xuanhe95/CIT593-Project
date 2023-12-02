/************************************************************************/
/* File Name : lc4_loader.c		 										*/
/* Purpose   : This file implements the loader (ld) from PennSim		*/
/*             It will be called by main()								*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_memory.h"

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
            puts("looping");
            // taking input as two bytes
            err = read_next_half(my_obj_file, &type);
            if(err > 0){
                perror("parse_file() failed");
                printf("err is %d \n", err);
                delete_list(memory);
                return 1;
            } else if(err < 0){
                // end of file
                break;
            }

            printf("type is %x \n", type);
            printf("type is %x \n", 0xCADE);

            if((int)type == 0xCADE ||(int ) type == 0xDADA){
                printf("parssing code\n");
                int parse_code = code_section (my_obj_file,  memory);
                if(parse_code != 0){
                    perror("parse_file() failed\n");
                    printf("err is %d \n", err);
                    delete_list(memory);
                    return 1;
                }
            } else if(type == 0xC3B7){
                printf("parssing label\n");
                int parse_label = label_section (my_obj_file,  memory);
            } else{
                perror("parse_file() failed");
                printf("err is %d \n", err);
                delete_list(memory);
                return 1;
            }
        }
	return 0 ;
}

int code_section (FILE* my_obj_file, row_of_memory** memory){
    int err;
    unsigned short int c ;
    unsigned short int address;
    unsigned short int n ;

    // read 2 byte address field, if eof or err, wrong format
    err = read_next_half(my_obj_file, &address);
    printf("code address is %x \n", address);
    if(err != 0){
        perror("parse_file() failed");
        delete_list(memory);
        return 1;
    }
    // read 2 byte number field
    err = read_next_half(my_obj_file, &n);
    printf("n is %x \n", n);
    if(err != 0){
        perror("parse_file() failed");
        delete_list(memory);
        return 1;
    }
    // parse n * 2 bytes
    for(unsigned int i = 0; i < n; i++){
        printf("looping left %d \n", n - i);
        // parse 2 bytes content
        err = read_next_half(my_obj_file, &c);
        if(err != 0){
            perror("parse_file() failed");
            delete_list(memory);
            return 1;
        }
        printf("c is %x \n", c);
        printf("address + i is %u \n", address + i);
        // add content to next address
        add_to_list( memory, (short unsigned int)(address + i), c);
    }
    printf("end of code section\n");
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
        delete_list(memory);
        return 1;
    }
    printf("label address is %x \n", address);
    // read 2 byte number field
    err = read_next_half(my_obj_file, &n);
    if(err != 0){
        perror("parse_file() failed");
        delete_list(memory);
        return 1;
    }
    printf("label n is %x \n", n);

        // search the node with the address
        row_of_memory* head = *memory;
        row_of_memory* node = search_address(*memory, address);

        // if the address is not found, return 1
        if(node == NULL){
            add_to_list (memory, address, 0x0000);

            printf("add to list is done\n");
            printf("search address is %x \n", address);
            //search node again
            node = search_address(*memory, address);
            printf("search address is done/n");
        }

        printf("node address is %x \n", node->address);

        // allocate memory for the label
        node->label = (char*) malloc((sizeof(char) * n + 1));
        // check whether memory allocated successfully
        if (node->label == NULL) {
            perror("Memory allocation failed");
            return 1;
        }


        // read n bytes from the file
        for(int i = 0; i < n; i++){
            printf("looping left %d \n", n - i);
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

int read_next_half(FILE* my_obj_file, unsigned short* half){
    printf("reading half\n");
    // read 2 bytes from the file
    size_t bytes_read = fread(half, sizeof(unsigned short), 1, my_obj_file);
    // if read failed, return 1
    // if(bytes_read != sizeof(unsigned short)){
    //     perror("read_half() failed");
    //     printf("bytes_read is %d \n", bytes_read);
    //     printf("sizeof(unsigned short) is %d \n", sizeof(unsigned short));
    //     return 1;
    // }
    printf("read is %x\n", (*half & 0xFF00) >> 8);
    printf("read is %x\n", *half & 0x00FF);

    handleEndianness(half);
    printf("read is %x\n", (*half & 0xFF00) >> 8);
    printf("read is %x\n", *half & 0x00FF);

    // if end of file, return -1
    if(feof(my_obj_file)){
        printf("end of file\n");
        return -1;
    }
    printf("end of next half\n");
    return 0;
}

int read_next_char(FILE* my_obj_file, unsigned char* c){
    printf("reading char\n");
    // read 1 byte from the file
    size_t bytes_read = fread(c, sizeof(unsigned char), 1, my_obj_file);
    // if read failed, return 1
    if(bytes_read != sizeof(unsigned char)){
        perror("read_char() failed");
        return 1;
    }

    printf("char is %c\n", *c);
    // if end of file, return -1
    if(feof(my_obj_file)){
        return -1;
    }
    return 0;
}

void handleEndianness(unsigned short int* half){
    // swap the bytes
    unsigned short int temp = *half;
    *half = (temp << 8) | (temp >> 8);
}
