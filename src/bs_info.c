/*
 * bs_info.c
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "bsheader.h"

int bs_info(int argc, char** argv) {
	int i;
	for (i = 1; i < argc; i++) {
		char* filename = argv[i];
		FILE* file = fopen(filename, "r");
		BSHeader* pHeader = readHeader(file);
		if (pHeader != NULL) {
			printf("Header information of file: %s\n", filename);
			printHeaderInformation(pHeader, 1);
		} else {
			printf("Cannot open file: %s\n", filename);
		}
		autofree(pHeader);
		if (file != NULL ) {
			fclose(file);
		}
	}
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	return bs_info(argc, argv);
}
