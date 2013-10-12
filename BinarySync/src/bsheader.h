/*
 * header.h
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */
#ifndef BSHEADER_H_
#define BSHEADER_H_

#include <inttypes.h>
#include <stdio.h>
#include "error.h"
#include "common.h"


#define DEFAULT_BLOCK_SIZE 1048576
#define USER_DATA_LEN 32

typedef enum {
	CHECKSUM = 'C', DATA = 'D'
} BSType;

typedef struct {
	uint8_t version;
	uint8_t type;
	uint64_t totalSize;
	uint64_t blockSize;
	char pUserData[USER_DATA_LEN];
} BSHeader;

BSHeader* newHeader(BSType type, uint64_t totalSize, uint64_t blockSize, char* pUserData);
BSHeader* readHeader(FILE* input);
RETURN_CODE writeHeader(FILE* output, BSHeader* pHeader);
void printHeaderInformation(BSHeader* pHeader, BOOL printUserDataAsString);
uint64_t getBlockCount(BSHeader* pHeader);
uint64_t getLastBlockSize(BSHeader* pHeader);

#endif /* BSHEADER_H_ */
