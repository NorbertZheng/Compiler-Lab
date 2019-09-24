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
#define OPHIGHPRIOR(last, current)	((current == PLUS && (last == PLUS || last == MINUS)) || \
									(current == MINUS && (last == PLUS || last == MINUS)) || \
									(current == TIMES)									  || \
									(current == DIVISION))

char err_id[] = "error";
char * midexp;
extern char * yytext;

/* defined in name.c */
char *newname(void);
extern void freename(char *name);

/* DAG */
struct DAG_LinkedListNode {
	unsigned int no;
	unsigned int isOp;
	char *value;
	unsigned int left;
	unsigned int right;
	struct DAG_LinkedListNode *next;
};

struct DAG_LinkedList {
	struct DAG_LinkedListNode *head;
	struct DAG_LinkedListNode *tail;
	int size;
} DAGList;

void DAG_initDAGList(struct DAG_LinkedList *list)
{
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

/* 0 : donot exist		other : no in the DAGList */
unsigned int DAG_searchIdOrOp(struct DAG_LinkedList *list, unsigned int isOp, char *value, unsigned int left, unsigned int right)
{
	struct DAG_LinkedListNode *p;

	if (list->size == 0) {
		return 0;
	} else {
		p = list->head;
		while (p != NULL) {
			if (isOp) {
				/* use left and right */
				if (!strcmp(value, p->value) && (p->left == left) && (p->right == right) && p->isOp) {
					/* the string(char) of op equal, so is left and right */
					break;
				}
			} else {
				/* just define val */
				if (!(p->isOp) && !strcmp(value, p->value)) {
					break;
				}
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

struct DAG_LinkedListNode *searchNo(struct DAG_LinkedList *list, unsigned int no)
{
	struct DAG_LinkedListNode *p;

	if (list->size == 0) {
		goto searchNo_err;
	} else {
		p = list->head;
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

unsigned int DAG_addIDNode(struct DAG_LinkedList *list, char *name)
{
	unsigned int matchNo;
	struct DAG_LinkedListNode *node;

	if (list->size == 0) {
		goto addDAGIDNode_add;
	} else {
		/* DAGList has contents */
		matchNo = DAG_searchIdOrOp(list, 0, name, 0, 0);
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
	node->no = list->size + 1;
	node->isOp = 0;
	// printf("addDAGIDNode: prepare to malloc for affix!\n");
	node->value = malloc(strlen(name) + 1);
	// printf("addDAGIDNode: prepare to copy for affix!\n");
	sprintf(node->value, "%s", name);
	node->value[strlen(name)] = 0;
	node->left = 0;
	node->right = 0;
	node->next = NULL;
	/* add to the list */
	if (list->size == 0) {
		list->head = node;
		list->tail = node;
		list->size += 1;
	} else {
		list->tail->next = node;
		list->size += 1;
		list->tail = node;
	}
	// printf("addDAGIDNode: I'm done!\n");
	return node->no;
}

unsigned int DAG_addOPNode(struct DAG_LinkedList *list, char *op, unsigned int left, unsigned int right)
{
	unsigned int exprMatchNo;
	struct DAG_LinkedListNode *node;

	if (list->size == 0) {
		goto DAG_addOPNode_err;		/* No contents mean no id, left right don't match */
	} else {
		/* DAGList has contents */
		exprMatchNo = DAG_searchIdOrOp(list, 1, op, left, right);
		if (exprMatchNo != 0) {
			return exprMatchNo;
		} else {
			goto DAG_addOPNode_add;
		}
	}
DAG_addOPNode_add:
	node = malloc(sizeof(struct DAG_LinkedListNode));
	node->no = list->size + 1;
	node->isOp = 1;
	node->value = malloc(sizeof(char) * 2);
	sprintf(node->value, "%s", op);
	node->value[1] = 0;
	node->left = left;
	node->right = right;
	node->next = NULL;
	/* add to the list */
	if (list->size == 0) {
		list->head = node;
		list->tail = node;
		list->size += 1;
	} else {
		list->tail->next = node;
		list->size += 1;
		list->tail = node;
	}
	return node->no;
DAG_addOPNode_err:
	printf("addDAGOPNode_err: OP Node's left/right should in the list, but not!\n");
	return 0;
}

void DAG_display(struct DAG_LinkedList *list)
{
	struct DAG_LinkedListNode *p;

	if (list->size == 0) {
		printf("It's empty!\n");
	} else {
		p = list->head;
		printf("no\top\tleft\tright\n");
		while (p != NULL) {
			if (!(p->isOp)) {
				printf("%d\t%c\t%s\t\n", p->no, '=', p->value);
			} else {
				printf("%d\t%c\t%d\t%d\n", p->no, p->value[0], p->left, p->right);
			}
			p = p->next;
		}
	}
}

void DAG_free(struct DAG_LinkedList *list)
{
	struct DAG_LinkedListNode *p, *q;

	if (list->size == 0) {
		// do nothing
	} else {
		// printf("DAG_free: prepare to free all!\n");
		p = list->head;
		q = p->next;
		while (q != NULL) {
			free(p);
			p = q;
			q = p->next;
			// printf("p : %x\n", p);
		}
		free(p);
		// printf("DAG_free: free all!\n");
	}
	DAG_initDAGList(list);
	return;
}

/* BinaryTree */
#define LOC_INIT			0
/* temp useless */
#define LOC_LEFT			1
#define LOC_RIGHT			2
struct BinaryTreeNode {
	char *value;
	unsigned int isOp;
	unsigned int loc;		/* use for DAG LinkedList loc */
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
	// left->loc = LOC_LEFT;
	p->right = right;
	right->parent = p;
	// right->loc = LOC_RIGHT;

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

char *BinaryTree_generateInfixHelper(struct BinaryTreeNode *p)
{
	char *left, *right, *expr;

	if (p == NULL) {
		return NULL;
	} else {
		/* post order */
		if (p->left != NULL && p->right != NULL) {
			left = BinaryTree_generateInfixHelper(p->left);
			right = BinaryTree_generateInfixHelper(p->right);

			if (p->parent == NULL) {
				expr = malloc(strlen(left) + strlen(right) + 4);
				sprintf(expr, "%s %c %s", left, p->value[0], right);
				expr[strlen(left) + strlen(right) + 3] = 0;
			} else {
				if (OPHIGHPRIOR(CHAR2MACRO(p->parent->value[0]), CHAR2MACRO(p->value[0]))) {
					expr = malloc(strlen(left) + strlen(right) + 4);
					sprintf(expr, "%s %c %s", left, p->value[0], right);
					expr[strlen(left) + strlen(right) + 3] = 0;
				} else {
					expr = malloc(strlen(left) + strlen(right) + 6);
					sprintf(expr, "(%s %c %s)", left, p->value[0], right);
					expr[strlen(left) + strlen(right) + 5] = 0;
				}
			}
			free(left);
			free(right);
		} else if ((p->left != NULL) ^ (p->right != NULL)) {
			printf("BinaryTree_reconstructFHelper_err: one of p'children is NULL, but the other not!\n");
			return NULL;
		} else {
			expr = malloc(strlen(p->value) + 1);
			sprintf(expr, "%s", p->value);
			expr[strlen(p->value)] = 0;
		}

		return expr;
	}
}

void BinaryTree_generateInfix(void)
{
	printf("the infix expression is %s\n", BinaryTree_generateInfixHelper(BinaryTreeRoot));
}

unsigned int BinaryTree_buildDAGHelper(struct DAG_LinkedList *list, struct BinaryTreeNode *p)
{
	unsigned int leftNo, rightNo;

	if (p == NULL) {
		return 0;
	} else {
		/* post order */
		if (p->left != NULL && p->right != NULL) {
			leftNo = BinaryTree_buildDAGHelper(list, p->left);
			rightNo = BinaryTree_buildDAGHelper(list, p->right);

			if (p->isOp) {
				return DAG_addOPNode(list, p->value, leftNo, rightNo);
			} else {
				goto BinaryTree_buildDAGHelper_err;
				// return DAG_addIDNode(p->value);
			}
		} else if ((p->left != NULL) ^ (p->right != NULL)) {
			printf("BinaryTree_buildDAGHelper_err: one of p'children is NULL, but the other not!\n");
			return;
		} else {
			if (p->isOp) {
				goto BinaryTree_buildDAGHelper_err;
				// return DAG_addOPNode(p->value, leftNo, rightNo);
			} else {
				return DAG_addIDNode(list, p->value);
			}
		}
	}
BinaryTree_buildDAGHelper_err:
	printf("BinaryTree_buildDAGHelper_err: should never happen, somthing is wrong in the BinaryTree!\n");
	return 0;
}

void BinaryTree_buildDAG(struct DAG_LinkedList *list)
{
	BinaryTree_buildDAGHelper(list, BinaryTreeRoot);
}

void BinaryTree_freeHelper(struct BinaryTreeNode *p)
{
	if (p == NULL) {
		return;
	} else {
		/* post order */
		if (p->left != NULL && p->right != NULL) {
			BinaryTree_freeHelper(p->left);
			BinaryTree_freeHelper(p->right);

			free(p);
		} else if ((p->left != NULL) ^ (p->right != NULL)) {
			printf("BinaryTree_freeHelper_err: one of p'children is NULL, but the other not!\n");
			return;
		} else {
			free(p);
		}
		return;
	}
}

void BinaryTree_free(void)
{
	BinaryTree_freeHelper(BinaryTreeRoot);
	// printf("BinaryTree_free: finished!\n");
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
		// BinaryTreeNode_display();
		BinaryTree_generateTAC();
		BinaryTree_generateInfix();
		BinaryTree_buildDAG(&DAGList);
		// DAG_display(&DAGList);
		// free all, prepare for the next loop
		BinaryTree_free();
		DAG_free(&DAGList);
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

