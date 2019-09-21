/* plain.c	XL syntax analysis */
/* Basic parser, shows the structure but there's no code generation */
#include <stdio.h>
#include "lex.h"

statements()
{
	/*  statements -> expression SEMI
	 *             |  expression SEMI statements
	 */

	expression();

	if(match(SEMI)) {
		/* if the end match, always look ahead a new word. */
		advance();
    } else {
		fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
	}

	/* seems that we already give tacit consent 
	   to the suppose that input contains only 
	   statements!
	 */
	if(!match(EOI))
		statements();			/* handle next sentence */
}

expression()
{
	/* expression -> term expression' */

	term();
	expr_prime();
}

expr_prime()
{
	/* expression' -> PLUS term expression'
	 *              | epsilon
	 */

	if(match(PLUS))
	{
		advance();		/* always look ahead next word, when temp is done. */
		term();
		expr_prime();
	}
}

term()
{
	/* term -> factor term' */

	factor();
	term_prime();
}

term_prime()
{
	/* term' -> TIMES factor term'
	 *       |   epsilon
	 */

	if(match(TIMES))
	{
		advance();		/* always look ahead next word, when temp is done. */
		factor();
		term_prime();
	}
}

factor()
{
	/* factor   ->    NUM_OR_ID
	 *          |     LP expression RP
	 */

	if(match(NUM_OR_ID)) {
		advance();		/* always look ahead next word, when temp is done. */
	} else if(match(LP)) {
		advance();		/* always look ahead next word, when temp is done. */
		expression();
		if(match(RP))
			advance();	/* always look ahead next word, when temp is done. */
		else
			fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno);
	} else {
		fprintf(stderr, "%d Number or identifier expected\n", yylineno);
	}
}

