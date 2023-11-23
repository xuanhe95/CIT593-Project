/************************************************************************/
/* File Name : lc4_loader.c 									        */
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : Xuanhe Zhang										    	*/
/************************************************************************/

#include "lc4_loader.h"

FILE* open_file(char* file_name){
    FILE *fp;
    // open the file in binary mode
    fp = fopen(file_name, "rb");
    // if open failed, return NULL
    if (fp == NULL)
    {
        perror("open_file() failed");
        return NULL;
    }
    return fp;
}

int parse_file (FILE* my_obj_file, row_of_memory** memory){
    int err;
    // check if the file is open
    if (my_obj_file == NULL)
    {
        perror("file is not open");
        return 1;
    }
    // parse the file
    for(;;){
        // read 2 byte header field
        unsigned short type;

        err = read_next_half(my_obj_file, &type);
        if(err > 0){
            perror("parse_file() failed");
            delete_nodes(*memory);
            return 1;
        } else if(err < 0){
            // end of file
            break;
        }
        // if the header field is 0xCADE, 0xDADA, or 0xC3B7
        if(type == 0xCADE || type == 0xDADA || type == 0xC3B7){
            unsigned short address;
            unsigned short n;
            // read 2 byte address field
            err = read_next_half(my_obj_file, &address);
            if(err != 0){
                perror("parse_file() failed");
                delete_linked_list(*memory);
                return 1;
            }
            // read 2 byte n field
            err = read_next_half(my_obj_file, &n);
            if(err != 0){
                perror("parse_file() failed");
                delete_linked_list(*memory);
                return 1;
            }
            // parse the obj
            parse_obj(type, address, n, my_obj_file, memory);
        }
        // else the file is not valid
        else{
            perror("parse_file() failed");
            delete_linked_list(*memory);
            return 1;
        }
    }


}

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

int parse_obj(unsigned short type, unsigned short address, unsigned short n, FILE* my_obj_file, row_of_memory** head){
    if(type == 0xCADE){
        // prev_node is the node before the new node
        row_of_memory* prev_node = *head;
        for(int i = 0; i < n; i++){
            // read 2 byte content field
            unsigned short content;
            int err = read_next_half(my_obj_file, &content);
            // if read failed, return 1
            if(err != 0){
                perror("parse_obj() failed");
                return 1;
            }
            // add the new node to the linked list, update address
            add_to_list(prev_node, address++, content);
            // update prev_node
            prev_node = prev_node->next;
        }
    } else if(type == 0xDADA){

        
    } else if(type == 0xC3B7){
        // search the node with the address
        row_of_memory* node = search_address(address, head);
        // if the address is not found, return 1
        if(node == NULL){
            perror("parse_obj() failed");
            return 1;
        }
        // allocate memory for the label
        node->label = malloc(sizeof(char) * n + 1);
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
    } else{
        // if the type is not valid, return 1
        perror("parse_obj() failed");
        delete_nodes(*head);
        return 1;
    }
    return 0;
}
