/* retval.c	XL generate intermediate language */
#include <stdio.h>
#include "lex.h"

/* definited in name.c */
extern char *newname(void);
extern void freename(char *name); 

char *factor(void);
char *term(void);
char *expression(void);

statements()
{
	/*  statements -> expression SEMI  |  expression SEMI statements  */

	char *tempvar;

	while (!match(EOI)) {
		tempvar = expression();

		if(match(SEMI)) {
			advance();
		} else {
			fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
		}
		freename( tempvar );
	}
}

char *expression()
{
	/* expression -> term expression'
	 * expression' -> PLUS term expression' |  epsilon
	 */

	char *tempvar, *tempvar2;

	tempvar = term();
  
	while (match(PLUS) || match(MINUS)) {
		char op = yytext[0];
		advance();

		tempvar2 = term();	/* right prior? */

		printf("    %s %c= %s\n", tempvar, op, tempvar2);
		freename(tempvar2);
	}

	return tempvar;
}

char *term()
{
	char *tempvar, *tempvar2 ;

	tempvar = factor();
	while (match(TIMES) || match(DIVISION)) {
		char op = yytext[0];
		advance();

		tempvar2 = factor();

		printf("    %s %c= %s\n", tempvar, op, tempvar2);
		freename(tempvar2);
	}
  
	return tempvar;
}

char *factor()
{
	char *tempvar;

	if (match(NUM_OR_ID)) {
		/* for yytext doesn't end with '\0', we need
		 * yyleng to identify the boundary of yytext.
		 * therefore, we use %0.*s(form control character)
		 * which use yyleng as the length of this print.
		 * this is a normal way to print variable length string.
		 */
		printf("    %s = %0.*s\n", tempvar = newname(), yyleng, yytext );
		advance(); 
	} else if (match(LP)) {
		advance();
		tempvar = expression();

		if(match(RP)) {
			advance();
		} else {
			advance();
			fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno);
		}
	} else {
		tempvar = newname();
		advance();
		fprintf(stderr, "%d: Number or identifier expected\n", yylineno);
	}

	return tempvar;
}

