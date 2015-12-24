#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"

RETURN_CODE parse_args(int argc, char** argv,
					   char** ppDataFilename,
					   char** ppTargetFilename) {

	// Default values
	*ppDataFilename = NULL;
	*ppTargetFilename = NULL;

	printf("Parse arguments\n");

	opterr = 0;
	int c;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{ "data", 		required_argument, 0, 'd' },
			{ "target", 	required_argument, 0, 't' },
			{ 0, 0, 0, 0 } };
		c = getopt_long(argc, argv, "d:t:", long_options, &option_index);
		if (c == -1) { break; }
		switch (c) {
		case 'd':
			*ppDataFilename = optarg;
			printf("-- Data file: %s\n", *ppDataFilename);
			break;
		case 't':
			*ppTargetFilename = optarg;
			printf("-- Target: %s\n", *ppTargetFilename);
			break;
		default:
			printf("Invalid option\n");
		}
	}

	CHECK_PTR_RETURN(ppDataFilename, ILLEGAL_ARG);
	CHECK_PTR_RETURN(ppTargetFilename, ILLEGAL_ARG);

	return EXIT_SUCCESS;
}

RETURN_CODE checkHeaders(BSHeader* pHeader, FILE* pTagetFile) {
	CHECK_PTR_RETURN(pHeader, ILLEGAL_ARG);
	CHECK_PTR_RETURN(pTagetFile, ILLEGAL_ARG);
	if (pHeader->type != DATA) {
		return -1;
	}
	return NO_ERROR;
}

RETURN_CODE getDataBlockCount(BSHeader* pHeader, FILE* pDataFile, uint64_t* opBlockCount) {
	*opBlockCount = 0;
	RETURN_CODE rc;

	uint64_t fileSize;
	if ((rc = getFileSize(pDataFile, &fileSize)) != NO_ERROR) {
		return rc;
	}
	if (fileSize < (HEADER_LEN+FOOTER_LEN)) {
		return ILLEGAL_STATE;
	}
	uint64_t payload = fileSize - HEADER_LEN - FOOTER_LEN;
	uint64_t payloadUnit = sizeof(uint64_t) + pHeader->blockSize;
	uint64_t lastBlockSize = getLastBlockSize(pHeader);
	uint64_t modulo = payload % payloadUnit;
	if (modulo != 0 && modulo != lastBlockSize) {
		return ILLEGAL_STATE;
	}
	*opBlockCount = (payload / payloadUnit) + (modulo > 0 ? 1 : 0);
	return NO_ERROR;
}

RETURN_CODE bs_apply(int argc, char** argv) {
	RETURN_CODE rc = 0;
	char* pDataFilename = NULL;
	char* pTargetFilename = NULL;
	FILE* pDataFile = NULL;
	FILE* pTargetFile = NULL;
	void* pBuffer = NULL;

	BSHeader header;
	BSFooter footer;

TRY
	if ((rc = parse_args(argc, argv,
						 &pDataFilename,
						 &pTargetFilename)) != 0) {
		THROW("Invalid arguments", 1);
	}

	// Open data file
	if ((pDataFile = fopen(pDataFilename, "rb")) == NULL) {
		THROW("Error opening data file", OPEN_ERROR);
	}

	if ((rc = readHeaderFooter(pDataFile, &header, &footer)) != NO_ERROR) {
		THROW("Error reading header and footer", rc);
	}
	printHeaderInformation(&header, true);
	printFooterInformation(&footer);

	// Open target file
	if ((pTargetFile = fopen(pTargetFilename, "rb+")) == NULL) {
		THROW("Error opening target", OPEN_ERROR);
	}

	// Check
	if ((rc = checkHeaders(&header, pTargetFile)) != 0) {
		THROW("Data file is not compatible with target", rc);
	}

	// fseek on checksum files
	if ((rc = fseekAfterHeader(pDataFile))  != NO_ERROR) {
		THROW("Cannot set position", rc);
	}

	pBuffer = malloc(header.blockSize);
	printf("Data file contains %"PRIu64" block(s)\n", footer.itemCount);
	uint64_t blockCount = getBlockCount(&header);
	uint64_t lastBlockSize = getLastBlockSize(&header);
	uint64_t blockId;
	truncate(pTargetFilename, header.totalSize);
	for (uint64_t i = 0; i < footer.itemCount; ++i) {
		// Read block Id
		if (fread(&blockId, sizeof(uint64_t), 1, pDataFile) != 1) {
			THROW("Cannot read from data file", READ_ERROR);
		}
		if (fread(pBuffer, header.blockSize, 1, pDataFile) != 1) {
			THROW("Cannot read from data file", READ_ERROR);
		}
		printf("Block %"PRIu64" ... ", blockId);
		if(fseek(pTargetFile, blockId * header.blockSize, SEEK_SET) != 0) {
			THROW("Cannot set position on target file from", SEEK_ERROR);
		}
		if (fwrite(pBuffer, (blockId == (blockCount - 1)) ? lastBlockSize : header.blockSize, 1, pTargetFile) != 1) {
			THROW("Error writing block", rc);
		}
		printf("OK\n");
	}

CATCH

FINALLY
	AUTOFREE(pBuffer);
	AUTOCLOSE(pDataFile);
	AUTOCLOSE(pTargetFile);
	return exceptionId;
}

int main(int argc, char** argv) {
	printf("binary-sync: apply\n");
	return bs_apply(argc, argv);
}
