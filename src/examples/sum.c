#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
	argc = EXIT_SUCCESS;
	printf("%d ", fibo( atoi(argv[1]) ));
	printf("%d\n", sum4( atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]) ));
	//printf("%d %d\n", fibo( atoi(argv[1] )), sum4( atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])));

	return argc;
}
