#ifndef BLOCK_H_
#define BLOCK_H_

#include <inttypes.h>


uint32_t getChecksum(void* pData, size_t size);
void printProgress(uint64_t current, uint64_t total, char* message);

#endif /* BLOCK_H_ */
