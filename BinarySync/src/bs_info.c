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
	BSHeader header;
	BSFooter footer;
	for (int i = 1; i < argc; i++) {
		char* filename = argv[i];
		FILE* file = fopen(filename, "r");
		if (file != NULL) {
			RETURN_CODE rc = readHeaderFooter(file, &header, &footer);
			if (rc != NO_ERROR) {
				printf("%s: invalid file, error: %d\n", filename, rc);
			} else {
				printf("Header information of file: %s\n", filename);
				printHeaderInformation(&header, true);
				printFooterInformation(&footer);
			}
		} else {
			printf("Cannot open file: %s\n", filename);
		}
		printf("\n");
		AUTOCLOSE(file);
	}
	return NO_ERROR;
}

int main(int argc, char** argv) {
	printf("binary-sync: info\n");
	return bs_info(argc, argv);
}
