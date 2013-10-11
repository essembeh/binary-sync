#include <stdio.h>
#include <stdlib.h>

#include "block.h"
#include "adler32.h"

uint32_t getChecksum(void* pData, size_t size) {
	uint32_t out = adler32(0, pData, size);
	return out;
}


void printProgress(uint64_t current, uint64_t total, char* message) {
	int last = current == 1 ? -1 : ((current - 1) * 100 / total);
	int percent = current * 100 / total;
	if (last != percent) {
		printf("%s %d%%\n", message, percent);
		printf("\033[F\033[J");
	}
}
