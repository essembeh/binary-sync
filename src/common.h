/*
 * common.h
 *
 *  Created on: 30 mai 2013
 *      Author: sebastien
 */

#ifndef COMMON_H_
#define COMMON_H_

#define autofree(a) if (a != NULL) { free(a); a = NULL; }
#define autoclose(a) if (a != NULL) { fclose(a); a = NULL; }

#define throw(RC, MESSAGE, LABEL) perror(MESSAGE); rc = RC; goto LABEL;
#define throwiferror(RC, MESSAGE, LABEL)  if (rc && 1) { perror(MESSAGE); rc = RC; goto LABEL; }

#endif /* COMMON_H_ */
