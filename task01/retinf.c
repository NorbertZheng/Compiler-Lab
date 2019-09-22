/* retinf.c		AXL */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h"

#define _ID							0

#define REALOP(op)					((op == '+') || \
									 (op == '-') || \
									 (op == '*') || \
									 (op == '/'))
#define CHAR2MACRO(ch)				(((char) ch == '+') ? PLUS : \
									((char) ch == '-') ? MINUS : \
									((char) ch == '*') ? TIMES : \
									((char) ch == '/') ? DIVISION : _ID)
#define MACRO2CHAR(ch)				(((int) ch == PLUS) ? '+' : \
									((int) ch == MINUS) ? '-' : \
									((int) ch == TIMES) ? '*' : \
									((int) ch == DIVISION) ? '/' : \
									((int) ch == _ID) ? '=' : '?')

char err_id[] = "error";
char * midexp;
extern char * yytext;

struct DAG_LinkedListNode {
	unsigned int no;
	unsigned int op;
	char *affix;
	unsigned int left;
	unsigned int right;
	struct DAG_LinkedListNode *next;
};

struct DAG_LinkedList {
	struct DAG_LinkedListNode *head;
	struct DAG_LinkedListNode *tail;
	int size;
} DAGList;

unsigned int max(unsigned int a, unsigned int b)
{
	return (a > b) ? a : b;
}

void init_DAGList(void)
{
	DAGList.head = NULL;
	DAGList.tail = NULL;
	DAGList.size = 0;
}

unsigned int searchAffix(char *Affix)
{
	struct DAG_LinkedListNode *p;

	if (DAGList.size == 0) {
		return 0;
	} else {
		p = DAGList.head;
		while (p != NULL) {
			if (strncmp(Affix, p->affix, max((unsigned int) strlen(Affix), (unsigned int) strlen(p->affix))) == 0) {
				break;
			}
			p = p->next;
		}
		if (p == NULL) {
			return 0;
		} else {
			return p->no;
		}
	}
}

struct DAG_LinkedListNode *searchNo(unsigned int no)
{
	struct DAG_LinkedListNode *p;

	if (DAGList.size == 0) {
		goto searchNo_err;
	} else {
		p = DAGList.head;
		while (p != NULL) {
			if (p->no == no) {
				break;
			}
			p = p->next;
		}
		if (p == NULL) {
			goto searchNo_err;
		} else {
			return p;
		}
	}
searchNo_err:
	printf("searchNo_err: don't find!\n");
	return NULL;
}

unsigned int addDAGIDNode(char *Affix)
{
	unsigned int matchNo;
	struct DAG_LinkedListNode *node;

	if (DAGList.size == 0) {
		goto addDAGIDNode_add;
	} else {
		/* DAGList has contents */
		matchNo = searchAffix(Affix);
		if (matchNo != 0) {
			// printf("addDAGIDNode: I'm done!\n");
			return matchNo;
		} else {
			goto addDAGIDNode_add;
		}
	}
addDAGIDNode_add:
	// printf("addDAGIDNode: prepare to add!\n");
	node = malloc(sizeof(struct DAG_LinkedListNode));
	node->no = DAGList.size + 1;
	node->op = _ID;
	// printf("addDAGIDNode: prepare to malloc for affix!\n");
	node->affix = malloc(strlen(Affix) + 1);
	// printf("addDAGIDNode: prepare to copy for affix!\n");
	sprintf(node->affix, "%s", Affix);
	node->left = 0;
	node->right = 0;
	node->next = NULL;
	/* add to the list */
	if (DAGList.size == 0) {
		DAGList.head = node;
		DAGList.tail = node;
		DAGList.size += 1;
	} else {
		DAGList.tail->next = node;
		DAGList.size += 1;
		DAGList.tail = node;
	}
	// printf("addDAGIDNode: I'm done!\n");
	return node->no;
}

unsigned int addDAGOPNode(char op, char *leftAffix, char *rightAffix)
{
	unsigned int leftMatchNo, rightMatchNo, exprMatchNo;
	char *exprAffix;
	struct DAG_LinkedListNode *node;

	if (DAGList.size == 0) {
		goto addDAGOPNode_err;		/* No contents mean no id, left right don't match */
	} else {
		/* DAGList has contents */
		exprAffix = malloc(strlen(leftAffix) + strlen(rightAffix) + 2);
		sprintf(exprAffix, "%c%s%s", op, leftAffix, rightAffix);
		exprMatchNo = searchAffix(exprAffix);
		if (exprMatchNo != 0) {
			return exprMatchNo;
		} else {
			/* No equal expr exist */
			leftMatchNo = searchAffix(leftAffix);
			rightMatchNo = searchAffix(rightAffix);
			if (leftMatchNo == 0 || rightMatchNo == 0) {
				free(exprAffix);
				goto addDAGOPNode_err;
			} else {
				goto addDAGOPNode_add;
			}
		}
	}
addDAGOPNode_add:
	node = malloc(sizeof(struct DAG_LinkedListNode));
	node->no = DAGList.size + 1;
	node->op = CHAR2MACRO(op);
	node->affix = exprAffix;
	node->left = leftMatchNo;
	node->right = rightMatchNo;
	node->next = NULL;
	/* add to the list */
	if (DAGList.size == 0) {
		DAGList.head = node;
		DAGList.tail = node;
		DAGList.size += 1;
	} else {
		DAGList.tail->next = node;
		DAGList.size += 1;
		DAGList.tail = node;
	}
	return node->no;
addDAGOPNode_err:
	printf("addDAGOPNode_err: OP Node's left/right should in the list, but not!\n");
	return 0;
}

void displayDAG(void)
{
	struct DAG_LinkedListNode *p;

	if (DAGList.size == 0) {
		printf("It's empty!\n");
	} else {
		p = DAGList.head;
		printf("no\top\tleft\tright\n");
		while (p != NULL) {
			if (p->op == _ID) {
				printf("%d\t%c\t%s\t\n", p->no, MACRO2CHAR(p->op), p->affix);
			} else {
				printf("%d\t%c\t%d\t%d\n", p->no, MACRO2CHAR(p->op), p->left, p->right);
			}
			p = p->next;
		}
	}
}

/* defined in name.c */
char *newname(void);
extern void freename(char *name);

char *expression(void);

void statements (void)
{
	init_DAGList();
	/*  statements -> expression SEMI  |  expression SEMI statements  */
	printf("Please input an affix expression and ending with \";\"\n");
	while (!match(EOI)) {
		init_DAGList();
		if (expression() == 0) {
			goto statements_err;
		}

		if (match(SEMI)) {
			printf("Please input an affix expression and ending with \";\"\n");
			advance();
		} else {
			fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
		}
		displayDAG();
	}
statements_err:
	printf("statements_err: rise from expression call!\n");
	return;
}

char *expression()
{ 
	/*
	 * expression -> PLUS expression expression
	 *            |  MINUS expression expression
	 *            |  TIMES expression expression
	 *            |  DIVISION expression expression
	 *            |  NUM_OR_ID
	 */
	struct DAG_LinkedListNode *p;
	char *leftAffix, *rightAffix;

	if (match(NUM_OR_ID)) {
		leftAffix = (char *) malloc(yyleng + 1);
		strncpy(leftAffix, yytext, yyleng);
		leftAffix[yyleng] = 0;

		advance();

		p = searchNo(addDAGIDNode(leftAffix));
		
		/* for debug */
		displayDAG();
		if (p != NULL) {
			return p->affix;
		} else {
			goto expression_err;
		}
	} else if (match(PLUS) || match(MINUS) || match(TIMES) || match(DIVISION)) {
		char op = yytext[0];
		advance();

		leftAffix = expression();
		rightAffix = expression();

		p = searchNo(addDAGOPNode(op, leftAffix, rightAffix));

		/* for debug */
		displayDAG();
		if (p != NULL) {
			return p->affix;
		} else {
			goto expression_err;
		}
	}
expression_err:
	printf("expression_err: no entry exist in the DAG!\n");
	return 0;
}

