

/* toysql2.c  */ 
/* A lexer for a very small subset of SQL.  */
/* This lexer operates on a string. I hope to do a */
/* version that operates on a file very soon. */  

/* This code is released to the public domain. */ 
/* "Share and enjoy......"  :)     */ 


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define NUMBER_OF_KEYWORDS 9


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


/* Token types. */ 
typedef enum { KEYWORD, IDENTIFIER, 
	OPERATOR, STRING, NUMBER, _EOF_ } 
 token_type ; 	


/* Tokens.  */ 
typedef struct {
    token_type type;
    union {
        char *string_value;      
        int int_value;
    } value;
} token;



token *lex_kwident() { 
   
  token *t; 	
  
  return t;     
} 


token *lex_string() { 
   
  token *t; 	
  
  return t;     		 
} 



token *lex_number() { 
   
  token *t; 	
  
  return t;     	
} 


token *lex_punct() { 
   
  token *t; 	
  
  return t;     	
} 


token *lex_space() {  
   
  token *t; 	
  
  return t;     	
} 	



token *lex() { 	
   
  /*  ???  Function to get characters from string/file here */    
   
  /*  Loop through characters.  How do I make sure that */  
   
   
  token *t; 	
  
  return t;     	 	     				         
} 


/* Not a parser (yet) - just prints the tokens. */ 
void parse() { 
  token t;
  	
  printf("Tokentype: %d\n", t.type); 
}    	


          
int main() { 

char *mystr1 = "select mycol1, mycol2 from mytable where mycol1 < 10;" ; 

parse(); 

char *mystr2 = "select mycol1, mycol2 from mytable where city = \"Sydney\";" ; 

parse();


return 0; 

} 






     
     
       
 




