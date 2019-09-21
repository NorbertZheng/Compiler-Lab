/* retinf.c		AXL */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h"

#ifndef _NOP
#define _NOP
#define _TOP						0
#define _BOTTOM						1
#endif

#define CHAR2MACRO(ch)				(((char) ch == '+') ? PLUS : \
									((char) ch == '-') ? MINUS : \
									((char) ch == '*') ? TIMES : \
									((char) ch == '/') ? DIVISION : _BOTTOM)
#define OPHIGHPRIOR(last, current)	((current == PLUS && (last == PLUS || last == MINUS)) || \
									(current == MINUS && (last == PLUS || last == MINUS)) || \
									(current == TIMES)									  || \
									(current == DIVISION)								  || \
									(last_op == _TOP))

char err_id[] = "error";
char * midexp;
extern char * yytext;

struct YYLVAL {
	char *val;		/* record expression's intermediate reg name */
	char *expr;		/* record infix expression */
	int last_op;	/* last operation of expression for 
					 * elimination of redundant parentheses 
					 */
};
typedef struct YYLVAL Yylval;

/* defined in name.c */
char *newname(void);
extern void freename(char *name);

Yylval *expression (int last_op);
void init_Yylval(Yylval *yylval);

void statements (void)
{
	/*  statements -> expression SEMI  |  expression SEMI statements  */
	Yylval *expr;

	printf("Please input an affix expression and ending with \";\"\n");
	while (!match(EOI)) {
		expr = expression(_TOP);

		printf("the infix expression is %s\n", expr->expr);
		freename(expr->val);
		free(expr->expr);
		free(expr);
		if (match(SEMI)) {
			advance();
			printf("Please input an affix expression and ending with \";\"\n");
		} else {
			fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
		}
	}
}

Yylval *expression(int last_op)
{ 
	/*
	 * expression -> PLUS expression expression
	 *            |  MINUS expression expression
	 *            |  TIMES expression expression
	 *            |  DIVISION expression expression
	 *            |  NUM_OR_ID
	 */
	Yylval *left, *right;

	char *leftinf, *rightinf;

	if (match(NUM_OR_ID)) {
		char *leftval = newname();
		/* get the expression of Token */
		leftinf = (char *) malloc(yyleng + 1);
		strncpy(leftinf, yytext, yyleng);
		leftinf[yyleng] = 0;

		printf("    %s = %s\n", leftval, leftinf);

		left = (Yylval *) malloc(sizeof(Yylval));
		init_Yylval(left);
		left->val = leftval;
		left->expr = leftinf;
		left->last_op = last_op;

		advance();
		goto finish;
	} else if (match(PLUS) || match(MINUS) || match(TIMES) || match(DIVISION)) {
		char op = yytext[0];
		advance();

		left = expression(CHAR2MACRO(op));
		right = expression(CHAR2MACRO(op));
		/* generate code by side effects */
		printf("    %s %c= %s\n", left->val, op, right->val);

		/* first free reg name it occupis */
		freename(right->val);
		if (OPHIGHPRIOR(last_op, CHAR2MACRO(op))) {
			leftinf = (char *) malloc(strlen(left->expr) + strlen(right->expr) + 4);
			sprintf(leftinf, "%s %c %s", left->expr, op, right->expr);
		} else {
			leftinf = (char *) malloc(strlen(left->expr) + strlen(right->expr) + 6);
			sprintf(leftinf, "(%s %c %s)", left->expr, op, right->expr);	
		}
		/* then update the left->expr */
		free(left->expr);
		left->expr = leftinf;
		/* finally free the rest of right */
		free(right->expr);
		free(right);

		goto finish;
	}
finish:
	return left;
}

void init_Yylval(Yylval *yylval)
{
	/* init pointer as NULL */
	yylval->val = NULL;
	yylval->expr = NULL;
	/* _BOTTOM represents bottom-level expression */
	yylval->last_op = _BOTTOM;
}

