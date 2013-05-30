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

#define autofree(a) if (a != NULL) { free(a); a = NULL; }

typedef enum {
	CHECKSUM_REQUEST = 10, CHECKSUM_FILE = 20, DATA_REQUEST = 30, DATA_FILE = 40
} BSType;

typedef enum {
	ADLER32 = 'A', CRC32 = 'C'
} BSHashFunction;

typedef struct {
	uint8_t version;
	uint8_t type;
	uint8_t hashFunction;
	uint32_t totalSize;
	uint32_t blockSize;
	uint32_t userDataLength;
	void* pUserData;
} BSHeader;

#define HEADER_SIZE_WITHOUT_USER_DATA sizeof(BSHeader) - 1

BSHeader* readHeader(FILE* input);
int writeHeader(FILE* output, BSHeader* pHeader);
void printHeaderInformation(BSHeader* pHeader, char printUserDataAsString);

#endif /* BSHEADER_H_ */
