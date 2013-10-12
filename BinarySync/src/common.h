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


#define BOOL char
#define TRUE 1
#define FALSE 0

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
	printf("Error: %s, code:%d\n", exceptionMessage, exceptionId);

#define FINALLY \
	NO_EXCEPTION:



#define CHECK_RC_RETURN(RC0, RC) \
	if (RC0 != 0) { \
		return RC; }

#define CHECK_RC_THROW(RC0, message, id) \
	if (RC0 != 0) { \
		THROW(message, id); }

#define CHECK_PTR_RETURN(pLop, RC) \
	if (pLop == NULL) { \
		return RC; }

#define CHECK_PTR_THROW(pLop, message) \
	if (pLop == NULL) { \
		THROW(message, NULLPOINTER); }

#define AUTOFREE(pLop) \
	if (pLop != NULL) { \
		free(pLop); pLop = NULL; }

#define AUTOCLOSE(file) \
	if (file != NULL) { \
		fclose(file); file = NULL; }


uint64_t getFileSize(FILE* pFile);

#endif /* COMMON_H_ */
