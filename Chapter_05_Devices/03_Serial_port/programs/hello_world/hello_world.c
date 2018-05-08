/*! Hello world program */

#include <stdio.h>
#include <malloc.h>
#include <api/prog_info.h>
#define INC	32

int hello_world ()
{
	printf ( "Test program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__, "allocating memory");
	int *A = malloc(INC * 16 + 10);
	int *B = malloc(INC * 64 + 100);
	int *C = malloc(INC * 128 + 1000);
	int *D = malloc(INC * 256 + 10000);


	free(A);
	A = malloc(INC * 16 + 10);
	free(C);
	free(B);
	free(D);
	free(A);
	/*printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,*/
		 /*hello_world_PROG_HELP );*/

	/*printf ( "Hello World!\n" );*/

#if 0	/* test escape sequence */
	printf ( "\x1b[20;40H" "Hello World at 40, 20!\n" );
#endif

	return 0;
}
