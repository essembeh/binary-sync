#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"
#include "adler32.h"
#include "block.h"

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

	printf("Parse arguments");

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

	int rc = 0;
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

	// Open target
	if ((pTargetFile = fopen(pTargetFilename, "r")) == NULL ) {
		THROW("Error opening target", OPEN_ERROR);
	}

	// read size
	fseek(pTargetFile, 0L, SEEK_END);
	uint64_t totalSize = ftell(pTargetFile);
	fseek(pTargetFile, 0L, SEEK_SET);

	// Create header
	pHeader = newHeader(CHECKSUM, totalSize, blockSize, pUserData);
	printHeaderInformation(pHeader, TRUE);

	// Open checksum file
	if ((pOutputFile = fopen(pOutputFilename, "w")) == NULL ) {
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
	while (1) {
		size_t len = fread(buffer, 1, blockSize, pTargetFile);
		if (len <= 0) {
			break;
		}
		printProgress(++currentBlockId, blockCount, "Checksum");
		checksum = getChecksum(buffer, len);
		if (fwrite(&checksum, sizeof(uint32_t), 1, pOutputFile) != 1) {
			THROW("Error writing checksum", WRITE_ERROR)
		}
	}
	printf("Checksum complete\n");

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
