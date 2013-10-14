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
	BSHeader* pHeader = NULL;
	void* buffer = NULL;

TRY

	uint64_t blockSize = 0;
	CHECK_RC_THROW((rc = parse_args(argc, argv,
									&blockSize,
									&pTargetFilename,
									&pOutputFilename,
									&pUserData)), "Invalid arguments", rc);
	ASSERT((blockSize > 0), "Block size must be a positive value");

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
	pHeader = newHeader(CHECKSUM, totalSize, blockSize, pUserData);
	printf("Output header\n");
	printHeaderInformation(pHeader, TRUE);

	// Open checksum file
	if ((pOutputFile = fopen(pOutputFilename, "wb")) == NULL ) {
		THROW("Error opening checksum", OPEN_ERROR);
	}

	// Write header
	if ((rc = writeHeader(pOutputFile, pHeader)) != 0) {
		THROW("Error writing checksum", rc);
	}

	uint64_t blockCount = getBlockCount(pHeader);
	uint64_t lastBlockSize = getLastBlockSize(pHeader);

	printf("Total block count: %"PRIu64", last block size: %"PRIu64"\n", blockCount, lastBlockSize);

	// Read source file
	buffer = malloc(blockSize);
	uint64_t currentBlockId = 0;
	uint32_t checksum = 0;
	for(currentBlockId = 0; currentBlockId < blockCount; currentBlockId++) {
		printProgress(++currentBlockId, blockCount, "Computing checksum");
		uint64_t bufferSize = currentBlockId == (blockCount - 1) ? lastBlockSize : blockSize;
		if ((rc = fread(buffer, bufferSize, 1, pTargetFile)) != 1) {
			THROW("Error reading target file", rc);
		}
		checksum = getChecksum(buffer, bufferSize);
		if (fwrite(&checksum, sizeof(uint32_t), 1, pOutputFile) != 1) {
			THROW("Error writing checksum", WRITE_ERROR)
		}
	}

CATCH

FINALLY

	AUTOFREE(buffer);
	AUTOFREE(pHeader);
	AUTOCLOSE(pOutputFile);
	AUTOCLOSE(pTargetFile);

	return exceptionId;
}


int main(int argc, char** argv) {
	printf("binary-sync: checksum\n");
	return bs_checksum(argc, argv);
}
