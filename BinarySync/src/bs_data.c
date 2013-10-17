#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"

RETURN_CODE parse_args(int argc, char** argv,
					   char** ppLeftChecksumFilename,
					   char** ppRightChecksumFilename,
					   char** ppTargetFilename,
					   char** ppOutputFilename,
					   char** ppUserdata) {

	// Default values
	*ppLeftChecksumFilename = NULL;
	*ppRightChecksumFilename = NULL;
	*ppTargetFilename = NULL;
	*ppOutputFilename = NULL;
	*ppUserdata = NULL;

	printf("Parse arguments\n");

	opterr = 0;
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "left", 		required_argument, 0, 'l' },
			{ "right", 		required_argument, 0, 'r' },
			{ "target", 	required_argument, 0, 't' },
			{ "output",		required_argument, 0, 'o' },
			{ "user-data", 	required_argument, 0, 'u' },
			{ 0, 0, 0, 0 } };
		c = getopt_long(argc, argv, "l:r:t:o:u:", long_options, &option_index);
		if (c == -1) { break; }
		switch (c) {
		case 'l':
			*ppLeftChecksumFilename = optarg;
			printf("-- Left checksum file: %s\n", *ppLeftChecksumFilename);
			break;
		case 'r':
			*ppRightChecksumFilename = optarg;
			printf("-- Right checksum file: %s\n", *ppRightChecksumFilename);
			break;
		case 't':
			*ppTargetFilename = optarg;
			printf("-- Target: %s\n", *ppTargetFilename);
			break;
		case 'o':
			*ppOutputFilename = optarg;
			printf("-- Output data file: %s\n", *ppOutputFilename);
			break;
		case 'u':
			*ppUserdata = optarg;
			printf("-- User data: %s\n", *ppUserdata);
			break;
		default:
			printf("Invalid option\n");
		}
	}

	CHECK_PTR_RETURN(ppLeftChecksumFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(ppRightChecksumFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(ppTargetFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(ppOutputFilename, ILLEGAL_ARG);

	return EXIT_SUCCESS;
}

int checkHeaders(BSHeader* pHeaderLeft, BSHeader* pHeaderRight) {
	CHECK_PTR_RETURN(pHeaderLeft, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pHeaderRight, ILLEGAL_ARG);
	if (pHeaderLeft->version != pHeaderRight->version && pHeaderRight->version != 1) {
		return -1;
	}
	if (pHeaderLeft->blockSize != pHeaderRight->blockSize) {
		return -2;
	}
	if (pHeaderLeft->totalSize != pHeaderRight->totalSize) {
		return -3;
	}
	if (pHeaderLeft->type != pHeaderRight->type && pHeaderRight->type != CHECKSUM) {
		return -4;
	}
	return 0;
}

RETURN_CODE bs_data(int argc, char** argv) {

	int rc = 0;
	char* pLeftChecksumFilename = NULL;
	char* pRightChecksumFilename = NULL;
	char* pTargetFilename = NULL;
	char* pOutputFilename = NULL;
	char* pUserdata = NULL;

	FILE* pLeftChecksumFile = NULL;
	FILE* pRightChecksumFile = NULL;
	FILE* pTargetFile = NULL;
	FILE* pOutputFile = NULL;

	BSHeader leftHeader, rightHeader, outputHeader;
	BSFooter leftFooter, rightFooter, outputFooter;

	void* pBuffer = NULL;

TRY

	if ((rc = parse_args(argc, argv,
						 &pLeftChecksumFilename,
						 &pRightChecksumFilename,
						 &pTargetFilename,
						 &pOutputFilename,
						 &pUserdata)) != 0) {
		THROW("Invalid arguments", 1);
	}

	// Open files
	if ((pLeftChecksumFile = fopen(pLeftChecksumFilename, "rb")) == NULL) {
		THROW("Error opening left checksum file", OPEN_ERROR);
	}
	if ((pRightChecksumFile = fopen(pRightChecksumFilename, "rb")) == NULL) {
		THROW("Error opening right checksum file", OPEN_ERROR);
	}

	if ((rc = readHeaderFooter(pLeftChecksumFile, &leftHeader, &leftFooter)) != NO_ERROR) {
		THROW("Error reading header and footer for left checksum", rc);
	}

	if ((rc = readHeaderFooter(pRightChecksumFile, &rightHeader, &rightFooter)) != NO_ERROR) {
		THROW("Error reading header and footer for right checksum", rc);
	}

	// Check
	if ((rc = checkHeaders(&leftHeader, &rightHeader)) != 0) {
		THROW("Checksum files are incompatible", rc);
	}

	// Create header
	if (pUserdata == NULL) {
		pUserdata = rightHeader.pUserData;
	}
	initHeader(&outputHeader, DATA, rightHeader.totalSize, rightHeader.blockSize, pUserdata);
	printHeaderInformation(&outputHeader, TRUE);

	// Open output file
	if ((pOutputFile = fopen(pOutputFilename, "wb")) == NULL) {
		THROW("Error opening output data file", OPEN_ERROR);
	}

	// Write header
	if ((rc = writeHeader(pOutputFile, &outputHeader)) != 0) {
		THROW("Error writing checksum", rc);
	}

	// Open target file
	if ((pTargetFile = fopen(pTargetFilename, "rb")) == NULL) {
		THROW("Error opening target", OPEN_ERROR);
	}

	uint64_t currentBlock;
	uint32_t leftChecksum, rightChecksum;
	uint64_t blockCount = getBlockCount(&outputHeader);
	uint64_t diffCount = 0;
	pBuffer = malloc(outputHeader.blockSize);
	printf("Block count: %"PRIu64"\n", blockCount);
	for (currentBlock = 0; currentBlock < blockCount; currentBlock++) {
		printProgress(currentBlock + 1, blockCount, "Compare checksums");
		if (fread(&leftChecksum,  sizeof(uint32_t), 1, pLeftChecksumFile)  != 1 ||
			fread(&rightChecksum, sizeof(uint32_t), 1, pRightChecksumFile) != 1) {
			THROW("Cannot read from checksum file", 100);
		}
		if (leftChecksum != rightChecksum) {
			printf("Checksum are different for block %"PRIu64"\n", currentBlock);
			CHECK_RC_THROW(readBlock(pTargetFile, &outputHeader, currentBlock, pBuffer), "Error reading target", READ_ERROR);
			// Check if block have valid checksum
			uint32_t currentChecksum = getChecksum(pBuffer, outputHeader.blockSize);
			if (currentChecksum != leftChecksum) {
				printf("  Target file does not march left checksum: %"PRIu32" != %"PRIu32"\n", currentChecksum, leftChecksum);
				THROW("Invalid checksum for block", ILLEGAL_STATE);
			}
			if (fwrite(&currentBlock, sizeof(uint64_t),       1, pOutputFile) != 1 ||
				fwrite(pBuffer,       outputHeader.blockSize, 1, pOutputFile) != 1) {
				THROW("Error writing in data file", WRITE_ERROR);
			}
			diffCount++;
			fflush(pOutputFile);
		}
	}
	initFooter(&outputFooter, diffCount);
	if (fwrite(&outputFooter, FOOTER_LEN, 1, pOutputFile) != 1) {
		THROW("Error writing footer", WRITE_ERROR)
	}

CATCH

FINALLY
	AUTOFREE(pBuffer);
	AUTOCLOSE(pLeftChecksumFile);
	AUTOCLOSE(pRightChecksumFile);
	AUTOCLOSE(pOutputFile);
	AUTOCLOSE(pTargetFile);
	return exceptionId;
}

int main(int argc, char** argv) {
	printf("binary-sync: data\n");
	return bs_data(argc, argv);
}
