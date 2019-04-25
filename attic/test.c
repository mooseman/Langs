

/* toysql.c  */ 
/* A lexer for a very small subset of SQL.  */
/* This lexer operates on a string. I hope to do a */
/* version that operates on a file very soon. */  

/* This code is released to the public domain. */ 
/* "Share and enjoy......"  :)     */ 


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> 

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



/* Forward declarations. */ 
void lex(char *str) ; 

void parse(char token[],  char *toktype); 


typedef struct Lexer { 
	char *toktype; 
	char token[] ;  
} Lexer; 


bool kw_ident_start(char c) { 
  return isalpha(c) || c == '_' ; 		
} 	


bool kw_ident_end(char c) { 
  return !(isalnum(c) || c == '_') ; 
}   	 

/* Same condition can be used for both start and end. */ 
/* For end, we just say "while not string". */  
bool string(char c) { 
  return c == '"' ;
}   	

/* Same condition for both start and end. */ 
/* For end, we just say "while number". */  
bool number(char c) { 
  return isdigit(c); 
} 
   	

bool punct(char c) { 
  return ispunct(c) && c != '_' && c != '"' ;
} 

  	
bool space(char c) { 
  return isspace(c) ; 
}   	  	 
  	

Lexer *init_lexer(Lexer mylexer) { 
   mylexer = malloc(sizeof(Lexer)); 
   return mylexer;
} 

    	
lexer *do_it(lexer mylexer, char *mystring) { 
   
   	   
  

} 


void lex(char *str) { 	
		
   if (isalpha(*str) || *str == '_')       lex_kwident(str) ; 
   else if ( (*str == '\"') )              lex_string(str);    
   else if (isdigit(*str))                 lex_number(str); 
   else if (ispunct(*str) && *str != '_')  lex_punct(str);  
   else if (isspace(*str))                 {str++; lex(str);}  
                     	 				         
} 


/* Not a parser (yet) - just prints the tokens. */ 
void parse(char token[],  char *toktype) { 
  printf("Token: %s Tokentype: %s\n", token, toktype); 
}    	


          
int main() { 

char *mystr1 = "select mycol1, mycol2 from mytable where mycol1 <= 10;" ; 
lex(mystr1); 

char *mystr2 = "select mycol1, mycol2 from mytable where city = \"Sydney\";" ; 
lex(mystr2); 

return 0; 

} 






     
     
       
 




