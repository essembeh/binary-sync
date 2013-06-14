/*
 * adler32.h
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#ifndef ADLER32_H_
#define ADLER32_H_

#include "zutil.h"

uLong ZEXPORT adler32(uLong adler, const Bytef *buf, uInt len);

#endif /* ADLER32_H_ */
