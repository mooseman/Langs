

/* toysql_parser.c  */ 
/* A lexer and parser for a very small subset of SQL.  */
/* This version reads the code from a file. */ 

/* This code is released to the public domain. */ 
/* "Share and enjoy......"  :)     */ 


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


#define NUMBER_OF_KEYWORDS 9
#define MAXLEN 80    // Maximum length of a token. 


/* Array of our keywords in string form. */ 
char *kw_strings[] = { 
   "select", "from", "where", "and", "or", "not", "in", "is", "null" 
    } ; 
   
    
/*  Search function to search the array of keywords. */ 
int search(char *arr[], int dim, char *str) { 
	
	int i;  	
	int found_match;
	
	for (i=0; i<dim; i++) { 
		if ( !strcmp(arr[i] , str ) )  {   
	        found_match = 1;		
	        break; 
    }   else found_match = 0;    
 }  /* For */     

    return found_match; 
}  /* search */ 


/* Enum of token types. */ 
typedef enum { KEYWORD, IDENT, INTEGER, STRING, 
	PUNCT, _EOF_ } tokentype ; 
	 

/* A token. */
typedef struct { 
   tokentype toktype;
   union { 
	       char string_value[MAXLEN]; 
	       int int_value; 
	     }  value; 
 } token ; 	       	



/* Forward declaration. */ 
void parse(token *mytok); 
char *ptr ;   // For strtol use.   


void lex(FILE *fp) {    
   
   token *mytok = malloc(sizeof(token)); 
   tokentype mytype;
    
   int i=0;
   
   char myresult[MAXLEN];
   
   int curr_char = fgetc(fp);	
        	   
  while (curr_char != '\0') {         
	
  if ( isspace(curr_char) ) { 
        curr_char = fgetc(fp);
       }
            	
  else if (isalpha(curr_char) || curr_char=='_')  { 	 
     while ( (isalnum(curr_char) || curr_char=='_') 
         && i<MAXLEN ) { 		  
		    myresult[i] = curr_char;
		    curr_char = fgetc(fp);	
		    i++;	  
		}   	
		
    if (search(kw_strings, NUMBER_OF_KEYWORDS, myresult) == 1 )
           mytype = KEYWORD; 
    else
           mytype = IDENT; 
				
   myresult[i] = '\0' ;   
   
   mytok->toktype = mytype;
   strcpy( mytok->value.string_value, myresult);
   
   parse(mytok);   							         
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token          
   }  // Keyword or identifier 

      
 else if ( curr_char == '"' ) { 
	   myresult[i] = curr_char;
	   curr_char = fgetc(fp);  	 
	   i++;	 
	    while ( ( curr_char != '"') && curr_char != '\0' 
	      && i<MAXLEN-1) {	   	    
			myresult[i] = curr_char;
		    curr_char = fgetc(fp); 		    
		    i++;	 
          } 
     myresult[i] = curr_char;  // Append the last quote. 
     myresult[i+1] = '\0' ;    // Append null char. 
     curr_char = fgetc(fp);	 // Move on from last quote.           
     mytype = STRING; 
     
     mytok->toktype = mytype; 
     strcpy( mytok->value.string_value, myresult);
           
     parse(mytok); 
     memset(&myresult[0], 0, sizeof(myresult));  // Clear token          
   }  // String 	         
	         
	         
   else if ( isdigit(curr_char) )  { 
	  	while (isdigit(curr_char) && curr_char != '\0' 
	  	   && i<MAXLEN) { 	
 			myresult[i] = curr_char; 			   		    
		    curr_char = fgetc(fp); 		    		    
		    i++;	 
        }  
   myresult[i] = '\0' ;    // Append null char.        
   mytype = INTEGER ;   
   mytok->toktype = mytype; 
   int j=strtol(myresult, &ptr, 10);
   mytok->value.int_value = j;
      
   parse(mytok); 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token          
   }  // Number   	   	         
  	         
  	         
   else if ( ispunct(curr_char) && curr_char != '_' 
        && curr_char != '"' ) { 
		     myresult[i] = curr_char;
		     curr_char = fgetc(fp); 		
		     i++;	 	
      while (ispunct(curr_char) && curr_char != '\0' 
        && i<MAXLEN) {   
		       myresult[i] = curr_char;      				  
		       curr_char = fgetc(fp);		   
		       i++;	   	    	 	      
	        }		 
   mytype = PUNCT;   
   
   mytok->toktype = mytype; 
   strcpy(mytok->value.string_value, myresult);
   
   parse(mytok); 
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token           
   }  // Punct 
   
   else {	      
	       exit(0); 
	    } 
	      	    
   memset(&myresult[0], 0, sizeof(myresult));  // Clear token   
   i = 0;  // Reset i.     	    	    
   }  // while c != '\0'   
    
   exit(0) ;  
    
}  // lex()  

   
   
/* Not a parser (yet) - just prints the tokens. */ 
void parse(token *mytok) { 
   	
  printf("%d  ", mytok->toktype); 
     
  if (mytok->value.string_value) 
       printf("%s\n", mytok->value.string_value);
  else if (mytok->value.int_value) 
       printf("%d\n", mytok->value.int_value);       
  
}    	
  
          
          
int main(int argc, char **argv) { 

FILE *myfile; 

myfile = fopen(argv[1], "r"); 

if (myfile == NULL) 
   return -1;  
   
lex(myfile);   

fclose(myfile);

return 0; 

} 




