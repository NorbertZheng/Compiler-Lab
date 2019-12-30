/************************************************************/
/*      copyright hanfei.wang@gmail.com                     */
/*             2017.12.02                                   */
/************************************************************/
#include "tree.h"
#include "type.h"

/* the cursor of new type index for the type_env[] and type structure */
static int nindex = 0;    
static int step = 0;  

static Type inttype = {0, 3, NULL, NULL};		// INT type struct
static Type_env inttype_entry = {0, NULL};		// INT type entry

static Type_env_ptr type_env[MAXNODE];

/* non-static variable */
Type_ptr global_type_env[MAX_ENV];

/* for the inherited attribute abs, we use list as the stack */
extern char *name_env[MAX_ENV];
extern FILE * yyin;

Type_ptr make_rec_type(void);
Type_ptr type_copy(Type_ptr t);

Var_list_ptr add_var_list(Type_ptr var, Var_list_ptr list)
{
	Var_list_ptr tmp = (Var_list_ptr) smalloc(sizeof(Var_list)); 
	tmp->type_var = var;
	tmp->next = list;		// add to the head, and return the head of list
	return tmp; 
}

Var_list_ptr list_copy(Var_list_ptr source) 
{
	if (source == NULL) {
		return NULL;
	}

	// from head to tail, rec copy
	return add_var_list(source->type_var, list_copy(source->next));
}

static void free_list(Var_list_ptr source)
{
	Var_list_ptr tmp;

	while (source != NULL) {
		tmp = source;
		source = source->next;		// update the head of source
		sfree(tmp);					// free the old head
	}

	return;
}

Type_ptr make_op_type()
{
	return make_arrowtype(&inttype, make_arrowtype(&inttype, &inttype)); 
}

void init_type_env()
{
	int i = 0;

	type_env[0] = &inttype_entry;
	type_env[0]->type = &inttype;		// change the NULL to inttype(original)
	while (i < INIT_POS) {
		new_env();
		global_type_env[i] = storetype(make_arrowtype(&inttype, make_arrowtype(&inttype, &inttype)));    
		i++;
	}

	return;
}

Type_ptr get_n_th_from_global(int i)
{      
	if (strcmp(name_env[i], "Z") == 0 ||
		strcmp(name_env[i], "Y") == 0 ||
		strcmp(name_env[i], "rec") == 0) {
		return make_rec_type();
	}
	return restoretype(global_type_env[i]);
}

/* pos is the current top of name_env[] */
Type_ptr get_n_th(Var_list_ptr list, int n, int pos)
{ 

	int i = 0;
	while (i != n - 1 && list != NULL) {
		list = list->next;
		i++;
	}

	/* get the target & not end loop because of list is NULL */
	if (i == n - 1  && list != NULL) {
		return list -> type_var;
	}

	if ((pos - n) >= 0) {
		return get_n_th_from_global(pos - n);
	}

	printf("wrong access global type env\n");
	exit (1);
}

/* test whether type_env overflow */
void test_limit_env(void) 
{
	if (nindex == MAXNODE) {
		printf("Maximum typing node exceeds!\n");
		exit (1);
	}

	return;
}

void new_type_env(Type_ptr t)
{
	type_env[nindex] = (Type_env_ptr) smalloc(sizeof(Type_env));
	type_env[nindex]->type = t;
	type_env[nindex]->redirect = t->index;		// point to itself

	return;
}

/******************/
/*   MAKE TYPE    */
/******************/
Type_ptr make_inttype()
{
	return &inttype;		// no need to make inttype, for we already get one!
}

/* the bound is the bound level of abstraction */
Type_ptr make_vartype(void)
{
	Type_ptr tmp = (Type_ptr) smalloc(sizeof(Type));
	test_limit_env();		// for safety
 
	tmp->index = nindex;	// De Brujin index
	tmp->kind = Typevar;
	tmp->left = NULL;
	tmp->right = NULL;
	new_type_env(tmp);		// add into type_env(static)

	nindex++;				// update nindex

	return tmp;
}

/* it's not a new type, need sub-type to construct it */
Type_ptr make_arrowtype(Type_ptr left, Type_ptr right)
{
	Type_ptr tmp = (Type_ptr) smalloc(sizeof(Type));
	test_limit_env ();		// for safety

	tmp->kind = Arrow;
	tmp->index = nindex;  
	tmp->left = left;		// not NULL!!!
	tmp->right = right;
	new_type_env(tmp);		// add into type_env(static)

	nindex ++;				// update nindex

	return tmp;
}

/* a (Typevar, index) node is final iff 
   type_env[index] -> type -> index == index */ 
/* find final type index for a given index */
int final_index(int index)
{
	int i = index;
	if (type_env[i] == NULL) {
		return -1;			// index is invalid
	}
	if (type_env[i]->type->kind == Arrow) {
		return i;			// arrow cannot redirect
	}

	/* other type(int & typevar) can redirect, we need to bak */
	if (i == (type_env[i]->redirect)) {
		return i;
	}
	return final_index(type_env[i]->redirect);
}

/* return final type node for a giving Typevar node */
Type_ptr final_type(Type_ptr t)
{
	int i;

	/* we need get the origin redirect type first */
	i = final_index(t->index);
	if (i == -1) {
		return NULL;		// t->index is invalid
	}
	return type_env[i]->type;
}

/* simply will replace the leaves of the type_tree t 
   with the corresponding final type */
Type_ptr simply(Type_ptr t)
{
	if (t == NULL) {
		return NULL;
	}
	/* all go bake to the origin */
	switch (t->kind) {
		case Int:
			return t;
		case Typevar: {
			int i = t->index;
			Type_ptr f = final_type(t);
			type_env[i]->redirect = f->index;
			return f;
 		}
		default: {
			/* arrow, will change the original arrow, then return its pointer */
			Type_ptr left = simply(t->left), right = simply(t->right);
			t->left = left;
			t->right = right;
			return t;
		}
	}
}

/* for a giving final Typevar node, rewrite it to
   an arrow node */
Type_ptr get_instance(Type_ptr type_tree)
{
	Type_ptr p = final_type(type_tree);

	p->kind = Arrow;
	p->left = make_vartype();
	p->right = make_vartype();

	return p; 
}

/* left child of an Arrow node is not necessary final */
Type_ptr get_left(Type_ptr type_tree)
{
	if (type_tree == NULL) {
		return NULL;
	}
	return (type_tree -> left);
}

Type_ptr get_right(Type_ptr type_tree)
{
	if (type_tree == NULL) {
		return NULL;
	}
	return (type_tree -> right);
}

/* type_tree must be simpled */
int is_occur_node(int index, Type_ptr type_tree)
{
	int i = index;

	if (type_tree == NULL) {
		return 1;
	}
	/* return whether the origin node */
	switch (type_tree->kind) {
		case Typevar:
			return type_env[type_tree->index]->redirect == i;
		case Arrow: 
			/* left and right may be not final!!! */
			return is_occur_node(i, final_type(type_tree->left)) || is_occur_node(i, final_type(type_tree->right));
		case Int:
			return 0;
	}
}

/* let t1 redirect to t2 */
void unify_leaf(Type_ptr t1, Type_ptr t2)
{
	int index1 = (t1->index);
	int index2 = (t2->index);

	if (index1 != index2) {
		type_env[index1]->redirect = index2;
	}
	return;
}

void unify_leaf_arrow(Type_ptr leaf, Type_ptr t)
{
	int index = leaf->index;
	type_env[index]->redirect = t->index;
	return;
}

void print_error(Type_ptr ty1, Type_ptr ty2)
{
	printf("type ");
	printtype(ty1);
	printf(" and type ");
	printtype(ty2 );
	printf(" can't be unified!\n");

	return;
}

/* precondition: t1 and t2 must be final type */
int unify(Type_ptr t1, Type_ptr t2)
{
	t1 = simply(t1);
	t2 = simply(t2);

	if (t1 == NULL || t2 == NULL) {
		printf("null type occur! typing error!\n");
		return 0;
	} else {
		switch (t1->kind) {
			case Int: {
				if (t2->kind == Int) {
					return 1;
				} else if (t2->kind == Typevar) {
					unify_leaf(t2, t1);
					return 1;
				} else {
					print_error(t1, t2);
					return 0;
				}
			}
			case Typevar: {
				if (t2->kind == Int) {
					unify_leaf(t1, t2);
					return 1;
				} else if (t2->kind == Typevar) {
					unify_leaf(t1, t2);
					return 1;
				} else {
					if (is_occur_node(t1->index, t2)) {
						print_error(t1, t2);
						return 0;
					} else {
						unify_leaf_arrow(t1, t2);
						return 1;
					}
				}
			}
			case Arrow: {
				if (t2->kind == Int) {
					print_error(t1, t2);
					return 0;
				} else if (t2->kind == Typevar) {
					if (is_occur_node(t2->index, t1)) {
						print_error(t1, t2);
						return 0;
					} else {
						unify_leaf_arrow(t2, t1);
						return 1;
					}
				} else {
					return unify(get_left(t1), get_left(t2)) && unify(get_right(t1), get_right(t2));
				}
			}
		}
		return 1;
	}
}

/* print type with index as type variable */
void print_type_debug(Type_ptr tree)
{
	if (tree == NULL) {
		printf ("NULL"); 
		return; 
	}
	switch (tree->kind) {
		case Int:
			printf("int");
			return;
		case Typevar: {
			printf("%d", tree->index);		// De Brujin index
			return;
		}
		case Arrow:
			printf("(");
			print_type_debug(tree->left);	// for arrow, it's rec
			printf (" -> ");
			print_type_debug(tree->right);
			printf(")");
			return;
	}  
}

/* debug print type_env[] */
void print_env(void)
{
	int i;

	printf("type_env[] = [");
	for (i = 0; i < nindex; i++) {
		printf("%d(%d,", i, type_env[i]->redirect);
		print_type_debug(type_env[i]->type);
		printf(")");
		if (i + 1 < nindex) {
			printf(", ");		// not end, split!
		}
	}
	printf("]\n");
}

void print_abs(Var_list_ptr source)
{
	Var_list_ptr tmp = source;
	printf("abs: [");
	while (tmp != NULL) {
		print_type_debug(tmp->type_var);
		if (tmp->next != NULL) {
			printf(",");
		}
		tmp = tmp->next;
	}
	printf("]\n");
}

/* return (A -> A) -> A for Y, Z */
Type_ptr make_rec_type(void)
{
	Type_ptr any = make_vartype();
	return make_arrowtype(make_arrowtype(any, any), any);
}

int is_debug = 0;
/* post condition: the return type must be final */
Type_ptr typing (Var_list_ptr abs, AST *t, int top)
{
	Type_ptr cond, left, right, result;

	if (t == NULL) {
		return NULL;
	} else {
		switch (t->kind) {
			case CONST: {
				return make_inttype();
			}
			case VAR: {
				result = get_n_th(abs, t->value, top);
				break;
			}
			case ABS: {
				left = make_vartype();
				right = typing(add_var_list(left, list_copy(abs)), t->rchild, top + 1);
				if (left == NULL || right == NULL) {
					result = NULL;
				} else {
					result = make_arrowtype(left, right);
				}
				break;
			}
			case COND: {
				cond = typing(list_copy(abs), t->cond, top);
				left = typing(list_copy(abs), t->lchild, top);	
				right = typing(list_copy(abs), t->rchild, top);
				if (cond->kind == Int && unify(left, right)) {
					result = left;
				} else {
					result = NULL;
				}
				break;
			}
			case APP: {
				left = typing(list_copy(abs), t->lchild, top);
				if (left == NULL) {
					result = NULL;
					break;
				} else {
					right = typing(list_copy(abs), t->rchild, top);
					if (right == NULL) {
						result = NULL;
						break;
					} else {
						left = final_type(left);
						if (left != NULL) {
							switch (left->kind) {
								case Int: {
									result = NULL;
									break;
								}
								case Typevar: {
									left = get_instance(left);
									if (unify(left->left, right)) {
										result = left->right;
									} else {
										result = NULL;
									}
									break;
								}
								case Arrow: {
									if (unify(left->left, right)) {
										result = left->right;
									} else {
										result = NULL;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
		if (yyin == stdin) {
			printf("typing step %d and top = %d:\n", ++step, top);
			print_abs(abs);
			print_env();
			print_expression(t, stdout);
			printf(" |== ");
			print_type_debug(result);
			printf("\n");
		}
		free_list(abs);
		return result;
	}
}

/* to change the index to letter */
static int index_order [MAXNODE] = {0};
static int order = 0;

int find_index (int index)
{
	int i;

	for (i = 1; i < order; i ++) {
		if (index_order[i] == index) {
			return i;
		}
	}
	index_order[order] = index;

	return order ++;
}

void printtype(Type_ptr tree)
{
	if (tree == NULL) {
		printf("NULL");
		return;
	}
	tree = final_type(tree);
	switch (tree->kind) {
		case Int:
			printf("int");
			return;
		case Typevar: {
			int i = (tree->index);
			int offset = find_index(i);

			printf ("%c", 'A'+ (--offset % 26));
			offset = offset / 26;
			if (offset) {
				printf("%d", offset);
			}
			return;
		}
		case Arrow:
			printf("(");
			printtype(tree->left);		// for arrow, it's rec
			printf (" -> ");
			printtype (tree -> right);
			printf(")");
			return;
	}
}

/* generate type_env independant  type tree */
Type_ptr storetype(Type_ptr tree)
{
	if (tree == NULL) {
		return NULL;
	}
	switch (tree->kind) {
		case Int:
			return &inttype;
		case Typevar: {
			int i = final_index(tree->index);
			Type_ptr t = type_env[i]->type;
			switch (t->kind) {
				case Int:
					return &inttype;
				case Arrow:
					tree->left = t->left;
					tree->right = t->right; 
 					break;
				default: {
					int offset = find_index(i);
					Type_ptr tmp;

					if (offset == 0) {
						return &inttype;		// final is INT
					}
					tmp = (Type_ptr) smalloc(sizeof(Type));
					tmp->index = offset;
					tmp->kind = Typevar;
					tmp->left = tmp->right = NULL;
 					return tmp;
				}
			}
		}
	}
	{
		Type_ptr tmp = (Type_ptr) smalloc(sizeof(Type));
		tmp->index = 0;
		tmp->kind = Arrow;
		tmp->left = storetype(tree->left);
		tmp->right = storetype(tree->right);
		return tmp;
	} 
}

/* restore the stored type in the current env */
/* generate type_env independant  type tree */
Type_ptr restoretype(Type_ptr tree)
{
	int i;
	Type_ptr restore = type_copy(tree);

	for (i = 0; i < MAXNODE; i++) {
		index_order[i] = 0;
	}

	return restore;
}

/* generate new index of current typing env for type of 
   the declaration */
/* use the lchild store old index */
int find_old_index(int index)
{
	return index_order[index];
}

Type_ptr type_copy(Type_ptr t)
{
	Type_ptr tmp;
	if (t == NULL) {
		return NULL;
	}
	switch (t->kind) {
		case Int:
			return t;
		case Typevar: {
			int old_index = find_old_index(t->index);
			if (old_index) {
				return type_env[old_index]->type;
			}
			tmp = make_vartype();
			index_order[t->index] = tmp->index;
			return tmp;
 		}
		case Arrow:
			return make_arrowtype(type_copy(t->left), type_copy(t->right));
		default:
			return NULL;
	}
}

void new_env(void)
{
	int i;
	for (i = 1; i < nindex; i++) {
		sfree(type_env[i]->type);
		sfree(type_env[i]);
	}
	for (i = 0; i < order; i++) {
		index_order[i] = 0;
	}
	nindex = 1;
	order = 1;
	step = 0;
}

/*
> 31) -> ((30 -> (31 -> 44)) -> 44)) -> (((28 -> (28 -> 39)) -> 39) -> ((36 -> (
@m.m(@f.@n.n f(f(@f.@x.f x)))(@n.@f.@x.n f (f x))
((((((A -> B) -> (A -> B)) -> C) -> (((((A -> B) -> (A -> B)) -> C) -> (C -> D)) -> D)) -> ((((E -> F) -> (F -> G)) -> ((E -> F) -> (E -> G))) -> H)) -> H)
((((((A -> B) -> (A -> B)) -> C) -> (((((A -> B) -> (A -> B)) -> C) -> (C -> D)) -> D)) -> ((((E -> F) -> (F -> G)) -> ((E -> F) -> (E -> G))) -> H)) -> H)
((((((A -> B) -> (A -> B)) -> C) -> (((((A -> B) -> (A -> B)) -> C) -> (C -> D)) -> D)) -> ((((E -> F) -> (F -> G)) -> ((E -> F) -> (E -> G))) -> H)) -> H)
((((((A -> B) -> (A -> B)) -> C) -> (((((A -> B) -> (A -> B)) -> C) -> (C -> D)) -> D))E -> ((((F -> G) -> (G -> H)) -> ((F -> G) -> (F -> H)))I -> J)) -> J)
((((((A -> B) -> (A -> B)) -> C) -> (((((A -> B) -> (A -> B)) -> C) -> (C -> D)) -> D)) -> ((((E -> F) -> (F -> G)) -> ((E -> F) -> (E -> G))) -> H)) -> H)

*/

