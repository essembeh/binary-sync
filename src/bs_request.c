#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"
#include "adler32.h"
#include "block.h"

int bs_request(int argc, char** argv) {

	uint64_t blockSize = 0;
	char* pFromChecksumFilename = NULL;
	char* pToChecksumFilename = NULL;
	char* pOutputRequestFilename = NULL;
	char* pUserData = NULL;

	opterr = 0;

	int c;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
				{ "from", required_argument, 0, 'f' },
				{ "to", required_argument, 0, 't' },
				{ "output", required_argument, 0, 'o' },
				{ "user-data", required_argument, 0, 'u' },
				{ 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "f:t:o:u:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'f':
			pFromChecksumFilename = optarg;
			printf("From checksum file: %s\n", pFromChecksumFilename);
			break;
		case 't':
			pToChecksumFilename = optarg;
			printf("To checksum file: %s\n", pToChecksumFilename);
			break;
		case 'o':
			pOutputRequestFilename = optarg;
			printf("Output request file: %s\n", pOutputRequestFilename);
			break;
		case 'u':
			pUserData = optarg;
			printf("User data: %s\n", pUserData);
			break;
		default:
			printf("Invalid option\n");
		}
	}

	if (pFromChecksumFilename == NULL || pToChecksumFilename == NULL
			|| pOutputRequestFilename == NULL) {
		return EXIT_FAILURE;
	}


/*
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
		perror("Error opening checksum\n");
		exit(10);
	}

	// Write header
	int rc = writeHeader(pChecksum, pHeader);
	exitrc(rc, "Error writing checksum");

	uint64_t blockCount = getBlockCount(pHeader);
	uint64_t lastBlockSize = getLastBlockSize(pHeader);

	printf("Total block count: %"PRIu64", last block size: %"PRIu64"\n",
			blockCount, lastBlockSize);

	// Read source file
	void* buffer = malloc(blockSize);
	uint64_t currentBlockIndex = 0;
	while (1) {
		size_t len = fread(buffer, 1, blockSize, pSource);
		if (len <= 0) {
			break;
		}
		printProgress(++currentBlockIndex, blockCount, "Checksum");
		uint32_t checksum = getChecksum(buffer, len, pHeader);
		if (fwrite(&checksum, sizeof(uint32_t), 1, pChecksum) != 1) {
			perror("Error writing checksum\n");
			exit(10);
		}
	}
	printf("Checksum complete\n");
	autofree(pHeader);
	fclose(pChecksum);
	fclose(pSource);
*/
	return EXIT_SUCCESS;
}
