/* improved.c	XL improved syntax analysis */
#include <stdio.h>
#include "lex.h"

/* Revised parser  */
int	legal_lookahead(int first_arg, ...);

void factor(void);
void term(void);
void expression(void);

statements()
{
	/*  statements -> expression SEMI |  expression SEMI statements */

	while(!match(EOI)) {
		expression();

		if(match(SEMI))
			advance();
		else
			fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
	}
}

void expression()
{
	/* expression  -> term expression'
	 * expression' -> PLUS term expression' |  epsilon
	 */

	if (!legal_lookahead(NUM_OR_ID, LP, 0))
		return;

	term();
	while(match(PLUS) || match (MINUS)) {
		advance();		/* always look ahead next word, when temp is done. */
		term();
	}
}

void term()
{
	if(!legal_lookahead(NUM_OR_ID, LP, 0))
		return;

	factor();
	while(match(TIMES) || match(DIVISION)) {
		advance();		/* always look ahead next word, when temp is done. */
		factor();
	}
}

void factor()
{
	if(!legal_lookahead(NUM_OR_ID, LP, 0))
		return;

	if(match(NUM_OR_ID)) {
		advance();
	} else if(match(LP)) {
		advance();
		expression();
		if(match(RP)) {
			advance();
		} else {
			fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno);
		} 
	} else {
		fprintf(stderr, "%d: Number or identifier expected\n", yylineno);
	}
}

#include <stdarg.h>

#define MAXFIRST	16
#define SYNCH		SEMI

int legal_lookahead(int first_arg, ...)
{
	/* This function is a typical variable parameter 
	 * function whose parameters are the next list 
	 * of posible legitimate input. If the list is 
	 * empty to indicate the end tag of the matching 
	 * file, if the current input is not legitimate, 
	 * the function will abandon the current input 
	 * util there is a legitimate one. It is legitimate 
	 * to return a non-zero to indicate the character to 
	 * be looked forward. Otherwise, it means an error 
	 * that cannot be recovered.
	 */
	va_list args;
	int tok;
	int lookaheads[MAXFIRST], *p = lookaheads, *current;
	int error_printed = 0;
	int rval = 0;

	va_start(args, first_arg);

	if(!first_arg) {
		if(match(EOI))
			rval = 1;
	} else {
		*p++ = first_arg;
		while((tok = va_arg(args, int)) && p < &lookaheads[MAXFIRST]) {
			/* next token is available  && p(addr) < the max length of lookaheads */
			*p++ = tok;
		}

		while(!match(SYNCH)) {
			/* If match SYNCH(SEMI(;)), we arrive at the 
			 * end of current statement. return rval=0 
			 * immediately.
			Be careful:
			 * At present, it is not restorable. this function 
			 * will skip some Tokens to find one which equals
			 * the Token value of input parameters.
			 */
			for(current = lookaheads; current < p ; ++current) {
				if(match(*current)) {
					rval = 1;
					goto exit;
				}
			}

			if(!error_printed) {
				/* avoid dupicate printing of error messages */
				fprintf(stderr, "Line %d: Syntax error\n", yylineno);
				error_printed = 1;
			}

			advance();
		}
	}

exit:
	va_end(args);
	return rval;
}

