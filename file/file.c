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
#include <assert.h>
#include <string.h>
#include "file.h"

/* UNIX file ending. Might yield funny results on Windows/Mac files. */
char *file_read_line(FILE *fp)
{
	char *ret = NULL;
	int c;
	size_t i = 0;

	assert(fp != NULL);

	while((c = fgetc(fp)) != EOF) {
		i++;
		ret = realloc(ret, i + 1);
		if(ret == NULL)
			return(NULL);

		ret[i - 1] = c;

		if(c == '\n') break;
	}

	if(i)
		ret[i] = '\0';

	return(ret);
}


char **file_read(FILE *fp)
{
	char **ret = NULL;
	char *line;
	size_t i = 0;

	assert(fp != NULL);

	while((line = file_read_line(fp)) != NULL) {
		i++;
		ret = realloc(ret, sizeof(*ret) * i);
		if(ret == NULL)
			return(NULL);

		ret[i - 1] = line;
	}

	ret = realloc(ret, sizeof(*ret) * (i + 1));
	if(ret == NULL)
		return(NULL);

	ret[i] = NULL;

	return(ret);
}

