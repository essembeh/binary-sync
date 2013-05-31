/*
 ============================================================================
 Name        : binary-sync.c
 Author      : Sébastien M-B
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "common.h"
#include "functions.h"

int test (int a) {
	return a;
}

int main(int argc, char** argv) {
	//return bs_info(argc, argv);
	return bs_checksum(argc, argv);

	return EXIT_SUCCESS;
}
