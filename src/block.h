#ifndef BLOCK_H_
#define BLOCK_H_

#include <inttypes.h>

uint32_t getChecksum(void* pData, size_t size, BSHeader* pHeader);
uint64_t getBlockCount(BSHeader* pHeader);
uint64_t getLastBlockSize(BSHeader* pHeader);
void printProgress(uint64_t current, uint64_t total, char* message);

#endif /* BLOCK_H_ */
