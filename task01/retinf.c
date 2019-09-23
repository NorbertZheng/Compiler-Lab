/* retinf.c		AXL */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h"

#define _ID							0
#define _NOP						0

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

/* defined in name.c */
char *newname(void);
extern void freename(char *name);

/* DAG */
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

/* BinaryTree */
#define LOC_INIT			0
#define LOC_LEFT			1
#define LOC_RIGHT			2
struct BinaryTreeNode {
	char *value;
	unsigned int isOp;
	unsigned int loc;
	char *reg;
	struct BinaryTreeNode *parent;
	struct BinaryTreeNode *left;
	struct BinaryTreeNode *right;
};

/* BinaryTree root */
struct BinaryTreeNode *BinaryTreeRoot;

struct BinaryTreeNode *BinaryTree_addOPNode(char op, struct BinaryTreeNode *left, struct BinaryTreeNode *right)
{
	struct BinaryTreeNode *p;

	p = malloc(sizeof(struct BinaryTreeNode));
	p->value = malloc(sizeof(char) * 2);
	sprintf(p->value, "%c", op);
	p->value[1] = 0;
	p->isOp = 1;
	p->loc = LOC_INIT;
	p->reg = NULL;
	p->parent = NULL;
	p->left = left;
	left->parent = p;
	left->loc = LOC_LEFT;
	p->right = right;
	right->parent = p;
	right->loc = LOC_RIGHT;

	return p;
}

struct BinaryTreeNode *BinaryTree_addIDNode(char *id)
{
	struct BinaryTreeNode *p;

	p = malloc(sizeof(struct BinaryTreeNode));
	p->value = malloc(strlen(id) + 1);
	sprintf(p->value, "%s", id);
	p->isOp = 0;
	p->loc = LOC_INIT;
	p->reg = NULL;
	p->parent = NULL;
	p->left = NULL;
	p->right = NULL;

	return p;
}

void BinaryTreeNode_displayHelper(struct BinaryTreeNode *p, unsigned int level)
{
	int i;

	if (level == 0) {
		printf("    ");
	} else {
		for (i = 0;i < level; i++)
			printf("    ");
	}
	if (level)
		printf("|__ ");

	printf("%s\n", p->value);

	if (p->left != NULL)
		BinaryTreeNode_displayHelper(p->left, level + 1);
	if (p->right != NULL)
		BinaryTreeNode_displayHelper(p->right, level + 1);
}

void BinaryTreeNode_display(void)
{
	printf("BinaryTree:\n");
	BinaryTreeNode_displayHelper(BinaryTreeRoot, 0);
}

/* 0 : same		1 : different */
unsigned int BinaryTreeNode_cmpValue(struct BinaryTreeNode *p, struct BinaryTreeNode *q)
{
	if (p == NULL || q == NULL) {
		return 1;
	} else {
		return strcmp(p->value, q->value);
	}
}

/* 0 : same		1 : different */
unsigned int BinaryTreeNode_AddOrTime(struct BinaryTreeNode *p)
{
	return !(strcmp(p->value, "+") ^ strcmp(p->value, "*"));
}

void BinaryTree_reconstructFHelper(struct BinaryTreeNode *p)
{
	struct BinaryTreeNode *pLeft, *pRight;

	if (p == NULL) {
		return;
	} else {
		/* post order */
		if (p->left != NULL && p->right != NULL) {
			BinaryTree_reconstructFHelper(p->left);
			BinaryTree_reconstructFHelper(p->right);
		} else if ((p->left != NULL) ^ (p->right != NULL)) {
			printf("BinaryTree_reconstructFHelper_err: one of p'children is NULL, but the other not!\n");
			return;
		} else {
			/* leaf node, no need to reconstruct */
			return;
		}

		/*			0					0
		 *			+					+
		 *		  /   \ 	===>	  /   \
		 *	   1 *	   + 2  	   2 +	   * 1
		 *			 /   \		   /   \
		 *			+	  *		  +	    *		(posible level)
		 *			3	  4		  3		4
		 */
		if (!BinaryTreeNode_cmpValue(p, p->right) && BinaryTreeNode_cmpValue(p, p->left) && !BinaryTreeNode_AddOrTime(p)) {
			/* p->value = * or +, and p->value is equal to right->value, not left->value */
			pLeft = p->left;
			p->left = p->right;
			p->right = pLeft;
		}
		/*					0							0							0
		 *			ptr-->	+					ptr-->  +				  	ptr-->  +
		 *				  /   \ 					  /   \						  /   \	
		 *				 /	   \		===>		 /	   \		===>		 /	   \
		 *			  1 +		+ 2 			  2 +		+ 5				  5 +		* 7
		 *			   / \	   / \				   / \	   / \				   / \
		 *			  /   \	  /   \			  	  /   \	  /   \     	      /   \
		 *			 +	   * +     *		   1 +	   * *     *    	   2 +     * 8				(Recursion!!!)
		 *			 3     4 5     6			/ \    6 7     8			/ \
		 *									   /   \					   /   \
		 *									  +		*					1 +     * 6
		 *									  3     4					 / \
		 *																/   \
		 *												  		 	   +     *
		 *														 	   3	 4
		 */
		else if (!BinaryTreeNode_cmpValue(p, p->right) && !BinaryTreeNode_cmpValue(p, p->left) && !BinaryTreeNode_AddOrTime(p)) {
			/* p->value = * or +, and p->value is equal to right->value, and left->value */
			while (!BinaryTreeNode_cmpValue(p, p->right) && !BinaryTreeNode_cmpValue(p, p->left) && !BinaryTreeNode_AddOrTime(p)) {
				/* 0->right = 5, remember to change 5's parent */
				pRight = p->right;
				p->right = p->right->left;
				p->right->parent = p;
				/* 0->left = 2, and 2->left = 1, remember to change 1's parent */
				pLeft = p->left;
				p->left = pRight;
				p->left->left = pLeft;
				p->left->left->parent = p->left;
				/* only (1 & 5)'s parent is changed */
			}
		}
	}
}

void BinaryTree_reconstructF(void)
{
	BinaryTree_reconstructFHelper(BinaryTreeRoot);
}

void BinaryTree_generateTACHelper(struct BinaryTreeNode *p)
{
	if (p == NULL) {
		return;
	} else {
		/* post order */
		if (p->left != NULL && p->right != NULL) {
			BinaryTree_generateTACHelper(p->left);
			BinaryTree_generateTACHelper(p->right);

			p->reg = p->left->reg;
			// p->left->reg = NULL;		// will change this assign for DAG in the future
			printf("    %s %c= %s\n", p->reg, p->value[0], p->right->reg);
			// release right->reg
			freename(p->right->reg);
		} else if ((p->left != NULL) ^ (p->right != NULL)) {
			printf("BinaryTree_reconstructFHelper_err: one of p'children is NULL, but the other not!\n");
			return;
		} else {
			p->reg = newname();
			printf("    %s = %s\n", p->reg, p->value);
		}

		/* BinaryTreeRoot release the last reg */
		if (p == BinaryTreeRoot)
			freename(p->reg);
		return;
	}
}		

void BinaryTree_generateTAC(void)
{
	BinaryTree_generateTACHelper(BinaryTreeRoot);
}

struct BinaryTreeNode *expression(void);

void statements (void)
{
	/*  statements -> expression SEMI  |  expression SEMI statements  */
	struct BinaryTreeNode *statementRoot;

	printf("Please input an affix expression and ending with \";\"\n");
	while (!match(EOI)) {
		statementRoot = expression();

		BinaryTreeRoot = statementRoot;
		BinaryTree_reconstructF();
		BinaryTreeNode_display();
		BinaryTree_generateTAC();
		if (match(SEMI)) {
			printf("Please input an affix expression and ending with \";\"\n");
			advance();
		} else {
			fprintf(stderr, "%d: Inserting missing semicolon\n", yylineno);
		}
	}
statements_err:
	printf("statements_err: rise from expression call!\n");
	return;
}

struct BinaryTreeNode *expression()
{ 
	/*
	 * expression -> PLUS expression expression
	 *            |  MINUS expression expression
	 *            |  TIMES expression expression
	 *            |  DIVISION expression expression
	 *            |  NUM_OR_ID
	 */
	struct BinaryTreeNode *p;
	struct BinaryTreeNode *left, *right;
	char *leftAffix, *rightAffix;

	if (match(NUM_OR_ID)) {
		leftAffix = (char *) malloc(yyleng + 1);
		strncpy(leftAffix, yytext, yyleng);
		leftAffix[yyleng] = 0;

		advance();

		p = BinaryTree_addIDNode(leftAffix);
		free(leftAffix);
		if (p != NULL) {
			return p;
		} else {
			goto expression_err;
		}
	} else if (match(PLUS) || match(MINUS) || match(TIMES) || match(DIVISION)) {
		char op = yytext[0];
		advance();

		left = expression();
		right = expression();

		p = BinaryTree_addOPNode(op, left, right);

		if (p != NULL) {
			return p;
		} else {
			goto expression_err;
		}
	}
expression_err:
	printf("expression_err: no entry exist in the DAG!\n");
	return 0;
}

