%{
/* the grammar of lambda expression */
#include "tree.h"

void yyerror (char const *);
/************************************/

char *name_env[MAX_ENV] = {"+", "-", "*", "/", "=", "<"};

AST *ast_env[MAX_ENV];

int current = INIT_POS;

#define YYSTYPE AST *
FILE *texfile;
int is_decl = 0;
%}
%nonassoc '.'
%left THEN ELSE
%left INT LET ID IF FI '(' '@'
%left CONCAT
%%

lines : decl 

| lines decl 
;
 
decl : LET {is_decl = 1;} ID '=' expr ';' {
    name_env[current] = (char *) $3 -> lchild;
    ast_env[current++] = $5;
  }

|  expr ';' {
  print_expression($1, stdout);
  printtree($1);
  free_ast($1);
  printf("\nplease input a lambda term with \";\":\n");  
  }
;

expr : INT 

| ID {
  /* TODO */
 }

| IF expr THEN expr ELSE expr FI { /* TODO */ } 

| '(' expr ')' { /* TODO */ }

| '@' ID  { /* TODO (midaction)*/ } '.'  expr %prec THEN { 
  /* TODO */
  } 

| expr expr %prec CONCAT   {/* TODO */ }
;

%%

extern FILE * yyin;

void yyerror(char const *s)
{
	printf ("%s!\n", s);
}

int main ()
{
	if ((texfile = fopen("./test/expr.tex", "w")) == NULL) {
		exit(1);
	}

	printf("please input a lambda term with \";\":\n");  

	yyparse ();
	fclose(texfile);
	return 0;
}
