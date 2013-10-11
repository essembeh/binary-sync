/*
 * common.h
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "error.h"

RETURN_CODE bs_info(int argc, char** argv);
RETURN_CODE bs_checksum(int argc, char** argv);
RETURN_CODE bs_data(int argc, char** argv);
RETURN_CODE bs_apply(int argc, char** argv);

#endif /* COMMON_H_ */
