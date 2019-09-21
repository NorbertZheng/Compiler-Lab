/* lex.c	 XL parser */
#include "lex.h"
#include <stdio.h>
#include <ctype.h>

char *yytext = "";		/* current word. Due to directly pointing
						   to input_buffer, it doesn't end with '\0',
						   be carefully! init it as 0, which means
						   buffer is empty, and needs read new line.
						Personal guess:
						   Maybe it uses \n.* rule to match one line,
						   then copy it into the buffer, yyless(1) 
						   push back the line's content, begin lex again.
						 */
int yyleng = 0;			/* the length of current word */
int yylineno = 0;		/* the line no of current word */

lex()
{
	static char input_buffer[128];
	char *current;

	current = yytext + yyleng;  	/* skip words that have been accessed */

	while(1) {						/* read next word */
    while(!*current) {
		/* if current line has been finished, 
		   read a new line from the keyboard,
		   and skip the space.
		 */
		current = input_buffer;
  		/* if an error occurs when reading a new line, 
		   set input_buffer empty and return EOI */
		if(!fgets(input_buffer, 127, stdin)) {
			*current = '\0' ;
			return EOI;
		}
		/* otherwise, increase yylineno */
		++yylineno;

		/* skip the space */
		while(isspace(*current))
			++current;
		}

		for(; *current; ++current) {
			/* Get the next token */
			yytext = current;
			yyleng = 1;

			/* according to current char, 
			   return different Token
			 */
			switch (*current) {
				case ';': return SEMI;
				case '+': return PLUS;
				case '-': return MINUS;
				case '/': return DIVISION;
				case '*': return TIMES;
				case '(': return LP;
				case ')': return RP;

				case '\n':
				case '\t':
				case ' ' : break;

				default:
					if(!isalnum(*current))
	    				fprintf(stderr, "Ignoring illegal input <%c>\n", *current);
					else {
						/* current char is alpha or number,
						   continue to read next char.
						Be careful:
						   before switch, yytext is already 
						   updated to current, so the D-value 
						   between current and yytext is yyleng.
						 */
						while(isalnum(*current))
							++current;
						yyleng = current - yytext;
						return NUM_OR_ID;
					}
					break;
			}
		}
	}
}

static int Lookahead = -1;		/* store the Token value we look ahead.
								   init it with -1, which means we should
								   read a word when we first call match func.
								 */

int match( int token )
{
	/* determine whether token is the same
	   as the current lookahead Token value.
	 */
	if(Lookahead == -1)
		Lookahead = lex();

	return token == Lookahead;
}

void advance()
{
	/* look ahead a word, and store its Token value 
	   in the static int val Lookahead. */
	Lookahead = lex();
}

