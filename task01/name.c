/* name.c	XL reg distributor */
#include <stdio.h>
#include <stdlib.h>

extern int yylineno;

char *Names[] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
				 "t8", "t9", "t10", "t11", "t12", "t13", "t14", "t15"};
char **Namep = Names;		/* init as the begin addr of Names */

char *newname()
{
	if(Namep >= &Names[sizeof(Names)/sizeof(*Names)]) {
		/* If current Namep >= the last (char *) pointer,
		 * just print error info(don't have free names).
		 */
		fprintf(stderr, "%d: Expression too complex\n", yylineno);
		exit(1);
	}

	return (*Namep++);		/* Namep always points to the next available name */
}

freename(s)
char *s;
{
	if(Namep > Names) {
		/* we can write *Namep? seems a little bit wired */
		*--Namep = s;		/* Namep always points to the next available name */
	} else {
    	fprintf(stderr, "%d: (Internal error) Name stack underflow\n", yylineno);
	}
}

