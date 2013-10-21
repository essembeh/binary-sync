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
#include <stdbool.h>

#include "error.h"
#include "common.h"


#define DEFAULT_BLOCK_SIZE 1048576
#define USER_DATA_LEN 32

typedef enum {
	CHECKSUM = 'C', DATA = 'D'
} BSType;

typedef struct {
	char tag[2];
	uint8_t version;
	uint8_t type;
	uint64_t totalSize;
	uint64_t blockSize;
	char pUserData[USER_DATA_LEN];
} BSHeader;

typedef struct {
	uint64_t itemCount;
	char tag[2];
} BSFooter;

RETURN_CODE initHeader(BSHeader* pHeader, BSType type, uint64_t totalSize, uint64_t blockSize, char* pUserdata);
RETURN_CODE updateUserData(BSHeader* pHeader, char* pUserData);
RETURN_CODE initFooter(BSFooter* pFooter, uint64_t itemCount);
RETURN_CODE readHeaderFooter(FILE* input, BSHeader* pHeader, BSFooter* pFooter);
RETURN_CODE writeHeader(FILE* output, BSHeader* pHeader);
RETURN_CODE writeFooter(FILE* output, BSFooter* pFooter);
RETURN_CODE checkTag(BSHeader* pHeader, BSFooter* pFooter);
RETURN_CODE writeTag(BSHeader* pHeader, BSFooter* pFooter);
RETURN_CODE fseekAfterHeader(FILE* input);

void printHeaderInformation(BSHeader* pHeader, bool printUserDataAsString);
void printFooterInformation(BSFooter* pFooter);

uint64_t getBlockCount(BSHeader* pHeader);
uint64_t getItemSize(BSHeader* pHeader);
uint64_t getLastBlockSize(BSHeader* pHeader);
RETURN_CODE readBlock(FILE* pSource, BSHeader* pHeader, uint64_t blockId, void* pBuffer);
RETURN_CODE getExpectedPayloadSize(BSHeader* pHeader, BSFooter* pFooter, uint64_t* pOutSize);

#define HEADER_LEN  sizeof(BSHeader)
#define FOOTER_LEN  sizeof(BSFooter)

#endif /* BSHEADER_H_ */
