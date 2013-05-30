#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"
#include "bsheader.h"

int bs_checksum(int argc, char** argv) {
	opterr = 0;

	int c;
	int digit_optind = 0;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
				{ "block-size", required_argument, 0, 'b' },
				{ "output", no_argument, 0, 'o' },
				{ "user-data", required_argument, 0, 'u' },
				{ 0, 0, 0, 0 } };

		c = getopt_long(argc, argv, "b:o:u:", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch (c) {
		case 'b':
			printf("option b with value '%s'\n", optarg);
			break;

		case 'o':
			printf("option o with value '%s'\n", optarg);
			break;

		case 'u':
			printf("option u with value '%s'\n", optarg);
			break;

		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc) {
			printf("%s ", argv[optind++]);
		}
		printf("\n");
	}

	return EXIT_SUCCESS;
}
