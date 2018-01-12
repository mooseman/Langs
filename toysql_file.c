

/* toysql_file.c  */ 
/* A lexer for a very small subset of SQL.  */
/* This version reads the code from a file. */ 

/* This code is released to the public domain. */ 
/* "Share and enjoy......"  :)     */ 


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define NUMBER_OF_KEYWORDS 9


// FILE *myfile=0;


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



void lex(FILE *myfile) { 
   char token[20];	
   char *toktype;
   int i=0;		
   int c;  	
	
   while ( ( c=fgetc(myfile) ) != '\0' )  {  	
	
   if ( isspace(c) )
      //c = fgetc(myfile); 	
        continue;  
	
   else if (isalpha(c) || c=='_')	{
	  while ( (isalnum(c) || c=='_') && c != '\0' && i<20 ) { 	   
		 token[i] = c;
		 i++;
		 c = fgetc(myfile);
      } 
   
   token[i] = '\0' ;  
   
      if (search(kw_strings, NUMBER_OF_KEYWORDS, token) == 1 )
           toktype = "Keyword";
      else
           toktype = "Identifier";  
           
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));  // Clear token                                         
   }  // Keyword or identifier 

      
   else if ( c == '"' ) { 
	  while ( ( c != '"') && c != '\0' && i<20) { 		
		  token[i] = c;
		  i++;
		  c = fgetc(myfile);
      } 
       
   /* Add the end double-quote. */    
   token[i] = '"' ; 
   token[i+1] = '\0' ; 
   //c = fgetc(myfile); 
   //afile++;  // Move on from quote.                
   toktype = "String" ; 
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));  // Clear token                  
   }  // String 	         
	         
	         
   else if ( isdigit(c) )  { 
	  	while (isdigit(c) && c != '\0' && i<20) { 		
		    token[i] = c;
		    i++;
		    c = fgetc(myfile); 
        } 
   token[i] = '\0' ;     
   toktype = "Number" ;   
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));  // Clear token                  
   }  // Number   	   	         
  	         
  	         
   else if ( ispunct(c) )  { 	
	    while (ispunct(c) && c != '\0' && i<20) { 		
		    token[i] = c;
		    i++;
		    c = fgetc(myfile); 
   } 
   token[i] = '\0' ;  
   toktype = "Punct" ;  
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));  // Clear token                 
   }  // Punct 
   
   else {	      
	       exit(0); 
	    }   
      
   memset(&token[0], 0, sizeof(token));  // Clear token   
   i = 0;  // Reset i.                  
   }  // while c != '\0'   
    
   exit(0) ;  
    
}  // lex()  



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






     
     
       
 




