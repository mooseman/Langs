

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



/* Forward declarations. */ 
void parse(char token[],  char *toktype); 


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



void lex(FILE *fp) {    
   
   int i=0;
   char *toktype=0;	
   char token[20];
   
   int curr_char = fgetc(fp);	
        	   
  while (curr_char != '\0') {         
	
  if ( isspace(curr_char) ) { 
        curr_char = fgetc(fp);
       }
            	
  else if (isalpha(curr_char) || curr_char=='_')  { 	 
     while ( (isalnum(curr_char) || curr_char=='_') ) { 		  
		    token[i] = curr_char;
		    curr_char = fgetc(fp);	
		    i++;	  
		}   	
		
    if (search(kw_strings, NUMBER_OF_KEYWORDS, token) == 1 )
           toktype = "Keyword";
    else
           toktype = "Identifier";  		
				
   token[i] = '\0' ;   
   
   parse(token, toktype);   							         
   memset(&token[0], 0, sizeof(token));  // Clear token          
   }  // Keyword or identifier 

      
 else if ( curr_char == '"' ) { 
	   token[i] = curr_char;
	   curr_char = fgetc(fp);  	 
	   i++;	 
	    while ( ( curr_char != '"') && curr_char != '\0' ) {	   	    
			token[i] = curr_char;
		    curr_char = fgetc(fp); 		    
		    i++;	 
          } 
     token[i] = curr_char;  // Append the last quote. 
     token[i+1] = '\0' ;    // Append null char. 
     curr_char = fgetc(fp);	 // Move on from last quote. 
     toktype = "String" ;  
      
     parse(token, toktype); 
     memset(&token[0], 0, sizeof(token));  // Clear token          
   }  // String 	         
	         
	         
   else if ( isdigit(curr_char) )  { 
	  	while (isdigit(curr_char) && curr_char != '\0') { 	
 			token[i] = curr_char; 			   		    
		    curr_char = fgetc(fp); 		    		    
		    i++;	 
        }    
   toktype = "Number" ;   
   
   parse(token, toktype); 
   memset(&token[0], 0, sizeof(token));  // Clear token          
   }  // Number   	   	         
  	         
  	         
   else if ( ispunct(curr_char) && curr_char != '_' 
        && curr_char != '"' ) { 
		     token[i] = curr_char;
		     curr_char = fgetc(fp); 		
		     i++;	 	
      while (ispunct(curr_char) && curr_char != '\0' ) {   
		       token[i] = curr_char;      				  
		       curr_char = fgetc(fp);		   
		       i++;	   	    	 	      
	        }		 
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




