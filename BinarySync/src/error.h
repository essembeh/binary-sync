/*
 * error.h
 *
 *  Created on: 11 oct. 2013
 *      Author: sebastien
 */

#ifndef ERROR_H_
#define ERROR_H_

#define RETURN_CODE int

#define NO_ERROR 0

#define UNKNOWN_ERROR   1
#define NULLPOINTER     10
#define MALLOC          11

#define OPEN_ERROR      100
#define READ_ERROR      101
#define WRITE_ERROR     102
#define SEEK_ERROR      103

#define ILLEGAL_ARG     110
#define ILLEGAL_STATE   111
#define ASSERTION_ERROR 112

#define BS_INVALID_HEADER      201
#define BS_INVALID_FOOTER      202
#define BS_INVALID_VERSION     203
#define BS_INVALID_TAG         204
#define BS_INVALID_SIZE        205
#define BS_INVALID_BLOCK_COUNT 206
#define BS_INVALID_ITEMCOUNT   207

#endif /* ERROR_H_ */
