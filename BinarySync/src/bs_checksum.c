#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"

RETURN_CODE parse_args(int argc, char** argv,
					   uint64_t* pBlockSize,
					   char** ppTargetFilename,
					   char** ppOutputFilename,
					   char** ppUserdata) {
	// Default values
	*ppTargetFilename = NULL;
	*ppOutputFilename = NULL;
	*ppUserdata = NULL;
	*pBlockSize = DEFAULT_BLOCK_SIZE;

	printf("Parse arguments\n");
	opterr = 0;
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
				{ "block-size", required_argument, 0, 'b' },
				{ "target", 	required_argument, 0, 't' },
				{ "output", 	required_argument, 0, 'o' },
				{ "user-data", 	required_argument, 0, 'u' },
				{ 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "b:t:o:u:", long_options, &option_index);
		if (c == -1) { break; }
		switch (c) {
		case 'b':
			sscanf(optarg, "%"PRIu64, pBlockSize);
			printf("-- Using block size: %"PRIu64"\n", *pBlockSize);
			break;
		case 't':
			*ppTargetFilename = optarg;
			printf("-- Target: %s\n", *ppTargetFilename);
			break;
		case 'o':
			*ppOutputFilename = optarg;
			printf("-- Output checksum file: %s\n", *ppOutputFilename);
			break;
		case 'u':
			*ppUserdata = optarg;
			printf("-- User data: %s\n", *ppUserdata);
			break;
		default:
			printf("-- Invalid option\n");
		}
	}

	CHECK_PTR_RETURN(*ppOutputFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(*ppTargetFilename, ILLEGAL_ARG);

	return NO_ERROR;
}

RETURN_CODE bs_checksum(int argc, char** argv) {

	RETURN_CODE rc = 0;
	char* pTargetFilename = NULL;
	char* pOutputFilename = NULL;
	char* pUserData = NULL;
	FILE* pTargetFile = NULL;
	FILE* pOutputFile = NULL;
	void* pBuffer = NULL;

	BSHeader header;
	BSFooter footer;

TRY
	uint64_t blockSize = 0;
	CHECK_RC_THROW((rc = parse_args(argc, argv,
									&blockSize,
									&pTargetFilename,
									&pOutputFilename,
									&pUserData)), "Invalid arguments", rc);

	ASSERT_THROW((blockSize > 0), "Block size must be a positive value");

	// Open target
	if ((pTargetFile = fopen(pTargetFilename, "rb")) == NULL ) {
		THROW("Error opening target", OPEN_ERROR);
	}

	// read size
	uint64_t totalSize;
	if ((rc = getFileSize(pTargetFile, &totalSize)) != NO_ERROR) {
		THROW("Error getting file size", rc);
	}

	// Create header
	initHeader(&header, CHECKSUM, totalSize, blockSize, pUserData);
	printf("Output header\n");
	printHeaderInformation(&header, true);

	// Open checksum file
	if ((pOutputFile = fopen(pOutputFilename, "wb")) == NULL ) {
		THROW("Error opening checksum", OPEN_ERROR);
	}

	// Write header
	if ((rc = writeHeader(pOutputFile, &header)) != 0) {
		THROW("Error writing checksum", rc);
	}

	uint64_t blockCount = getBlockCount(&header);

	printf("Total block count: %"PRIu64"\n", blockCount);

	// Read source file
	pBuffer = malloc(blockSize);
	uint32_t checksum = 0;
	for(uint64_t currentBlockId = 0; currentBlockId < blockCount; currentBlockId++) {
		printProgress(currentBlockId, blockCount, "Computing checksum");
		if ((rc = readBlock(pTargetFile, &header, currentBlockId, pBuffer)) != NO_ERROR) {
			printf("Error reading block %"PRIu64"\n", currentBlockId);
			THROW("Error reading block from target", rc);
		}
		checksum = getChecksum(pBuffer, header.blockSize);
		if (fwrite(&checksum, sizeof(uint32_t), 1, pOutputFile) != 1) {
			THROW("Error writing checksum", WRITE_ERROR)
		}
	}
	initFooter(&footer, blockCount);
	if (fwrite(&footer, FOOTER_LEN, 1, pOutputFile) != 1) {
		THROW("Error writing footer", WRITE_ERROR)
	}

CATCH

FINALLY
	AUTOFREE(pBuffer);
	AUTOCLOSE(pOutputFile);
	AUTOCLOSE(pTargetFile);

	return exceptionId;
}


int main(int argc, char** argv) {
	printf("binary-sync: checksum\n");
	return bs_checksum(argc, argv);
}
