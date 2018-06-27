%{
	#include <stdio.h> 
	#include <stdlib.h>
%}

LETTRE [a-zA-Z]+
MOT {LETTRE}+


%%

	

%%
int main(){
	yylex();
}


int yywrap(){
    return 1;
    }   