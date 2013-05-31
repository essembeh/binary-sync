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


#define BOOL char
#define TRUE 1
#define FALSE 0


#define DEFAULT_BLOCK_SIZE 1048576

typedef enum {
	CHECKSUM = 10, REQUEST = 20, DATA = 30
} BSType;

typedef enum {
	ADLER32 = 'A', CRC32 = 'C'
} BSHashFunction;

typedef struct {
	uint8_t version;
	uint8_t type;
	uint8_t hashFunction;
	uint64_t totalSize;
	uint64_t blockSize;
	uint32_t userDataLength;
	void* pUserData;
} BSHeader;

#define HEADER_SIZE_WITHOUT_USER_DATA sizeof(BSHeader) - 1

BSHeader* newHeader(BSType type, uint64_t totalSize, uint64_t blockSize, char* pUserData);
BSHeader* readHeader(FILE* input);
BSHeader* deleteHeader(BSHeader* pHeader);
int writeHeader(FILE* output, BSHeader* pHeader);
void updateUserData(BSHeader* pHeader, char* pUserData);
void printHeaderInformation(BSHeader* pHeader, BOOL printUserDataAsString);

#endif /* BSHEADER_H_ */
