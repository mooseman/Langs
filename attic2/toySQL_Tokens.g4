

// This grammar is released to the public domain. 
// "Share and enjoy...."  :)  


lexer grammar toySQL_Tokens; // note "lexer grammar"

// keywords
SELECT : 'SELECT' |  'select' ;
FROM   : 'FROM'   |  'from'   ; 
AS     : 'AS'     |  'as'     ;   
WHERE  : 'WHERE'  |  'where'  ; 
AND    : 'AND'    |  'and'    ; 
OR     : 'OR'     |  'or'     ;  

Semi 
    :  ';' 
; 

Comma 
    :  ',' 
; 


RelOp :  '==' | '!=' | '<' | '>' | '<=' | '>=' ; 


Identifier  :  [a-zA-Z_] [a-zA-Z_0-9]+ ;  

Number :  [1-9] [0-9]* ; 

String_Literal
       :  ["] (~["\r\n] | '\\\\' | '\\"')* ["]
;

WS : [ \t\r\n]+ -> skip ;  

      



