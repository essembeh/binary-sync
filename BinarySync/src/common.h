/*
 * common.h
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <inttypes.h>

#include "error.h"


#define TRY \
	char* exceptionMessage = NULL; \
	RETURN_CODE exceptionId = NO_ERROR;

#define THROW(message, id) \
	exceptionMessage = message; \
	exceptionId = id; \
	goto CATCH_EXCEPTION;

#define CATCH \
	goto NO_EXCEPTION; \
	CATCH_EXCEPTION: \
	printf("[ERROR] %s (%d)\n", exceptionMessage, exceptionId);

#define FINALLY \
	NO_EXCEPTION:


#define ASSERT_RETURN(COND, RC) \
	if (! (COND)) { \
		return RC; }

#define ASSERT_THROW(COND, MESSAGE) \
	if (! (COND)) { \
		THROW(MESSAGE, ASSERTION_ERROR); }

#define CHECK_RC_RETURN(RC, RC1) \
	if (RC != NO_ERROR) { \
		return RC1; }

#define CHECK_RC_THROW(RC, MESSAGE, RC1) \
	if (RC != NO_ERROR) { \
		THROW(MESSAGE, RC1); }

#define CHECK_PTR_RETURN(PTR, RC) \
	if (PTR == NULL) { \
		return RC; }

#define CHECK_PTR_THROW(PTR, MESSAGE) \
	if (PTR == NULL) { \
		THROW(MESSAGE, NULLPOINTER); }

#define AUTOFREE(PTR) \
	if (PTR != NULL) { \
		free(PTR); PTR = NULL; }

#define AUTOCLOSE(PTR) \
	if (PTR != NULL) { \
		fclose(PTR); PTR = NULL; }

RETURN_CODE getFileSize(FILE* pFile, uint64_t* opSize);
void printProgress(uint64_t current, uint64_t total, char* message);
uint32_t getChecksum(void* pData, size_t size);

#endif /* COMMON_H_ */
