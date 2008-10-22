/*  Copyright (c) 2006-2008, Philip Busch <broesel@studcs.uni-sb.de>
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
#include "string.h"


static int string_first_nonwhite(char *s)
{
	int i = 0;

	while(s[i] != '\0') {
		if(strchr(WHITESPACES, s[i]) == NULL)
			break;
		else i++;
	}

	return(i);
}


static int string_last_nonwhite(char *s)
{
	int len, i = 0;

	len = strlen(s) - 1;

	while(i < len) {
		if(strchr(WHITESPACES, s[len - i]) == NULL)
			break;
		else i++;
	}

	return(len - i);
}


void string_trim(char *s)
{
	int first, last;

	if(s == NULL)
		return;

	first = string_first_nonwhite(s);
	last  = string_last_nonwhite(s);

	if(last == 0) {
		s[0] = '\0';
	} else {
		s[last + 1] = '\0';
		memmove(s, s + first, strlen(s) - first + 1);
	}
}


int main()
{
	FILE *fp;
	char buf[1024];

	fp = fopen("test.txt", "r");
	if(fp == NULL) {
		fprintf(stderr, "ERROR: cannot open file\n");
		exit(EXIT_FAILURE);
	}

	while(fgets(buf, 1024, fp) != NULL) {
		string_trim(buf);
		printf("%d===%s===\n", strlen(buf), buf);
	}

	fclose(fp);

	return(0);
}

