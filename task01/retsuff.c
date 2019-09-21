/* retval.c		XL generate prefix expression & intermediate expression */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h"

char err_id[] = "error";
char *midexp;

struct YYLVAL {
  char *val;		/* record expression's intermediate temp reg */
  char *expr;		/* record expression's prefix expression */
};
typedef struct YYLVAL Yylval;

/* defined in name.c */
extern char *newname( void );
extern void freename( char *name );

Yylval *factor(void);
Yylval *term(void);
Yylval *expression(void);

void statements(void)
{
	/*  statements -> expression SEMI  |  expression SEMI statements  */
	Yylval *temp;
	printf("Please input an infix expression and ending with \";\"\n");
	while (!match(EOI)) {
		temp = expression();

		printf("the affix expression is %s\n", temp -> expr);
		freename(temp->val);
		free(temp->expr);
		free(temp);
  
		if (match(SEMI)) {
			advance();
			printf("Please input an infix expression and ending with \";\"\n");
		} else {
			fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
		}
	}
}

Yylval *expression()
{
	/* expression -> term expression'
	 * expression' -> PLUS term expression' 
     *             |  MINUS term expression'
     *             |  epsilon           
	 */
	Yylval *temp, *temp2;

	char *tmpmid;		/* record prefix expression */
	char *tmpmid1;

	temp = term();
	tmpmid = temp->expr;

	while (match(PLUS) || match(MINUS)) {
		char op = yytext[0];
		advance();

		temp2 = term();
		printf("    %s %c= %s\n", temp ->val, op, temp2 ->val);
		/* generate code by side effects */
		freename(temp2->val);
      
		/* generate the coresponding prefix expression 
		 * for the sentence pattern (term PLUS term2) : 
		 * "+" + " " + term->expr + " " + term2->expr  
		 */
		/* generate the coresponding prefix expression 
		 * for the sentence pattern (term MINUS term2) : 
		 * "-" + " " + term->expr + " " + term2->expr  
		 */
		tmpmid1 = (char *) malloc(strlen(temp2->expr) + strlen(tmpmid) + 4);
		sprintf(tmpmid1, "%c %s %s", op, tmpmid, temp2 -> expr);

		free(tmpmid);		/* remember to free! */

		tmpmid = tmpmid1;	/* left prior */
		free(temp2->expr);
		free(temp2); 
	}
	temp->expr = tmpmid;	/* use the first term's YYLVAL as its return */

	return temp;
}

Yylval *term(void)
{
	Yylval *temp, *temp2 ;
	char *tmpmid, *tmpmid1;

	temp = factor();
	tmpmid = temp->expr;

	while (match(TIMES) || match(DIVISION)) {
		char op = yytext[0];
		advance();

		temp2 = factor();
		printf("    %s %c= %s\n", temp->val, op, temp2->val);
		/* generate code by side effects */

		freename(temp2->val);		/* temp->val take over */

		tmpmid1 = (char *) malloc(strlen(temp2->expr) + strlen(tmpmid) + 4);
		sprintf ( tmpmid1, "%c %s %s", op, tmpmid, temp2 -> expr);

		free(tmpmid);				/* remember to free! */

		tmpmid = tmpmid1;
		free(temp2->expr);
		free(temp2);
	}
	temp->expr = tmpmid;

	return temp;
}

Yylval *factor(void)
{
	Yylval *temp;
	char * tmpvar, *tmpexpr;

	if (match(NUM_OR_ID)) {
		tmpvar = newname();
		tmpexpr = (char *) malloc(yyleng + 1);
		/* +1 -> +1(for '\0') */
		strncpy(tmpexpr, yytext, yyleng);
		tmpexpr[yyleng] = 0; 

		printf("    %s = %s\n", tmpvar, tmpexpr);
		/* generate code by side effects */

		/* must use malloc, cannot return the pointer 
		 * which point to the local val!
		 */
		temp = (Yylval *) malloc(sizeof(Yylval));
		temp->val = tmpvar;
		temp->expr = tmpexpr;
		advance();
	} else {
		if (match(LP)) {
			advance();
			temp = expression();

			if (match(RP)) {
				advance();
			} else {
				fprintf(stderr, "%d: Mismatched parenthesis\n", yylineno);
			}
		} else {
			char *s ;
			advance();

			s = (char *) malloc(10);
			strcpy(s,"error_id");
			fprintf(stderr, "%d: Number or identifier expected\n", yylineno);
			temp = (Yylval *) malloc(sizeof(Yylval));
			temp->val =  newname();
			temp->expr = s;
		}
	}

	return temp;
}

