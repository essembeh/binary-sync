#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"
#include "adler32.h"
#include "block.h"

int parse_args(int argc, char** argv, uint64_t* pBlockSize, char** ppSource, char** ppOutput, char** ppUserdata) {
	// Default values
	*ppOutput = NULL;
	*ppSource = NULL;
	*ppUserdata = NULL;
	*pBlockSize = DEFAULT_BLOCK_SIZE;

	printf("Parse arguments");

	opterr = 0;
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = { { "block-size",
				required_argument, 0, 'b' }, { "source", required_argument, 0,
				's' }, { "output", required_argument, 0, 'o' }, { "user-data",
				required_argument, 0, 'u' }, { 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "b:s:o:u:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'b':
			sscanf(optarg, "%"PRIu64, pBlockSize);
			printf("Using block size: %"PRIu64"\n", *pBlockSize);
			break;
		case 'o':
			*ppOutput = optarg;
			printf("Output checksum file: %s\n", *ppOutput);
			break;
		case 's':
			*ppSource = optarg;
			printf("Input source file: %s\n", *ppSource);
			break;
		case 'u':
			*ppUserdata = optarg;
			printf("User data: %s\n", *ppUserdata);
			break;
		default:
			printf("Invalid option\n");
		}
	}

	if (*ppOutput == NULL || *ppSource == NULL ) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
int bs_checksum(int argc, char** argv) {

	int rc = 0;
	uint64_t blockSize = 0;
	uint64_t totalSize = 0;
	uint64_t blockCount = 0;
	uint64_t lastBlockSize = 0;
	uint64_t currentBlock = 0;
	char* pOutputChecksumFilename = NULL;
	char* pInputSourceFilename = NULL;
	char* pUserData = NULL;
	FILE* pSource = NULL;
	FILE* pChecksum = NULL;
	BSHeader* pHeader = NULL;

	rc = parse_args(argc, argv,
			&blockSize,
			&pInputSourceFilename,
			&pOutputChecksumFilename,
			&pUserData);
	throwiferror(rc, "Invalid arguments", END);;

	// Open source file
	pSource = fopen(pInputSourceFilename, "r");
	if (pSource == NULL ) {
		throw(10, "Error opening source file\n", END);
	}

	// read size
	fseek(pSource, 0L, SEEK_END);
	totalSize = ftell(pSource);
	fseek(pSource, 0L, SEEK_SET);

	// Create header
	pHeader = newHeader(CHECKSUM, totalSize, blockSize, pUserData);
	printHeaderInformation(pHeader, TRUE);

	// Open checksum file
	pChecksum = fopen(pOutputChecksumFilename, "w");
	if (pChecksum == NULL ) {
		throw(11, "Error opening checksum\n", END);
	}

	// Write header
	rc = writeHeader(pChecksum, pHeader);
	throwiferror(rc, "Error writing checksum\n", END);

	blockCount = getBlockCount(pHeader);
	lastBlockSize = getLastBlockSize(pHeader);

	printf("Total block count: %"PRIu64", last block size: %"PRIu64"\n", blockCount, lastBlockSize);

	// Read source file
	void* buffer = malloc(blockSize);
	while (1) {
		size_t len = fread(buffer, 1, blockSize, pSource);
		if (len <= 0) {
			break;
		}
		printProgress(++currentBlock, blockCount, "Checksum");
		uint32_t checksum = getChecksum(buffer, len, pHeader);
		if (fwrite(&checksum, sizeof(uint32_t), 1, pChecksum) != 1) {
			throw(10, "Error writing checksum\n", END);
		}
	}
	printf("Checksum complete\n");

END:
	pHeader = deleteHeader(pHeader);
	autoclose(pChecksum);
	autoclose(pSource);

	return rc;
}
