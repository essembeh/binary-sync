/*
 * common.h
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#ifndef COMMON_H_
#define COMMON_H_

#define TRY \
	char* exceptionMessage = NULL; \
	int exceptionId = 0; \

#define THROW(message, id) \
	exceptionMessage = message; \
	exceptionId = id; \
	goto CATCH_EXCEPTION; \


#define CATCH \
	goto NO_EXCEPTION; \
	CATCH_EXCEPTION: \
	printf("Error: %s, code:%d\n", exceptionMessage, exceptionId); \


#define FINALLY \
	NO_EXCEPTION: \


#define autofree(a) if (a != NULL) { free(a); a = NULL; }
#define autoclose(a) if (a != NULL) { fclose(a); a = NULL; }


#endif /* COMMON_H_ */
