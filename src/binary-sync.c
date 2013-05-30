/*
 ============================================================================
 Name        : binary-sync.c
 Author      : Sébastien M-B
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

int main(int argc, char** argv) {
	//return bs_info(argc, argv);
	return bs_checksum(argc, argv);
}
