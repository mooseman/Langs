

/* ast4.c  */
/* Trying out ASTs. */
 
/* This code is released to the public domain. */
/* "Share and enjoy......"  :)     */
 
 
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
 

#define MAXLEN 10    // Maximum length of a token. 



/* Enum of token types. */
typedef enum { INTEGER, PLUS, MINUS, 
	 MUL, DIV, LPAREN, RPAREN, NONE	} tokentype ; 
      

/* Define a "type" for values. */ 
typedef union { 
   char string_value[MAXLEN]; 
   int int_value; 
} value; 



/* A token. */
typedef struct { 
   tokentype toktype; 
   value tokval; 
 } token ;      


token maketok(tokentype ttype, value tval) { 
	token rettok = malloc(sizeof(token));
    rettok->toktype = ttype;
    rettok->tokval = tval; 
    return *rettok; 
}     


/* A node. */ 
typedef struct { 
	 token mytok;      
} node;     	


/*  A tree.  */  
/*  This needs to be able to have NODES as left or right. */ 
typedef struct { 	
	node root;
	node left, right; 	     
} tree; 


/*  Create a node "type".  */ 
typedef enum { NODE, TREE } nodetype ;


/* Make a tree. */ 
tree make_tree(nodetype *left, nodetype *root, 
      nodetype *right) {
   tree *mytree = malloc(sizeof(binop_node));
     
   mybinop->left = left; 
   mybinop->root = root;
   mybinop->right = right;                                          
   return *mytree;
}


tree add_node(tree *mytree, node *mynode) { 
	
	
	
	
} 	



/*  Print a tree. */ 
/*  ( This could maybe be a "visitor pattern". Visit each */ 
/*  node and either print it or iteratively evaluate */ 
/*  the tree. )  */  
void print_tree(tree *mytree)  { 
   if (mytree != NULL) { 	


}


  


int interpret(FILE *fp) { 
   
   int result=0;   
   int c;
   token *currtok = malloc(sizeof(token));  	     
   
   while ( ( c = fgetc(fp) != EOF ) )  {
      *currtok = get_token(fp); 
   
   if ( expect(currtok->toktype, INTEGER) == 0 ) {        
        result = currtok->value.int_value;           
   }            
   
   else if ( expect(currtok->toktype, LPAREN) == 0 ) {   	    
   	    *currtok = get_token(fp); 
	    expect(currtok->toktype, RPAREN);	  
   }   
 
   else if ( expect(currtok->toktype, MUL) == 0 ) {                                  	    	    
	    result *= currtok->value.int_value; 	   
   }     
	            
   else if ( expect(currtok->toktype, DIV) == 0 )  { 	            
	    if ( currtok->value.int_value != 0 ) { 
	               result /= currtok->value.int_value; 	              
	    }  else printf("Error! Division by zero attempted.\n");       	               
   }   	      
             	           			     
   else if ( expect(currtok->toktype, PLUS) ) {        
        result += currtok->value.int_value;       
   }                
     
   else if ( expect(currtok->toktype, MINUS) )  {        
        result -= currtok->value.int_value;        
   } 	     
    
   *currtok = get_token(fp);        
   
  } // while 
   
   return result;            
   
}  // Interpret.  
    
    
        
           
int main(int argc, char **argv) { 
 
FILE *myfile; 
 
myfile = fopen(argv[1], "r"); 
 
if (myfile == NULL) 
   return -1;  
    
int myres = interpret(myfile); 

printf("%d\n", myres);  
 
fclose(myfile);
 
return 0; 
 
}






