

// This grammar is released to the public domain. 
// "Share and enjoy...."  :)  


grammar toySQL;
import toySQL_Tokens;  


stmt 
    : SELECT what_to_select FROM table_stmt (where_stmt) Semi  
; 


SELECT : 'select' ; 

FROM   : 'from' ;  

AS  :  'as'  ;  

what_to_select
    :  Star  
    |  (other_stmt)*
; 

Star 
    :  '*' 
;

other_stmt 
    :  field_stmt
    |  value_stmt
    |  other_stmt Comma ( field_stmt | value_stmt )  
;  

field_stmt 
    :  field 
    |  field AS alias 
; 

value_stmt 
    :  value AS alias   
;

field
    :  Identifier
;

alias 
    :  Identifier
;  

value  
    :  Number 
    |  String_Literal 
;


table_stmt 
    :  Identifier
; 


where_stmt  
    :  WHERE compare_stmt 
; 

compare_stmt 
    :  ( field | value ) RelOp ( field | value ) 
    |  compare_stmt OR compare_stmt 
    |  compare_stmt AND compare_stmt 
; 



