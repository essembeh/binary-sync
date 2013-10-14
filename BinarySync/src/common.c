#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <zlib.h>

#include "common.h"


RETURN_CODE getFileSize(FILE* pFile, uint64_t* opSize) {
	*opSize = 0;
	CHECK_PTR_RETURN(pFile, ILLEGAL_ARG);
	uint64_t currentPos = ftell(pFile);
	if (fseek(pFile, 0, SEEK_END) != 0) {
		return SEEK_ERROR;
	}
	*opSize = ftell(pFile);
	if (fseek(pFile, currentPos, SEEK_SET) != 0) {
		return SEEK_ERROR;
	}
	return NO_ERROR;
}

void printProgress(uint64_t current, uint64_t total, char* message) {
	if (current == total) {
		printf("%s: done\n", message);
	} else {
		uint64_t currentValue = current * 100 / total;
		if (current == 0 || ((current - 1 ) * 100 / total) != currentValue) {
			printf("%s: %"PRIu64"%%\n", message, currentValue);
			printf("\033[F\033[J");
		}
	}
}

uint32_t getChecksum(void* pData, size_t size) {
	uint32_t out = adler32(0, pData, size);
	return out;
}
