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
	pOut->totalSize = totalSize;
	pOut->blockSize = blockSize;
	memset(pOut->pUserData, 0, USER_DATA_LEN);
	if (pUserData != NULL && strlen(pUserData) < USER_DATA_LEN) {
		memccpy(pOut->pUserData, pUserData, 1, strlen(pUserData));
	}
	return pOut;
}

BSHeader* readHeader(FILE* input) {
	BSHeader* pOut = NULL;
	if (input != NULL) {
		pOut = malloc(sizeof(BSHeader));
		// Read header without user data
		if (fread(pOut, sizeof(BSHeader), 1, input) != 1) {
			AUTOFREE(pOut);
			return NULL ;
		}
	}
	return pOut;
}

RETURN_CODE writeHeader(FILE* output, BSHeader* pHeader) {
	CHECK_PTR_RETURN(output, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	if (fwrite(pHeader, sizeof(BSHeader), 1, output) != 1) {
		return WRITE_ERROR;
	}
	return NO_ERROR;
}

RETURN_CODE updateUserData(BSHeader* pHeader, char* pUserData) {
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	memset(pHeader->pUserData, 0, USER_DATA_LEN);
	if (pUserData != NULL && strlen(pUserData) < USER_DATA_LEN) {
		memccpy(pHeader->pUserData, pUserData, 1, strlen(pUserData));
	}
	return NO_ERROR;
}

void printHeaderInformation(BSHeader* pHeader, BOOL printUserDataAsString) {
	if (pHeader != NULL ) {
		printf("\t%16s: %lu\n", "Size of header", sizeof(BSHeader));
		printf("\t%16s: %"PRIu8"\n", "Version", pHeader->version);
		switch (pHeader->type) {
		case CHECKSUM:
			printf("\t%16s: CHECKSUM\n", "Type");
			break;
		case DATA:
			printf("\t%16s: DATA\n", "Type");
			break;
		default:
			printf("\tType: UNKNOWN\n");
		}
		printf("\t%16s: %"PRIu64"\n", "Total size", pHeader->totalSize);
		printf("\t%16s: %"PRIu64"\n", "Block size", pHeader->blockSize);
		printf("\t%16s: %d\n", "User data length", USER_DATA_LEN);
		uint64_t blockCount = getBlockCount(pHeader);
		printf("\t%16s: %"PRIu64"\n", "Block count", blockCount);
		if (pHeader->pUserData != NULL && printUserDataAsString == TRUE) {
			printf("\t%16s: %s\n", "User data", pHeader->pUserData);
		}
	}
}


uint64_t getBlockCount(BSHeader* pHeader) {
	return (pHeader->totalSize + pHeader->blockSize - 1) / pHeader->blockSize;
}

uint64_t getLastBlockSize(BSHeader* pHeader) {
	uint64_t out = pHeader->totalSize % pHeader->blockSize;
	return out == 0 ? pHeader->blockSize : out;
}
