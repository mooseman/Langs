

/* ast2.c  */
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
      

/* A token. */
typedef struct { 
   tokentype toktype; 
   union { 
           char string_value[MAXLEN]; 
           int int_value; 
         }  value; 
 } token ;      


/* A num struct. */ 
//typedef struct { 
    	


/*
typedef struct { 
	enum { integer_exp, string_exp } exptype; 
	token  *op;
	token  *left, *right; 	     
} binop; 
*/

/*
binop make_binop(token *left, token *op, token *right) {
   binop *myop = malloc(sizeof(binop));
   
   if ( left->toktype == INTEGER && right->toktype == INTEGER ) 
          myop->exptype = integer_exp; 
   else if ( left->toktype == STRING && right->toktype == STRING )           
          myop->exptype = string_exp;       
          
          myop->op = op;
          myop->left = left;
          myop->right = right;
                                     
   return *myop;
}
*/



/* Need some lexing code here. */ 
token get_token(FILE *fp) {
   token *mytok = malloc(sizeof(token)); 
   tokentype mytype;     
   int i=0;    
   char myresult[MAXLEN];
      
   int curr_char = fgetc(fp);    
                
   while (curr_char != EOF) {   
	      
	   if ( isspace(curr_char) ) { 
        curr_char = fgetc(fp);
       }
	
       else if ( isdigit(curr_char) )  { 
          while (isdigit(curr_char) && curr_char != EOF
             && i<MAXLEN) {     
             myresult[i] = curr_char;                            
            curr_char = fgetc(fp);                         
            i++;     
      }  
      myresult[i] = '\0' ;    // Append null char.        
      mytype = INTEGER ;   
      mytok->toktype = mytype; 
      mytok->value.int_value = strtol(myresult, NULL, 10);
       
      return *mytok; 
      memset(&myresult[0], 0, sizeof(myresult));  // Clear token          
   }  // Number                       	
	
   else if (curr_char == '+')  { 		 			  
             myresult[i] = curr_char;
             curr_char = fgetc(fp);         
             i++;         
                
   mytype = PLUS;       
   mytok->toktype = mytype; 
   strcpy(mytok->value.string_value, myresult);
    
   return *mytok; 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token           
   }  // Plus 
   
   
   else if (curr_char == '-')  { 		 			  
             myresult[i] = curr_char;
             curr_char = fgetc(fp);         
             i++;         
                
   mytype = MINUS;       
   mytok->toktype = mytype; 
   strcpy(mytok->value.string_value, myresult);
    
   return *mytok; 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token           
   }  // Minus 
   
   
   else if (curr_char == '*')  { 		 			  
             myresult[i] = curr_char;
             curr_char = fgetc(fp);         
             i++;         
                
   mytype = MUL;       
   mytok->toktype = mytype; 
   strcpy(mytok->value.string_value, myresult);
    
   return *mytok; 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token           
   }  // Multiply 
   
      
   else if (curr_char == '/')  { 		 			  
             myresult[i] = curr_char;
             curr_char = fgetc(fp);         
             i++;         
                
   mytype = DIV;       
   mytok->toktype = mytype; 
   strcpy(mytok->value.string_value, myresult);
    
   return *mytok; 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token           
   }  // Divide  
    
    
   else if (curr_char == '(')  { 		 			  
             myresult[i] = curr_char;
             curr_char = fgetc(fp);         
             i++;         
                
   mytype = LPAREN;       
   mytok->toktype = mytype; 
   strcpy(mytok->value.string_value, myresult);
    
   return *mytok; 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token           
   }  // Left parenthesis  
    
    
  else if (curr_char == ')')  { 		 			  
             myresult[i] = curr_char;
             curr_char = fgetc(fp);         
             i++;         
                
   mytype = RPAREN;       
   mytok->toktype = mytype; 
   strcpy(mytok->value.string_value, myresult);
    
   return *mytok; 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token           
   }  // Right parenthesis    
    
    
   else {  
	       mytype = NONE; 
	       mytok->toktype = mytype; 
	       strcpy(mytok->value.string_value, "NONE");	               
           exit(0); 
        }  // Anything else.  
    
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token   
   i = 0;  // Reset i.                     
   }  // while c != '\0'   
     
   exit(0) ;  
     
}  // lex()  



int expect(tokentype exptype, tokentype acttype) {   
  int retval;    
     
  if ( exptype == acttype ) {      
      retval = 0;      
  }   else if ( exptype != acttype ) {     	     
      printf("Wrong token type - expected %d\n", exptype);
      retval = -1;
  }    
  return retval;  
}



void interpret(FILE *fp) { 
       
   token *currtok = malloc(sizeof(token));  
   *currtok = get_token(fp) ; 	     
   
   while ( ( currtok->toktype != NONE ) )  {
  
      if ( currtok->toktype == INTEGER ) { 
		  printf("%d ", currtok->value.int_value);
     } 		  
     else printf("%s ", currtok->value.string_value);		  
		  
     *currtok = get_token(fp); 		  
		           
  } // while 
         
}   // Interpret.  
    
    
        
           
int main(int argc, char **argv) { 
 
FILE *myfile; 
 
myfile = fopen(argv[1], "r"); 
 
if (myfile == NULL) 
   return -1;  
    
interpret(myfile); 
 
fclose(myfile);
 
return 0; 
 
}






