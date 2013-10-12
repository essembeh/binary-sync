#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "common.h"


uint64_t getFileSize(FILE* pFile) {
	uint64_t out = 0;

	if (pFile != NULL) {
		fseek(pFile, 0, SEEK_END);
		out = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
	}
	return out;
}
