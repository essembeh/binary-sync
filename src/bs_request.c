#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"
#include "adler32.h"
#include "block.h"



int parse_args(int argc, char** argv, char** ppLeft, char** ppRight, char** ppOutput, char** ppUserdata) {
	// Default values
	*ppOutput = NULL;
	*ppLeft = NULL;
	*ppRight = NULL;
	*ppUserdata = NULL;

	printf("Parse arguments");

	opterr = 0;
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
				{ "left", required_argument, 0, 'l' },
				{ "right", required_argument, 0, 'r' },
				{ "output", required_argument, 0, 'o' },
				{ "user-data", required_argument, 0, 'u' },
				{ 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "l:r:o:u:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'l':
			*ppLeft = optarg;
			printf("Left checksum file: %s\n", *ppLeft);
			break;
		case 'r':
			*ppRight = optarg;
			printf("Right checksum file: %s\n", *ppRight);
			break;
		case 'o':
			*ppOutput = optarg;
			printf("Output file: %s\n", *ppOutput);
			break;
		case 'u':
			*ppUserdata = optarg;
			printf("User data: %s\n", *ppUserdata);
			break;
		default:
			printf("Invalid option\n");
		}
	}

	if (*ppOutput == NULL || *ppLeft == NULL  || *ppRight == NULL ) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int bs_request(int argc, char** argv) {

	int rc = 0;
	uint64_t blockSize = 0;
	uint64_t totalSize = 0;
	uint64_t blockCount = 0;
	uint64_t lastBlockSize = 0;
	uint64_t currentBlock = 0;
	char* pOutputRequestFilename = NULL;
	char* pLeftChecksumFilename = NULL;
	char* pRightChecksumFilename = NULL;
	char* pUserData = NULL;
	FILE* pLeft= NULL;
	FILE* pRight = NULL;
	FILE* pOutput = NULL;
	BSHeader* pHeaderLeft = NULL;
	BSHeader* pHeaderRight = NULL;
	BSHeader* pHeaderOut = NULL;

TRY;

	rc = parse_args(argc, argv,
			&pLeftChecksumFilename,
			&pRightChecksumFilename,
			&pOutputRequestFilename,
			&pUserData);

	if (rc != 0) {
		THROW("Invalid arguments", 1);
	}

	// Open files
	pLeft = fopen(pLeftChecksumFilename, "r");
	if (pLeft == NULL ) {
		THROW("Error opening left checksum", 10);
	}
	pHeaderLeft = readHeader(pLeft);

	pRight = fopen(pRightChecksumFilename, "r");
	if (pRight == NULL ) {
		THROW("Error opening right checksum", 11);
	}
	pHeaderRight = readHeader(pRight);

	// Create header
	if (pUserData == NULL) {
		pUserData = pHeaderRight->pUserData;
	}
	pHeaderOut = newHeader(REQUEST, pHeaderRight->totalSize, pHeaderRight->blockSize, pUserData);
	printHeaderInformation(pHeaderOut, TRUE);

	// Open checksum file
	pOutput = fopen(pOutputRequestFilename, "w");
	if (pOutput == NULL ) {
		THROW("Error opening output request file", 12);
	}

	// Write header
	if (writeHeader(pOutput, pHeaderOut)) {
		THROW("Error writing checksum", 20);
	}

CATCH;

FINALLY;
	pHeaderLeft = deleteHeader(pHeaderLeft);
	pHeaderRight = deleteHeader(pHeaderRight);
	pHeaderOut = deleteHeader(pHeaderOut);
	autoclose(pLeft);
	autoclose(pRight);
	autoclose(pOutput);

	return exceptionId;
}

int main (int argc, char** argv) {
	return bs_request(argc, argv);
}
