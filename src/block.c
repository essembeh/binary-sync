#include <stdio.h>
#include <stdlib.h>

#include "bsheader.h"
#include "block.h"
#include "adler32.h"

uint32_t getChecksum(void* pData, size_t size, BSHeader* pHeader) {
	uint32_t out = 0;
	switch (pHeader->hashFunction) {
	case ADLER32:
		out = adler32(0, pData, size);
		break;
	default:
		perror("Invalid hash function\n");
		exit(1);
	}
	return out;
}

uint64_t getBlockCount(BSHeader* pHeader) {
	return (pHeader->totalSize + pHeader->blockSize - 1) / pHeader->blockSize;
}

uint64_t getLastBlockSize(BSHeader* pHeader) {
	uint64_t out = pHeader->totalSize % pHeader->blockSize;
	return out == 0 ? pHeader->blockSize : out;
}
