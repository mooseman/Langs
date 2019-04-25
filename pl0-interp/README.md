

# PL0 Interpreter

https://en.wikipedia.org/wiki/PL/0

This code is all based on the C++ PL/0 interpreter written by Marcin Josiński 
( https://github.com/Marcin-Josinski/pl0-interpreter ) 

I am in the process of translating the code to C. 
Any bugs are *my* bugs, not Marcin's.  

Very many thanks for writing that, Marcin, and special thanks for 
releasing it under the Unlicense!   

This C version of the interpreter is also released as "public domain" 
under the Unlicense.  
"Share and enjoy........."   :)  


## Building
Just cd to the src directory and run Make. 

```
cd src
make 
```

## Usage
```
./pl0 path_to_code.pl0
```
Example:
```
./pl0 ../../examples/ex1.pl0 
```
## To do

PL0 Virtual Machine and fix call_statement
