/************************************************************************/
/* File Name : lc4_disassembler.c 										*/
/* Purpose   : This file implements the reverse assembler 				*/
/*             for LC4 assembly.  It will be called by main()			*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h> 
#include "lc4_memory.h"
#include <string.h>

// for clang
#include <stdlib.h>



int reverse_assemble (row_of_memory* memory){
  /* binary constants should be proceeded by a 0b as in 0b011 for decimal 3 */ 
	/* make sure head isn't NULL */
    if(memory == NULL){
		  printf("head of the linkedList is missing.");
		  return -1;
	  } 
    /* traverse linked list, print contents of the node that has ops code starting w/ 1 */
    row_of_memory* current = search_opcode(memory, 0x0001);


    //printf("TST current->address is %04x \n",current->address);
      
    while (current != NULL) {
        printf("current->address is %04x \n",current->address);
        //printf("current->next is %04x \n",current->next->address); 
        //printf("current->next > 0x1FFF: %d\n", current->next->address > 0x1FFF); 
        //also check if the address is within code region



        if(current->address > (unsigned short int) 0x1FFF){
            current = search_opcode(current, 0x01);
            break; 
        } 
        
        // allocate memory for the assembly
        current->assembly = (char*) malloc(16+ 1); //15 is the length of ops - max when add with -IMM
        // Check if memory allocation was successful
        if (current->assembly == NULL) { 
            fprintf(stderr, "Memory allocation failed.\n");
            return -1;
        }
        printf("AFT current->address is %04x \n",current->address);
        //check if IMM for add
        if (current->contents  & 0x20 ){
            printf("IMM current->address is %04x \n",current->address);
            getAdd(current->contents, current);  
        } 
        else{ // not IMM   
            printf("NOT IMM current->address is %04x \n",current->address);
          
            int op = (current->contents >> 3) & 7 ;
          
            switch(op) { 
              case 0:
                  getAdd(current->contents, current); 
                  break; 
              case 1 :
                  getMulSubDiv(current->contents, current,1); 
                  break; 

              case 2:
                  getMulSubDiv(current->contents, current,2);   
                  break;  

              case 3:
                  getMulSubDiv(current->contents, current,3); 
                  break;   

              default:
                  // parse error
                  printf("Error: Invalid opcode.\n");
                  delete_list(memory);
                  break;  
            }     
        }      
            
      printf("current->address is %04x \n",current->address); 
      printf("current->contents is %04x \n",current->contents); 
      printf("current->assembly is %s \n",current->assembly);  
        
        //update current node
        current = search_opcode(current,0x0001); 
        //printf("UPD current->address is %04x \n",current->address);
      } 
	return 0 ;
}
 

int getAdd (unsigned short int instruction,row_of_memory* node )
{     
  printf("instruction is %04x \n",instruction);
    char result[17] = "ADD R"; 
 
    char strnum1[2];
        sprintf(strnum1, "%d", (instruction >> 9) & 7);
    
    strcat(result,  strnum1); 
    strcat(result, ", R"); 

    char strnum2[2];
        sprintf(strnum2, "%d", (instruction >> 6) & 7);
     
    strcat(result,  strnum2); 

    char strnum3[20];
    printf("instruction & 0x20 is %d \n",instruction & 0x20); 
    if ((instruction & 0x20) != 0) //check for IMM
    {   
      printf("IMM instruction is %04x \n",instruction);
        //strcat(result, instruction & 0x1F);
        strcat(result, ", ");

          printf("IMM instruction is %04x \n",instruction);
        //check for negative IMM
        // if(instruction & 0b10000){
        //      //strcat(result, "-");
        //      sprintf(strnum3, "%d", (instruction & 0xF) - 16);
        // strcat(result,  strnum3);
        // } else{
        //     sprintf(strnum3, "%d", instruction & 0xF);
        //     strcat(result,  strnum3);
        // }

        short int imm = instruction & 0b11111;

                      printf("IMM is %d \n",imm);
                      printf("IMM  & is %d \n",imm & 0b10000);
        if((imm & 0b10000) != 0){
          printf("IMM is %d \n",imm);
            imm = imm | (short int) 0b1111111111100000;
            printf("IMM is %d \n",imm);
        }
        
          printf("IMM is %d \n",imm);
      
        sprintf(strnum3, "%d", imm);
        strcat(result,  strnum3);
        printf("end\n");
    } else {
      printf("test\n");
      strcat(result, ", R");
      sprintf(strnum3, "%d", instruction & 7);
      strcat(result,  strnum3);
    }
    printf("result is %s \n",result); 
    //result[15] = '\0';   
    // allocate memory for the assembly
    node->assembly = (char*)  malloc( strlen(result) ); //16 is the length of ops - max when add with -IMM
    // Check if memory allocation was successful
    if (node->assembly == NULL) { 
        fprintf(stderr, "Memory allocation failed.\n");
        return -1;
        }
    
    //pass value to node assembly
    strcpy(node->assembly, result); 
    return 0;
}

int getMulSubDiv(unsigned short int instruction, row_of_memory* node, int OPS)
{   
    char result[16]; 
    if (OPS == 1){  
      strcpy(result,  "MUL R"  ); 
    } 
    else if (OPS == 2){ 
      strcpy(result,"SUB R" ); 
    } 
    else if (OPS == 3){ 
      strcpy(result,"DIV R" );
    } 
 
    char strnum1[2];
    sprintf(strnum1, "%d", (instruction >> 9) & 7);
      
    strcat(result,  strnum1); 
    strcat(result, ", R");  

    char strnum2[2];
    sprintf(strnum2, "%d", (instruction >> 6) & 7);
     
    strcat(result,  strnum2);
 
    strcat(result, ", R"); 

    char strnum3[2];
    sprintf(strnum3, "%d", instruction   & 7);
    strcat(result,  strnum3);
    // allocate memory for the assembly
    node->assembly =  (char*) malloc( strlen(result) ); //16 is the length of ops - max when add with -IMM
    // Check if memory allocation was successful
    if (node->assembly == NULL) { 
        fprintf(stderr, "Memory allocation failed.\n");
        return -1;
        }
 
    //pass value to node assembly
    strcpy(node->assembly, result); 
    
    return 0;
}

 

 