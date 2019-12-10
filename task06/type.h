#ifndef _TYPE_H
#define _TYPE_H

#define MAXNODE		256

/* struct */
typedef enum {
	Typevar	=	1,
	Arrow	=	2,
	Int		=	3
} Type_kind;

/* for type tree */
typedef struct type {
	int index;
	Type_kind kind;
	struct type *left, *right;
} Type;

typedef Type * Type_ptr;

typedef struct varlist {
	Type_ptr type_var;
	struct varlist *next;
} Var_list;

typedef Var_list * Var_list_ptr;

typedef struct type_env{
	int redirect;
	Type_ptr type;
} Type_env;

typedef Type_env * Type_env_ptr;

/* variables */
extern int is_debug;
extern Type_ptr global_type_env[MAX_ENV];

/* funcs */
Var_list_ptr  list_clone(Var_list_ptr source);
Type_ptr make_inttype(void);
Type_ptr make_vartype(void);
Type_ptr make_arrowtype(Type_ptr left, Type_ptr right);
Type_ptr get_left(Type_ptr type_tree);
Type_ptr get_right(Type_ptr type_tree);
Type_ptr get_instance(Type_ptr type_tree);
int is_occur_node(int index, Type_ptr type_tree);
int unify(Type_ptr t1, Type_ptr t2);
Type_ptr typing (Var_list_ptr abs, AST *, int);

void printtype(Type_ptr tree);
void new_env(void);
void init_type_env();
Type_ptr storetype(Type_ptr tree);
Type_ptr restoretype(Type_ptr tree);

#endif

