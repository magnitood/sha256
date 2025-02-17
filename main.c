#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "sha256.h"

void handle_error()
{
	switch(errno) {
		case EACCES:
			perror("Cannot open file\n");
			break;
		default:
			perror("Error\n");
			break;
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	if (argc >= 2) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			handle_error();
			exit(EXIT_FAILURE);
		}
	} else {
		fp = stdin;
	}
	sha256(fp);
	print_digest();
	exit(EXIT_SUCCESS);
}
