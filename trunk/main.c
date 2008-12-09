/*  Copyright (c) 2006-2008, Philip Busch <philip@0xe3.com>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "main.h"

static void print_help(const char *);
static void print_usage(const char *);

int main(int argc, char *argv[])
{
	int c;
	int vlevel = 0;

	opterr = 0;
	while((c = getopt(argc, argv, ":vh")) != -1) {
		switch(c) {
			case 'a':
				break;
			case 'v':
				++vlevel;
				break;
			case 'h':
				print_help(argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				if(isprint(optopt)) {
					fprintf(stderr, \
					"%s: unknown option -%c\n", \
					argv[0], optopt);
				} else {
					fprintf(stderr, \
					"%s: unknown option character \\x%x\n"\
					, argv[0], optopt);
				}
				return(EXIT_FAILURE);
			case ':':
				fprintf(stderr, \
				"%s: option -%c requires an argument\n",\
				argv[0], optopt);
				return(EXIT_FAILURE);
		}
	}

	printf("%d\n", vlevel);

	while(argv[optind] != NULL) {
		printf("%s\n", argv[optind++]);
	}

	return EXIT_SUCCESS;
}

static void print_usage(const char *prog)
{
	fprintf(stderr, "USAGE: %s [option(s) --] <file>\n", prog);
}

static void print_help(const char *prog)
{
	printf("%s: FIXME: add some help text", prog);
}
