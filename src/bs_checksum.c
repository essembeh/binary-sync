#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"
#include "adler32.h"
#include "block.h"

int bs_checksum(int argc, char** argv) {

	uint64_t blockSize = 0;
	char* pOutputChecksumFilename = NULL;
	char* pInputSourceFilename = NULL;
	char* pUserData = NULL;

	opterr = 0;

	int c;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = { { "block-size",
				required_argument, 0, 'b' }, { "output", required_argument, 0,
				'o' }, { "source", required_argument, 0, 's' }, { "user-data",
				required_argument, 0, 'u' }, { 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "b:o:s:u:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'b':
			sscanf(optarg, "%"PRIu64, &blockSize);
			printf("Using block size: %"PRIu64"\n", blockSize);
			break;
		case 'o':
			pOutputChecksumFilename = optarg;
			printf("Output checksum file: %s\n", pOutputChecksumFilename);
			break;
		case 's':
			pInputSourceFilename = optarg;
			printf("Input source file: %s\n", pInputSourceFilename);
			break;
		case 'u':
			pUserData = optarg;
			printf("User data: %s\n", pUserData);
			break;
		default:
			printf("Invalid option\n");
		}
	}

	if (pInputSourceFilename == NULL || pOutputChecksumFilename == NULL
			|| blockSize <= 0) {
		return EXIT_FAILURE;
	}

	// Open source file
	FILE* pSource = fopen(pInputSourceFilename, "r");
	if (pSource == NULL ) {
		perror("Error opening source file\n");
		exit(10);
	}
	// read size
	fseek(pSource, 0L, SEEK_END);
	uint64_t totalSize = ftell(pSource);
	fseek(pSource, 0L, SEEK_SET);

	// Create header
	BSHeader* pHeader = newHeader(CHECKSUM, totalSize, blockSize, pUserData);
	printHeaderInformation(pHeader, TRUE);

	// Open checksum file
	FILE* pChecksum = fopen(pOutputChecksumFilename, "w");
	if (pChecksum == NULL ) {
		perror("Error writing checksum\n");
		exit(10);
	}

	// Write header
	if (writeHeader(pChecksum, pHeader) != 0) {
		perror("Error writing checksum\n");
		exit(10);
	}

	uint64_t blockCount = getBlockCount(pHeader);
	uint64_t lastBlockSize = getLastBlockSize(pHeader);

	printf("Total block count: %"PRIu64", last block size: %"PRIu64"\n", blockCount, lastBlockSize);

	// Read source file
	void* buffer = malloc(blockSize);
	while (1) {
		size_t len = fread(buffer, 1, blockSize, pSource);
		if (len > 0) {
			uint32_t checksum = getChecksum(buffer, len, pHeader);
			if (fwrite(&checksum, sizeof(uint32_t), 1, pChecksum) != 1) {
				perror("Error writing checksum\n");
				exit(10);
			}
		} else {
			break;
		}
	}

	fclose(pChecksum);
	fclose(pSource);
	return EXIT_SUCCESS;
}
