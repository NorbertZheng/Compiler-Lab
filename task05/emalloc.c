#include <stdlib.h>
#include <stdio.h>
#ifndef __TURBOC__  
#include <unistd.h>
#endif

/*
 * check whether each malloc is successful
 */
void * smalloc(size_t size)
{
	static char msg[] = "parser's ram is exhausted!\n";
	char *retval = malloc((unsigned) size);

	if (retval == 0) {
		/* fprintf might call malloc(), so... */
		write(2, msg, sizeof(msg));  /* print to stderr
		(file descriptor is 2) */
		exit(-1);
	}
	return retval;
}

void sfree(void *x)
{
	free(x);
}

