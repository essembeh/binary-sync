/*
 * header.c
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bsheader.h"

BSHeader* readHeader(FILE* input) {
	BSHeader* pOut = NULL;
	if (input != NULL ) {
		pOut = malloc(sizeof(BSHeader));
		// Read header without user data
		if (fread(pOut, HEADER_SIZE_WITHOUT_USER_DATA, 1, input) != 1) {
			autofree(pOut);
			return NULL ;
		}
		if (pOut->userDataLength > 0) {
			pOut->pUserData = malloc(pOut->userDataLength);
			if (fread(pOut->pUserData, pOut->userDataLength, 1, input) != 1) {
				autofree(pOut->pUserData);
				autofree(pOut);
				return NULL ;
			}
		} else {
			pOut->pUserData = NULL;
			pOut->userDataLength = 0;
		}
	}
	return pOut;
}

int writeHeader(FILE* output, BSHeader* pHeader) {
	if (output == NULL || pHeader == NULL ) {
		return 1;
	}
	if (fwrite(pHeader, HEADER_SIZE_WITHOUT_USER_DATA, 1, output) != 1) {
		return 10;
	}
	if (fwrite(pHeader->pUserData, pHeader->userDataLength, 1, output) != 1) {
		return 11;
	}
	return 0;
}

void printHeaderInformation(BSHeader* pHeader, char printUserDataAsString) {
	if (pHeader != NULL ) {
		printf("\t%16s: %u\n", "Version", pHeader->version);
		switch (pHeader->type) {
		case CHECKSUM_REQUEST:
			printf("\t%16s: CHECKSUM_REQUEST\n", "Type");
			break;
		case CHECKSUM_FILE:
			printf("\t%16s: CHECKSUM_FILE\n", "Type");
			break;
		case DATA_REQUEST:
			printf("\t%16s: DATA_REQUEST\n", "Type");
			break;
		case DATA_FILE:
			printf("\t%16s: DATA_FILE\n", "Type");
			break;
		default:
			printf("\tType: UNKNOWN\n");
		}
		switch (pHeader->hashFunction) {
		case ADLER32:
			printf("\t%16s: ADLER32\n", "Hash function");
			break;
		case CRC32:
			printf("\t%16s: CRC32\n", "Hash function");
			break;
		default:
			printf("\tHash function: UNKNOWN\n");
		}
		printf("\t%16s: %u\n", "Total size", pHeader->totalSize);
		printf("\t%16s: %u\n", "Block size", pHeader->blockSize);
		printf("\t%16s: %u\n", "User data length", pHeader->userDataLength);
		if (pHeader->userDataLength > 0 && printUserDataAsString > 0) {
			char* string = malloc(pHeader->userDataLength + 1);
			memcpy(string, pHeader->pUserData, pHeader->userDataLength);
			string[pHeader->userDataLength] = '\0';
			printf("\t%16s: %s\n", "User data", string);
		}
	}
}
