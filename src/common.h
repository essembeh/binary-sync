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

#define THROW(message) \
	exceptionMessage = message; \
	goto CATCH_EXCEPTION; \


#define CATCH \
	goto NO_EXCEPTION; \
	CATCH_EXCEPTION: \
	printf("Error: %s\n", exceptionMessage); \


#define FINALLY \
	NO_EXCEPTION: \


#define autofree(a) if (a != NULL) { free(a); a = NULL; }
#define autoclose(a) if (a != NULL) { fclose(a); a = NULL; }


#endif /* COMMON_H_ */
