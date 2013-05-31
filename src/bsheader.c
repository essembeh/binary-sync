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
#include "common.h"

BSHeader* newHeader(BSType type, uint64_t totalSize, uint64_t blockSize, char* pUserData) {
	BSHeader* pOut = NULL;

	pOut = malloc(sizeof(BSHeader));
	pOut->version = 1;
	pOut->type = type;
	pOut->hashFunction = ADLER32;
	pOut->totalSize = totalSize;
	pOut->blockSize = blockSize;
	updateUserData(pOut, pUserData);

	return pOut;
}

BSHeader* deleteHeader(BSHeader* pHeader) {
	if (pHeader != NULL) {
		autofree(pHeader->pUserData);
		autofree(pHeader);
	}
	return NULL;
}

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
	if (pHeader->userDataLength > 0 && fwrite(pHeader->pUserData, pHeader->userDataLength, 1, output) != 1) {
		return 11;
	}
	return 0;
}


void updateUserData(BSHeader* pHeader, char* pUserData) {
	if (pHeader != NULL) {
		autofree(pHeader->pUserData);
		if (pUserData == NULL) {
			pHeader->userDataLength = 0;
		} else {
			pHeader->userDataLength = strlen(pUserData) + 1;
			pHeader->pUserData = malloc(pHeader->userDataLength);
			bzero(pHeader->pUserData, pHeader->userDataLength);
			memcpy(pHeader->pUserData, pUserData, strlen(pUserData));
		}
	}
}

void printHeaderInformation(BSHeader* pHeader, BOOL printUserDataAsString) {
	if (pHeader != NULL ) {
		printf("\t%16s: %"PRIu8"\n", "Version", pHeader->version);
		switch (pHeader->type) {
		case CHECKSUM:
			printf("\t%16s: CHECKSUM\n", "Type");
			break;
		case REQUEST:
			printf("\t%16s: REQUEST\n", "Type");
			break;
		case DATA:
			printf("\t%16s: DATA\n", "Type");
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
		printf("\t%16s: %"PRIu64"\n", "Total size", pHeader->totalSize);
		printf("\t%16s: %"PRIu64"\n", "Block size", pHeader->blockSize);
		printf("\t%16s: %"PRIu32"\n", "User data length",
				pHeader->userDataLength);
		if (pHeader->userDataLength > 0 && printUserDataAsString == TRUE) {
			char* string = malloc(pHeader->userDataLength + 1);
			memcpy(string, pHeader->pUserData, pHeader->userDataLength);
			string[pHeader->userDataLength] = '\0';
			printf("\t%16s: %s\n", "User data", string);
		}
	}
}
