#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"

RETURN_CODE parse_args(int argc, char** argv,
					   char** ppRemoteChecksumFilename,
					   char** ppMasterChecksumFilename,
					   char** ppTargetFilename,
					   char** ppOutputFilename,
					   char** ppUserdata) {

	// Default values
	*ppRemoteChecksumFilename = NULL;
	*ppMasterChecksumFilename = NULL;
	*ppTargetFilename = NULL;
	*ppOutputFilename = NULL;
	*ppUserdata = NULL;

	printf("Parse arguments\n");

	opterr = 0;
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "remote", 	required_argument, 0, 'r' },
			{ "master", 	required_argument, 0, 'm' },
			{ "target", 	required_argument, 0, 't' },
			{ "output",		required_argument, 0, 'o' },
			{ "user-data", 	required_argument, 0, 'u' },
			{ 0, 0, 0, 0 } };
		c = getopt_long(argc, argv, "r:m:t:o:u:", long_options, &option_index);
		if (c == -1) { break; }
		switch (c) {
		case 'r':
			*ppRemoteChecksumFilename = optarg;
			printf("-- Remote checksum file: %s\n", *ppRemoteChecksumFilename);
			break;
		case 'm':
			*ppMasterChecksumFilename = optarg;
			printf("-- Master checksum file: %s\n", *ppMasterChecksumFilename);
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

	CHECK_PTR_RETURN(ppRemoteChecksumFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(ppMasterChecksumFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(ppTargetFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(ppOutputFilename, ILLEGAL_ARG);

	return EXIT_SUCCESS;
}

int checkHeaders(BSHeader* pHeaderRemote, BSHeader* pHeaderMaster) {
	CHECK_PTR_RETURN(pHeaderRemote, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pHeaderMaster, ILLEGAL_ARG);
	if (pHeaderRemote->version != pHeaderMaster->version && pHeaderMaster->version != 1) {
		return -1;
	}
	if (pHeaderRemote->blockSize != pHeaderMaster->blockSize) {
		return -2;
	}
	if (pHeaderRemote->type != pHeaderMaster->type && pHeaderMaster->type != CHECKSUM) {
		return -3;
	}
	return 0;
}

RETURN_CODE bs_data(int argc, char** argv) {

	int rc = 0;
	char* pRemoteChecksumFilename = NULL;
	char* pMasterChecksumFilename = NULL;
	char* pTargetFilename = NULL;
	char* pOutputFilename = NULL;
	char* pUserdata = NULL;

	FILE* pRemoteChecksumFile = NULL;
	FILE* pMasterChecksumFile = NULL;
	FILE* pTargetFile = NULL;
	FILE* pOutputFile = NULL;

	BSHeader remoteHeader, masterHeader, outputHeader;
	BSFooter leftFooter, rightFooter, outputFooter;

	void* pBuffer = NULL;

TRY
	if ((rc = parse_args(argc, argv,
						 &pRemoteChecksumFilename,
						 &pMasterChecksumFilename,
						 &pTargetFilename,
						 &pOutputFilename,
						 &pUserdata)) != 0) {
		THROW("Invalid arguments", 1);
	}

	// Open files
	if ((pRemoteChecksumFile = fopen(pRemoteChecksumFilename, "rb")) == NULL) {
		THROW("Error opening left checksum file", OPEN_ERROR);
	}
	if ((pMasterChecksumFile = fopen(pMasterChecksumFilename, "rb")) == NULL) {
		THROW("Error opening right checksum file", OPEN_ERROR);
	}

	if ((rc = readHeaderFooter(pRemoteChecksumFile, &remoteHeader, &leftFooter)) != NO_ERROR) {
		THROW("Error reading header and footer for left checksum", rc);
	}

	if ((rc = readHeaderFooter(pMasterChecksumFile, &masterHeader, &rightFooter)) != NO_ERROR) {
		THROW("Error reading header and footer for right checksum", rc);
	}

	// Check
	if ((rc = checkHeaders(&remoteHeader, &masterHeader)) != 0) {
		THROW("Checksum files are incompatible", rc);
	}

	// Create header
	if (pUserdata == NULL) {
		pUserdata = masterHeader.pUserData;
	}
	initHeader(&outputHeader, DATA, masterHeader.totalSize, masterHeader.blockSize, pUserdata);
	printHeaderInformation(&outputHeader, true);

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

	// fseek on checksum files
	if ((rc = fseekAfterHeader(pRemoteChecksumFile))  != NO_ERROR ||
		(rc = fseekAfterHeader(pMasterChecksumFile)) != NO_ERROR) {
		THROW("Cannot set position", rc);
	}

	uint32_t remoteChecksum, masterChecksum;
	uint64_t blockCount = getBlockCount(&outputHeader);
	uint64_t remoteBlockCount = getBlockCount(&remoteHeader);
	uint64_t diffCount = 0;
	pBuffer = malloc(outputHeader.blockSize);
	printf("Block count: %"PRIu64"\n", blockCount);
	for (uint64_t currentBlock = 0; currentBlock < blockCount; currentBlock++) {
		printProgress(currentBlock + 1, blockCount, "Compare checksums");

		if (currentBlock < remoteBlockCount) {
			if (fread(&remoteChecksum,  sizeof(uint32_t), 1, pRemoteChecksumFile)  != 1) {
				THROW("Cannot read from checksum file", 100);
			}
		}
		
		if (fread(&masterChecksum,  sizeof(uint32_t), 1, pMasterChecksumFile)  != 1) {
			THROW("Cannot read from checksum file", 100);
		}

		if (currentBlock >= remoteBlockCount || remoteChecksum != masterChecksum) {
			printf("Checksum are different for block %"PRIu64" (%"PRIu32" != %"PRIu32")\n", currentBlock, masterChecksum, remoteChecksum);
			CHECK_RC_THROW(readBlock(pTargetFile, &outputHeader, currentBlock, pBuffer), "Error reading target", READ_ERROR);
			// Check if block have valid checksum
			uint32_t currentChecksum = getChecksum(pBuffer, outputHeader.blockSize);
			if (currentChecksum != masterChecksum) {
				printf("  /!\\ Block %"PRIu64" from target is %"PRIu32" and should be %"PRIu32"\n",
						currentBlock, currentChecksum, masterChecksum);
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
	AUTOCLOSE(pRemoteChecksumFile);
	AUTOCLOSE(pMasterChecksumFile);
	AUTOCLOSE(pOutputFile);
	AUTOCLOSE(pTargetFile);
	return exceptionId;
}

int main(int argc, char** argv) {
	printf("binary-sync: data\n");
	return bs_data(argc, argv);
}
