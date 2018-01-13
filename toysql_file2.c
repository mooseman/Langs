

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

FILE *myfile=NULL;

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
void lex(FILE *fp) ; 

void parse(char token[],  char *toktype); 



void lex_kwident() { 
   char token[20];	
   char *toktype;
   int i=0;	
   int c; 	
	
	while ( (c=fgetc(myfile) != '\0') && i<20) { 		
		token[i] = c;
		i++;
		c=fgetc(myfile);
   } 
   
   token[i] = '\0' ;  
   
    if (search(kw_strings, NUMBER_OF_KEYWORDS, token) == 1 )
          toktype = "Keyword";
    else
          toktype = "Identifier" ;
      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));        		
} 


void lex_string() { 
   char token[20];	
   char *toktype;
   token[0] = '"' ;  // Put the quote in the array.   
   int i=1;  // 1 Because token[0] has been filled.   
   int c;   
	
	while ( (c=fgetc(myfile) != '"') && c != '\0' && i<20) { 		
		token[i] = c;
		i++;
		c=fgetc(myfile);
   } 
       
   /* Add the end double-quote. */    
   token[i] = '"' ; 
   token[i+1] = '\0' ;  
   //str++;  // Move on from quote.    
       
   toktype = "String" ;      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));         
} 



void lex_number() { 
   char token[20];	
   char *toktype;
   int i=0;	
   int c; 
	
	while ( isdigit(c=fgetc(myfile)) && c != '\0' && i<20) { 		
		token[i] = c;
		i++;
		c=fgetc(myfile);
   } 
   token[i] = '\0' ;  
   
   toktype = "Number" ;      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));          		
} 



void lex_punct() { 
   char token[20];	
   char *toktype;
   int i=0;	
   int c; 
	
	while (ispunct(c=fgetc(myfile)) && c != '\0' && i<20) { 		
		token[i] = c;
		i++;
		c=fgetc(myfile);
   } 
   token[i] = '\0' ;  
   toktype = "Punct" ;
      
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));        	
} 


void lex_space() {  
   char token[2] = " ";		 
   char *toktype = "Space";
   int c;    
       
  while ( isspace(c=fgetc(myfile)) && c != '\0') { 	  	 	
		c=fgetc(myfile);
   } 
    
  toktype = "Space" ; 
  parse(token, toktype); 
  memset(&token[0], 0, sizeof(token));   
} 	



void lex(FILE *myfile) { 	
		   		
   int c=fgetc(myfile);		
				        
   if (isalpha(c) || c == '_')             lex_kwident() ; 
   else if ((c == '"'))                    lex_string(); 
   else if (isspace(c))                    lex_space(); 
   else if (isdigit(c))                    lex_number(); 
   else if (ispunct(c) && c != '_')        lex_punct();      	
   else exit(0) ;             				         
  
} // lex 
 
 
/* Not a parser (yet) - just prints the tokens. */ 
void parse(char token[],  char *toktype) { 
  printf("Token: %s Tokentype: %s\n", token, toktype); 
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






     
     
       
 




