#include <stdio.h>
#include <stdlib.h>
#include "sha256.h"

int main(int argc, char *argv[])
{
	FILE *fp;
	if (argc >= 2) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			fprintf(stderr, "fopen: ");
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	} else {
		fp = stdin;
	}
	sha256(fp);
	print_digest();
	exit(EXIT_SUCCESS);
}
