/*
 * header.c
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bsheader.h"
#include "common.h"


RETURN_CODE initHeader(BSHeader* pHeader,
					   BSType type,
					   uint64_t totalSize,
					   uint64_t blockSize,
					   char* pUserdata) {
	// Check
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	ASSERT_RETURN(blockSize <= totalSize, ILLEGAL_ARG);

	// CLEAN
	memset(pHeader, 0, HEADER_LEN);

	// Init
	pHeader->version = 1;
	pHeader->type = type;
	pHeader->totalSize = totalSize;
	pHeader->blockSize = blockSize;
	writeTag(pHeader, NULL);
	updateUserData(pHeader, pUserdata);

	return NO_ERROR;
}

RETURN_CODE updateUserData(BSHeader* pHeader, char* pUserData) {
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	memset(pHeader->pUserData, 0, USER_DATA_LEN);
	if (pUserData != NULL && strlen(pUserData) < USER_DATA_LEN) {
		memcpy(pHeader->pUserData, pUserData, strlen(pUserData));
	}
	return NO_ERROR;
}

RETURN_CODE initFooter(BSFooter* pFooter,
					   uint64_t itemCount) {
	// Check
	CHECK_PTR_RETURN(pFooter, ILLEGAL_ARG);

	// CLEAN
	memset(pFooter, 0, FOOTER_LEN);

	// Init
	writeTag(NULL, pFooter);
	pFooter->itemCount = itemCount;

	return NO_ERROR;
}

RETURN_CODE readHeaderFooter(FILE* input,
							 BSHeader* pHeader,
							 BSFooter* pFooter) {
	// Check
	CHECK_PTR_RETURN(input, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pFooter, ILLEGAL_ARG);

	RETURN_CODE rc;

	uint64_t fileSize;
	if ((rc = getFileSize(input, &fileSize)) != NO_ERROR) {
		return rc;
	}
	ASSERT_RETURN(fileSize >= HEADER_LEN + FOOTER_LEN, BS_INVALID_SIZE);

	CHECK_RC_RETURN(fseek(input, 0, SEEK_SET), SEEK_ERROR);
	if ((rc = fread(pHeader, HEADER_LEN, 1, input)) != 1) {
		return READ_ERROR;
	}
	CHECK_RC_RETURN(fseek(input, -FOOTER_LEN, SEEK_END), SEEK_ERROR);
	if ((rc = fread(pFooter, FOOTER_LEN, 1, input)) != 1) {
		return READ_ERROR;
	}

	if ((rc = checkTag(pHeader, pFooter)) != NO_ERROR) {
		return rc;
	}
	ASSERT_RETURN(pHeader->type == DATA || pHeader->type == CHECKSUM, BS_INVALID_HEADER);
	ASSERT_RETURN(pHeader->version == 1, BS_INVALID_HEADER);

	uint64_t payloadSize = fileSize - HEADER_LEN - FOOTER_LEN;
	uint64_t blockCount = getBlockCount(pHeader);
	ASSERT_RETURN(blockCount > 0, BS_INVALID_BLOCK_COUNT);
	if (pHeader->type == CHECKSUM) {
		ASSERT_RETURN(pFooter->itemCount == blockCount, BS_INVALID_ITEMCOUNT);
		ASSERT_RETURN(payloadSize == getItemSize(pHeader) * blockCount, BS_INVALID_SIZE);
	} else if (pHeader->type == DATA) {
		ASSERT_RETURN(pFooter->itemCount <= blockCount, BS_INVALID_ITEMCOUNT);
		ASSERT_RETURN(payloadSize == getItemSize(pHeader) * pFooter->itemCount, BS_INVALID_SIZE);
	}
	return NO_ERROR;
}

RETURN_CODE writeHeader(FILE* output, BSHeader* pHeader) {
	CHECK_PTR_RETURN(output, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	if (fwrite(pHeader, sizeof(BSHeader), 1, output) != 1) {
		return WRITE_ERROR;
	}
	return NO_ERROR;
}

RETURN_CODE writeFooter(FILE* output, BSFooter* pFooter) {
	CHECK_PTR_RETURN(output, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pFooter, ILLEGAL_ARG);
	if (fwrite(pFooter, sizeof(BSFooter), 1, output) != 1) {
		return WRITE_ERROR;
	}
	return NO_ERROR;
}

RETURN_CODE checkTag(BSHeader* pHeader, BSFooter* pFooter) {
	if (pHeader != NULL) {
		if (pHeader->tag[0] != 'B' ||
			pHeader->tag[1] != 'S') {
			return BS_INVALID_TAG;
		}
	}
	if (pFooter != NULL) {
		if (pFooter->tag[0] != 'B' ||
			pFooter->tag[1] != 'S') {
			return BS_INVALID_TAG;
		}
	}
	return NO_ERROR;
}

RETURN_CODE writeTag(BSHeader* pHeader, BSFooter* pFooter) {
	if (pHeader != NULL) {
		pHeader->tag[0] = 'B';
		pHeader->tag[1] = 'S';
	}
	if (pFooter != NULL) {
		pFooter->tag[0] = 'B';
		pFooter->tag[1] = 'S';
	}
	return NO_ERROR;
}

RETURN_CODE fseekAfterHeader(FILE* input) {
	CHECK_PTR_RETURN(input, ILLEGAL_ARG);
	CHECK_RC_RETURN(fseek(input, HEADER_LEN, SEEK_SET), SEEK_ERROR);
	return NO_ERROR;
}

void printHeaderInformation(BSHeader* pHeader, bool printUserDataAsString) {
	if (pHeader != NULL) {
		printf("\t%16s: %lu\n", "Size of header", HEADER_LEN);
		printf("\t%16s: %"PRIu8"\n", "Version", pHeader->version);
		switch (pHeader->type) {
		case CHECKSUM:
			printf("\t%16s: CHECKSUM\n", "Type");
			break;
		case DATA:
			printf("\t%16s: DATA\n", "Type");
			break;
		default:
			printf("\tType: UNKNOWN\n");
		}
		printf("\t%16s: %"PRIu64"\n", "Total size", pHeader->totalSize);
		printf("\t%16s: %"PRIu64"\n", "Block size", pHeader->blockSize);
		printf("\t%16s: %"PRIu64"\n", "Block count", getBlockCount(pHeader));
		printf("\t%16s: %"PRIu64"\n", "Item size", getItemSize(pHeader));
		if (pHeader->pUserData != NULL && printUserDataAsString == true) {
			printf("\t%16s: %d\n", "User data length", USER_DATA_LEN);
			printf("\t%16s: %s\n", "User data", pHeader->pUserData);
		}
	}
}

void printFooterInformation(BSFooter* pFooter) {
	printf("\t%16s: %lu\n", "Size of footer", FOOTER_LEN);
	printf("\t%16s: %"PRIu64"\n", "Item count", pFooter->itemCount);
}

uint64_t getItemSize(BSHeader* pHeader) {
	if (pHeader->type == DATA) {
		return sizeof(uint64_t) + pHeader->blockSize;
	} else if (pHeader->type == CHECKSUM) {
		return sizeof(uint32_t);
	}
	return 0;
}

uint64_t getBlockCount(BSHeader* pHeader) {
	return (pHeader->totalSize + pHeader->blockSize - 1) / pHeader->blockSize;
}

uint64_t getLastBlockSize(BSHeader* pHeader) {
	uint64_t out = pHeader->totalSize % pHeader->blockSize;
	return out == 0 ? pHeader->blockSize : out;
}

RETURN_CODE readBlock(FILE* pSource, BSHeader* pHeader, uint64_t blockId, void* pBuffer) {
	CHECK_PTR_RETURN(pSource, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pBuffer, ILLEGAL_ARG);
	memset(pBuffer, 0, pHeader->blockSize);
	uint64_t payloadSize = pHeader->blockSize;
	if (blockId == (getBlockCount(pHeader) - 1)) {
		payloadSize = getLastBlockSize(pHeader);
	}
	CHECK_RC_RETURN(fseek(pSource, blockId * pHeader->blockSize, SEEK_SET), SEEK_ERROR);
	if (fread(pBuffer, payloadSize, 1, pSource) != 1) {
		return READ_ERROR;
	}
	return NO_ERROR;
}

RETURN_CODE getExpectedPayloadSize(BSHeader* pHeader,
								   BSFooter* pFooter,
								   uint64_t* pOutSize) {
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pFooter, ILLEGAL_ARG);

	uint64_t blockCount = getBlockCount(pHeader);
	ASSERT_RETURN(blockCount > 0, BS_INVALID_BLOCK_COUNT);

	*pOutSize = 0;
	if (pHeader->type == CHECKSUM) {

	} else if (pHeader->type == DATA) {

	} else {
		return ILLEGAL_STATE;
	}
	return NO_ERROR;
}
