/************************************************************************/
/* File Name : lc4_memory.c		 										*/
/* Purpose   : This file implements the linked_list helper functions	*/
/* 			   to manage the LC4 memory									*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_memory.h"

// for clang
#include <stdlib.h>


const int CODE_TYPE = 1;
const int DATA_TYPE = 2;

/*
 * adds a new node to a linked list pointed to by head
 */
int add_to_list (row_of_memory** head,
		 short unsigned int address,
		 short unsigned int contents)
{

    printf("add to list address is %04x  \n", address);

    /* check to see if there is already an entry for this address and update the contents.  no additional steps required in this case */
    row_of_memory* current = *head;
	row_of_memory* previous = NULL;
    while (current != NULL && current->address <= address) {
		previous = current;
        current = current->next;
    } 
    if(previous != NULL && previous->address == address){
        // if there is already an entry for this address, then update the contents.
        previous->contents = contents;
        return 0;
    }



    /* allocate memory for a single node */
	row_of_memory* new_node = (row_of_memory*) malloc(sizeof(row_of_memory) * 1); 

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

    printf("add to list address is %04x  \n", address);
    printf("new_node's address is %04x  \n", new_node->address);

	/* if *head is NULL, node created is the new head of the list! */
	if (*head == NULL) {
        printf("head is null\n");
		*head = new_node;
	}

    else if(previous == NULL){
        printf("previous is null\n");
        // if the new node is the first node
        *head = new_node;
    }

	/* otherwise, insert node into the list in address ascending order */
	else{
        printf("else\n");
		previous->next = new_node;
        //new_node->next = current;
	}
	/* return 0 for success, -1 if malloc fails */

    printf("add to list is done\n");

	return 0 ;
}



/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
			       short unsigned int address )
{
	/* traverse linked list, searching each node for "address"  */
    printf("search address address is %04x  \n", address);
    if(head == NULL){
        printf("head is null!");
    }

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
  row_of_memory* previous = NULL;
	while (current != NULL) {
		printf("%s \t  %-20.04x %-20.04x %-20s \n", current->label, current->address,current->contents, current->assembly); 
        //printf("next");
	
        //printf("current's address is %04x  \n", current->address);
				//printf("current's contents: %04x \n",  current->contents); 
        //printf("current's label is %s \n", current->label);

        previous = current;
        current = current->next;
    } 

	 
}


void print_list_to_file(char* filename, row_of_memory* head, row_of_memory** memory){
    if(*memory == NULL){
        printf("head of the linkedList is missing.");
        return;
        }
    	/* make sure head isn't NULL */
  if(head == NULL){
		printf("head of the linkedList is missing.");
		return;
	} 
    // open files
    FILE *file = fopen(filename, "w");

    // check whether open successfully
    if (file == NULL) {
        printf("Open files failed\n");
        return;
    }
    // address of the current node
    int address;
    // type of the current node
    int current_type = NULL;
    // type of the previous node
    int previous_type = NULL;

    // traverse the linked list
    row_of_memory* current = head;
    row_of_memory* previous = NULL;

    while(current != NULL){

        if(current->label != NULL){
            fprintf(file, "%s\n", current->label);
        }


        previous_type = current_type;
        
        if(current->address >= 0x0000 && current->address < 0x2000){
            current_type = CODE_TYPE;
        } else if(current->address >= 0x2000 && current->address < 0x8000){
            current_type = DATA_TYPE;
        } else{
            perror("Invalid address");
        }

        if(previous != NULL){
            printf("%hu\n", previous->address);
        }
        printf("%hu\n", current->address);
        printf("%d\n", previous_type);
        printf("%d\n", current_type);


        // initial line
        if(previous_type == NULL){
            process_new_line(file, current_type, current, memory);
        }
        // if current type is the same as previous type
        else if(previous_type == current_type){
            process_exist_line(file, current_type, previous, current, memory);
        }
        // if current type is different from previous type
        else{
            process_new_line(file, current_type, current, memory);
        }

        // update the previous and current node
        previous = current;
        current = current->next;
    }


    // close the file
    fclose(file);


}

int process_new_type(FILE* file, int type){
    if(type == CODE_TYPE){
        fprintf(file, ".CODE\n");
    } else if(type == DATA_TYPE){
        fprintf(file, ".DATA\n");
    }
    return 0;
}


int process_new_line(FILE* file, int type, const row_of_memory* current, row_of_memory** memory){
    process_new_type(file, type);
    process_new_addr(file, type, current, memory);
    return 0;
}

int process_new_addr(FILE* file, int type, const row_of_memory* current, row_of_memory** memory){
    if(type == CODE_TYPE){
        // add the address of the current node
        fprintf(file, ".ADDR %u\n", current->address);
        // if the current node has assembly, add the assembly
        if(current->assembly != NULL){
            fprintf(file, "%s\n", current->assembly);
        }
        else{
                            printf("parse_address: x%x\n", current->address & 0xFFFF);
            printf("parse_contents: x%x\n", current->contents & 0xFFFF);
            parse_contents(file, current, memory);
        }
    } else if(type == DATA_TYPE){
        // add the address of the current node
        fprintf(file, ".ADDR 0x%x\n", current->address);

        unsigned short contents = current->contents;
        fprintf(file, ".FILL 0x%x\n", contents >> 8);
        fprintf(file, ".FILL 0x%x\n", contents & 0xFF);
    }
    return 0;
}

int process_exist_line(FILE* file, int type, const row_of_memory* previous, const row_of_memory* current, row_of_memory** memory){
    // consective address
    if(previous->address == current->address-1){
        if(type == CODE_TYPE){
            if(current->assembly != NULL){
                fprintf(file, "%s\n", current->assembly);
            }
            else{
                printf("parse_address: x%x\n", current->address & 0xFFFF);
            printf("parse_contents: x%x\n", current->contents & 0xFFFF);
                parse_contents(file, current, memory);
            }
        }
        else if(type == DATA_TYPE){
            unsigned short contents = current->contents;
            fprintf(file, ".FILL 0x%x\n", contents >> 8);
            fprintf(file, ".FILL 0x%x\n", contents & 0xFF);
        }
    }
    // not consective address but same type
    else{
        process_new_addr(file, type, current, memory);
    }
}


const int BR_CODE = 0;
const int CMP_CODE = 2;
const int JSR_CODE = 4;
const int LOGIC_CODE = 5;
const int LDR_CODE = 6;
const int STR_CODE = 7;
const int RTI_CODE = 8;
const int CONST_CODE = 9;
const int SHIFT_CODE = 10;
const int JMP_CODE = 12;
const int HICONST_CODE = 13;
const int TRAP_CODE = 15;

int parse_contents(FILE* file, row_of_memory* current, row_of_memory** memory){
    unsigned short contents = current->contents;

    // get the opcode
    int opcode = (contents >> 12) & 0xF;
    switch(opcode){
        case BR_CODE:
            parse_br(file, current, memory);
            break;
        case CMP_CODE:
            parse_cmp(file, contents);
            break;
        case JSR_CODE:
            parse_jsr(file, current, memory);
            break;
        case LOGIC_CODE:
            parse_logic(file, contents);
            break;
        case LDR_CODE:
            parse_ldr(file, contents);
            break;
        case STR_CODE:
            parse_str(file, contents);
            break;
        case RTI_CODE:
            parse_rti(file, contents);
            break;
        case CONST_CODE:
            parse_const(file, contents);
            break;
        case SHIFT_CODE:
            parse_shift(file, contents);
            break;
        case JMP_CODE:
            parse_jmp(file, current, memory);
            break;
        case HICONST_CODE:
            parse_hiconst(file, contents);
            break;
        case TRAP_CODE:
            parse_trap(file, contents);
            break;
        default:
            perror("Invalid opcode");
            break;
    }
    return 0;
}




/*
 * delete entire linked list
 */
int delete_list    (row_of_memory** head )
{
	/* delete entire list node by node */
	/* set the list head pointer to NULL upon deletion */

    // if the linked list is empty, return

    row_of_memory* current = *head;
    row_of_memory* next;
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

// row_of_memory* create_node(short unsigned int address, short unsigned int contents){
//     // allocate memory for the new node
//     row_of_memory* node = (row_of_memory*) malloc(sizeof(row_of_memory));
//     // if malloc failed, return NULL
//     if(node == NULL){
//         perror("malloc() failed");
//         return NULL;
//     }
//     // initialize the new node
//     node->address = address;
//     node->contents = contents;
//     node->label = NULL;
//     node->assembly = NULL;
//     node->next = NULL;
//     // return the new node
//     return node;
// }


const int NOP = 0b000;
const int BRn = 0b100;
const int BRnz = 0b110;
const int BRnp = 0b101;
const int BRz = 0b010;
const int BRzp = 0b011;
const int BRp = 0b001;
const int BRnzp = 0b111;

int parse_br(FILE* file, row_of_memory* current, row_of_memory** memory){
    printf("parse_br\n");



    unsigned short contents = current->contents;
    unsigned short address = current->address;
        printf("br addr is %x\n", address);

    // get the opcode
    int opcode = (contents >> 9) & 0b111;

    if(opcode == NOP){
            fprintf(file, "NOP\n");
            return 0;
    }

    printf("opcode is %x\n", opcode);
 int imm = contents & 0x1FF;
    printf("imm is %x\n", imm);
     printf("imm is %d\n", imm);
    int mask = 0x100;
    printf("mask is %x\n", mask);
    printf("imm & mask is %x\n", imm & mask);
    // sign extension
    if((imm & mask) != 0){
        imm = imm | 0b11111111111111111111111000000000;
        imm = imm & 0x1FF;
        //pirntf("negative");
        printf("imm is %x\n", imm);
        printf("imm is %d\n", imm);
    }
    address += imm;
    address ++;
        printf("addr before mask is %x\n", address );
    address = address & 0xFF;
    printf("addr is %x\n", address );
    row_of_memory* node = search_address(*memory, address);
    printf("searching\n");
    if(node == NULL){
        printf("node is null\n");
    }
   else{

    printf("node's address is %04x  \n", node->address);
   }
    //int address = node->address;
    char* label = node->label;

    printf("label is %s\n", label);

    switch(opcode){
        case BRn:
            fprintf(file, "BRn %s\n", label);
            break;
        case BRnz:
            fprintf(file, "BRnz %s\n", label);
            break;
        case BRnp:
            fprintf(file, "BRnp %s\n", label);
            break;
        case BRz:
            fprintf(file, "BRz %s\n", label);
            break;
        case BRzp:
            fprintf(file, "BRzp %s\n", label);
            break;
        case BRp:
            fprintf(file, "BRp %s\n", label);
            break;
        case BRnzp:
            fprintf(file, "BRnzp %s\n", label);
            break;
        default:
            perror("Invalid opcode");
            break;
    }
}

const int CMP = 0b00;
const int CMPu = 0b01;
const int CMPi = 0b10;
const int CMPiu = 0b11;

int parse_cmp(FILE* file, int contents){
    // get the opcode
    int opcode = (contents >> 7) & 0b11;
    int rs = (contents >> 9) & 0b111;
    printf("rs is %d\n", rs);
    int rt = contents & 0b111;
    int imm = contents & 0b1111111;
    int uimm = contents & 0b1111111;
    int mask = 0b1000000;
    // sign extension
    if((imm & mask) != 0){
        imm = imm | 0b111111111111111111111111110000000;
        printf("cmp negative");
    }
    printf("imm is %d\n", imm);
      printf("imm is %u\n", imm);
    switch(opcode){
        case CMP:
            fprintf(file, "CMP R%d, R%d\n", rs, rt);
            break;
        case CMPu:
            fprintf(file, "CMPU R%d, R%d\n", rs, rt);
            break;
        case CMPi:
            fprintf(file, "CMPI R%d, #%d\n", rs, imm);
            break;
        case CMPiu:
            fprintf(file, "CMPIU R%d, x%x\n",rs, uimm);
            break;
        default:
            perror("Invalid opcode");
            break;
    }
}

int parse_jsr(FILE* file, row_of_memory* current, row_of_memory** memory){
    printf("parsing jsr\n");
    unsigned short contents = current->contents;
    unsigned short address = current->address;
    row_of_memory* node;

    // get the opcode
    int opcode = (contents >> 11) & 0b1;
    int rs = (contents >> 6) & 0b111;
    int imm = contents & 0b11111111111;
    int mask = 0b10000000000;
    // sign extension
    if((imm & mask) != 0){
        imm = imm | 0b11111111111111111111100000000000;
    }
    printf("addr is %x\n", address );


    switch(opcode){
        case 0:
            fprintf(file, "JSRR R%d\n", rs);
            break;
        case 1:
            node = search_address(*memory, address);
            printf("searching\n");
            if(node == NULL){
                printf("node is null\n");
            }
            else{
                printf("node's address is %04x  \n", node->address);
            }
    //int address = node->address;
            char* label = node->label;



            fprintf(file, "JSR %s\n",label);
            break;
        default:
            perror("Invalid opcode");
            break;
    }
}

const int AND = 0b000;
const int NOT = 0b001;
const int OR = 0b010;
const int XOR = 0b011;

int parse_logic(FILE* file, int contents){
    // get the opcode
    int opcode = (contents >> 3) & 0b111;
    int rd = (contents >> 9) & 0b111;
    int rs = (contents >> 6) & 0b111;
    int rt = contents & 0b111;
    int imm = contents & 0b11111;
    int mask = 0b10000;
    // sign extension
    if(imm & mask != 0){
        imm = imm | 0b11111111111111111111111111100000;
    }
    switch(opcode){
        case AND:
            fprintf(file, "AND R%d, R%d, R%d\n", rd, rs, rt);
            break;
        case NOT:
            fprintf(file, "NOT R%d, R%d\n", rd, rs);
            break;
        case OR:
            fprintf(file, "OR R%d, R%d, R%d\n", rd, rs, rt);
            break;
        case XOR:
            fprintf(file, "XOR R%d, R%d, R%d\n", rd, rs, rt);
            break;
        case 0b100:
        case 0b101:
        case 0b110:
        case 0b111:
            fprintf(file, "AND R%d, R%d, #%d\n", rd, rs, imm);
            break;
        default:
            perror("Invalid opcode");
            break;
    }
}

int parse_ldr(FILE* file, int contents){
    int rd = (contents >> 9) & 0b111;
    int rs = (contents >> 6) & 0b111;
    int imm = contents & 0b111111;
    int mask = 0b100000;
    // sign extension
    if((imm & mask) != 0){
        imm = imm | 0b11111111111111111111111111000000;
    }
    fprintf(file, "LDR R%d, R%d, #%d\n", rd, rs, imm);
}

int parse_str(FILE* file, int contents){
    int rd = (contents >> 9) & 0b111;
    int rs = (contents >> 6) & 0b111;
    int imm = contents & 0b111111;
    int mask = 0b100000;
    // sign extension
    if((imm & mask) != 0){
        imm = imm | 0b11111111111111111111111111000000;
    }
    fprintf(file, "STR R%d, R%d, #%d\n", rd, rs, imm);
}

int parse_rti(FILE* file, int contents){
    fprintf(file, "RTI\n");
}

int parse_const(FILE* file, int contents){
    int rd = (contents >> 9) & 0b111;
    int imm = contents & 0b111111111;
    int mask = 0b100000000;
    // sign extension
    if((imm & mask) != 0){
        imm = imm | 0b111111111111111111111111000000000;
    }
    fprintf(file, "CONST R%d, #%d\n", rd, imm);
}

const int SLL = 0b00;
const int SRA = 0b01;
const int SRL = 0b10;
const int MOD = 0b11;

int parse_shift(FILE* file, int contents){
    int opcode = (contents >> 4) & 0b11;
    int rd = (contents >> 9) & 0b111;
    int rs = (contents >> 6) & 0b111;
    int rt = contents & 0b111;
    int imm = contents & 0b1111;
    //int mask = 0b1000;
    // sign extension
    // if((imm & mask) != 0){
    //     imm = imm | 0b11111111111111111111111111110000;
    // }
    printf("shift imm is %d\n", imm);
        printf("shift imm is %u\n", imm);
    switch(opcode){
        case SLL:
            fprintf(file, "SLL R%d, R%d, #%u\n", rd, rs, imm);
            break;
        case SRA:
            fprintf(file, "SRA R%d, R%d, #%u\n", rd, rs, imm);
            break;
        case SRL:
            fprintf(file, "SRL R%d, R%d, #%u\n", rd, rs, imm);
            break;
        case MOD:
            fprintf(file, "MOD R%d, R%d, R%d\n", rd, rs, rt);
            break;
        default:
            perror("Invalid opcode");
            break;
    }
}

int parse_jmp(FILE* file, row_of_memory* current, row_of_memory** memory){
    printf("parsing jmp\n");
    unsigned short contents = current->contents;
    unsigned short address = current->address;


    int opcode = (contents >> 11) & 0b1;
    int rs = (contents >> 6) & 0b111;
    int rt = contents & 0b111;
    int imm = contents & 0b11111111111;
    int mask = 0b100000000;

    row_of_memory* node;
    // sign extension
    if((imm & mask) != 0){
        imm = imm | 0b111111111111111111111100000000000;
    }

        if((imm & mask) != 0){
        imm = imm | 0b11111111111111111111111000000000;
        imm = imm & 0x1FF;
        //pirntf("negative");
        printf("imm is %x\n", imm);
        printf("imm is %d\n", imm);
    }
    address += imm;
    address ++;

            printf("addr before mask is %x\n", address );
    address = address & 0xFF;
    printf("addr is %x\n", address );

    switch(opcode){
        case 0:
            fprintf(file, "JMPR R%d\n", rs);
            break;
        case 1:
            node = search_address(*memory, address);
            printf("searching\n");
            if(node == NULL){
            printf("node is null\n");
            }
            else{

            printf("node's address is %04x  \n", node->address);
            }
    //int address = node->address;
            char* label = node->label;

            fprintf(file, "JMP %s\n", label);
            break;
        default:
            perror("Invalid opcode");
            break;
    }
}

int parse_hiconst(FILE* file, int contents){
    int rd = (contents >> 9) & 0b111;
    int uimm = contents & 0b11111111;
    fprintf(file, "HICONST R%d, #%u\n", rd, uimm);
}

int parse_trap(FILE* file, int contents){
    int uimm = contents & 0b11111111;
    fprintf(file, "TRAP x%x\n", uimm);
}
