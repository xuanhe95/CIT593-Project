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


int reverse_assemble (row_of_memory* memory){
  /* binary constants should be proceeded by a 0b as in 0b011 for decimal 3 */ 
	/* make sure head isn't NULL */
    if(memory == NULL){
		  printf("head of the linkedList is missing.");
		  return -1;
	  } 
    /* traverse linked list, print contents of the node that has ops code starting w/ 1 */
    row_of_memory* current = search_opcode(memory, 0x0001);
      
    while (current != NULL) { 
        //printf("current->next is %04x \n",current->next->address); 
        //printf("current->next > 0x1FFF: %d\n", current->next->address > 0x1FFF); 
        //also check if the address is within code region
        if(current->address > 0x1FFF){
            current = search_opcode(current, 0x0001);
            break; 
        } 
        // allocate memory for the assembly
        current->assembly = (char*) malloc(15+ 1); //15 is the length of ops - max when add with -IMM
        // Check if memory allocation was successful
        if (current->assembly == NULL) { 
            fprintf(stderr, "Memory allocation failed.\n");
            return -1;
        }
        //check if IMM for add
        if (current->contents  & 0x20 ){
            getAdd(current->contents, current);  
        } 
        else{ // not IMM   
          
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
      } 
	return 0 ;
}
 

int getAdd (unsigned short int instruction,row_of_memory* node )
{     
    char result[16] = "ADD R"; 
 
    char strnum1[2];
    // read first 3 bits
    sprintf(strnum1, "%d", (instruction >> 9) & 7);
    // add to result
    strcat(result,  strnum1); 
    strcat(result, ", R"); 

    char strnum2[2];
    // read next 3 bits
    sprintf(strnum2, "%d", (instruction >> 6) & 7);
     // add to result
    strcat(result,  strnum2); 

    char strnum3[2];
    //printf("instruction & 0x20 is %d \n",instruction & 0x20); 
    if (instruction & 0x20) //check for IMM
    {   
        // //strcat(result, instruction & 0x1F);
        // strcat(result, ", ");


        // int mask = 1;
        // for(int i = 0; i < sizeof(int); i++){
        //     mask = mask << 1;
        //     mask = mask | 1;
        // }

        // //printf("mask is %d \n",mask);

        // //printf("instruction & mask is %d \n",instruction & mask);

        // sprintf(strnum3, "%d", instruction & mask);
        // //printf("strnum3 is %s \n",strnum3);
        // strcat(result,  strnum3);


        // //check for negative IMM
        // if(instruction & 0x8){
        //      strcat(result, "-");
        // }
         
        // sprintf(strnum3, "%d", instruction & 0x1F);
         
        // strcat(result,  strnum3);
   
    } else {
        strcat(result, ", R");
        // read last 3 bits
        sprintf(strnum3, "%d", instruction & 7);
        strcat(result,  strnum3);
        //printf("result is %s \n",result);  

        // // add null terminator
        // result[12] = '\0';
    }
    // //printf("result is %s \n",result); 
    // result[15] = '\0';   
    
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
    sprintf(strnum3, "%d", (instruction >> 3) & 7);
    strcat(result,  strnum2);

    result[12] = '\0';   
 
    //pass value to node assembly
    strcpy(node->assembly, result); 
    
    return 0;
}

 