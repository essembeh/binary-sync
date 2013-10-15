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
			printf("\n");
			printf("Header information of file: %s\n", filename);
			printHeaderInformation(pHeader, 1);

			if (pHeader->type == CHECKSUM) {
				uint64_t filesize;
				if (getFileSize(file, &filesize) != NO_ERROR) {
					printf("Error reading file size\n");
					continue;
				}
				uint64_t payload = filesize - HEADER_LEN;
				printf("\t%16s: %"PRIu64"\n", "Payload size", payload);
				uint64_t items = payload / sizeof(uint32_t);
				uint64_t zero = payload % sizeof(uint32_t);
				if (zero != 0) {
					printf("Error file size is not valid\n");
					continue;
				}
				printf("\t%16s: %"PRIu64"\n", "Checksum count", items);
				if (items != getBlockCount(pHeader)) {
					printf("Error Checksum count is different from Block count\n");
					continue;
				}

			} else if (pHeader->type == DATA) {

			}
		} else {
			printf("Cannot open file: %s\n", filename);
		}
		AUTOFREE(pHeader);
		AUTOCLOSE(file);
	}
	return NO_ERROR;
}

int main(int argc, char** argv) {
	printf("binary-sync: info\n");
	return bs_info(argc, argv);
}
