/* xsh_test.c - xsh_test */

#include <xinu.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_test - Run Tests
 *------------------------------------------------------------------------
 */
shellcmd xsh_test(int nargs, char *args[])
{
	// if (nargs > 1) {
	// 	fprintf(stderr,"use is: %s\n", args[0]);
	// 	return 1;
	// }

    printf("Process Test\n");
    return 0;
}
