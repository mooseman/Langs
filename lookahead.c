

/* lookahead.c */ 
/* Experimenting with lookahead. */ 
/* This code is released to the public domain. */ 
/* "Share and enjoy......"  :)     */ 


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


void advance(FILE *fp) { 
   int i=0;  
   int curr_char = fgetc(fp);	
   int next_char = fgetc(fp);   /* Lookahead 1 character. */ 
   int next_char2 = fgetc(fp); 	/* Lookahead 2 characters. */ 		   
   
while ( (next_char2 != '\0' && i<30) ) {   
   printf("%c %c %c\n", curr_char, next_char, next_char2); 	
   curr_char = next_char;
   next_char = next_char2;
   next_char2 = fgetc(fp);   
   i++;
}
exit(0); 
}


          
int main(int argc, char **argv) { 

FILE *myfile; 

myfile = fopen(argv[1], "r"); 

if (myfile == NULL) 
   return -1;  
   
advance(myfile);   

fclose(myfile);

return 0; 

} 






