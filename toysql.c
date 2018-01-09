

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



void lex_kwident(char *str) { 
   char token[20];	
   char *toktype;
   int i=0;	
	
	while (isalnum(*str) && *str != '\0' && i<20) { 		
		token[i] = *str;
		i++;
		str++;
   } 
   
   token[i] = '\0' ;  
   
    if (search(kw_strings, NUMBER_OF_KEYWORDS, token) == 1 )
          toktype = "Keyword";
    else
          toktype = "Identifier" ;
      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));      
   lex(str);    		
} 



void lex_string(char *str) { 
   char token[20];	
   char *toktype;
   token[0] = '"' ;  // Put the quote in the array.   
   int i=1;  // 1 Because token[0] has been filled.   
   str++;  // Move on from the quote.  	
	
	while ( (*str != '\"') && *str != '\0' && i<20) { 		
		token[i] = *str;
		i++;
		str++;
   } 
       
   /* Add the end double-quote. */    
   token[i] = '"' ; 
   token[i+1] = '\0' ;  
   str++;  // Move on from quote.    
       
   toktype = "String" ;      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));      
   lex(str);    		
} 



void lex_number(char *str) { 
   char token[20];	
   char *toktype;
   int i=0;	
	
	while (isdigit(*str) && *str != '\0' && i<20) { 		
		token[i] = *str;
		i++;
		str++;
   } 
   token[i] = '\0' ;  
   
   toktype = "Number" ;
      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));      
   lex(str);    		
} 



void lex_punct(char *str) { 
   char token[20];	
   char *toktype;
   int i=0;	
	
	while (ispunct(*str) && *str != '\0' && i<20) { 		
		token[i] = *str;
		i++;
		str++;
   } 
   token[i] = '\0' ;  
   toktype = "Punct" ;
      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));      
   lex(str);    		
} 


void lex_space(char *str) {  
   char token[2] = " \0";		 
   char *toktype = "Space";
    
  while ( isspace(*str) && *str != '\0') { 	  	 	
		str++;
   } 
    
  toktype = "Space" ; 
  parse(token, toktype); 
  memset(&token[0], 0, sizeof(token)); 
  lex(str);    		 
} 	



void lex(char *str) { 	
		
   if (isalpha(*str) || *str == '_')       lex_kwident(str) ; 
   else if ( (*str == '\"') )              lex_string(str); 
   else if (isspace(*str))                 lex_space(str); 
   else if (isdigit(*str))                 lex_number(str); 
   else if (ispunct(*str) && *str != '_')  lex_punct(str);      	
   				         
} 


/* Not a parser (yet) - just prints the tokens. */ 
void parse(char token[],  char *toktype) { 
  printf("Token: %s Tokentype: %s\n", token, toktype); 
}    	


          
int main() { 

char *mystr1 = "select mycol1, mycol2 from mytable where mycol1 < 10;" ; 
lex(mystr1); 

char *mystr2 = "select mycol1, mycol2 from mytable where city = \"Sydney\";" ; 
lex(mystr2); 

return 0; 

} 






     
     
       
 




