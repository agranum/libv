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
#include <ctype.h>
#include "string.h"


char *ltrim(char *const s)
{
	size_t len;
	char *cur;

	if(s && *s) {
		len = strlen(s);
		cur = s;

		while(*cur && isspace(*cur))
			++cur, --len;

		if(s != cur)
			memmove(s, cur, len + 1);
	}

	return s;
}

char *rtrim(char *const s)
{
	size_t len;
	char *cur;

	if(s && *s) {
		len = strlen(s);
		cur = s + len - 1;

		while(cur != s && isspace(*cur))
			--cur, --len;

		cur[isspace(*cur) ? 0 : 1] = '\0';
	}

	return s;
}

char *trim(char *const s)
{
	rtrim(s);
	ltrim(s);

	return s;
}


char *substr(const char *s, size_t start, size_t len)
{
	char *d;

	assert(s != NULL);

	d = malloc(len + 1);

	if(d == NULL)
		return NULL;

	strncpy(d, s + start, len);
	d[len] = '\0';

	return d;
}


size_t substr_count(const char *haystack, const char *needle)
{
	size_t len, c = 0;

	assert(haystack != NULL);
	assert(needle   != NULL);

	if(*haystack && *needle) {
		len = strlen(needle);

		while((haystack = strstr(haystack, needle))) 
			++c, haystack += len;
	}

	return c;
}

size_t substr_replace_compute_size(const char *haystack,
				   const char *needle,
				   const char *replace)
{
	size_t m;

	assert(haystack != NULL);
	assert(needle   != NULL);
	assert(replace  != NULL);

	m = substr_count(haystack, needle);

	return strlen(haystack)  -  m * strlen(needle)  +  m * strlen(replace);
}

int substr_index(size_t *pos, const char *haystack, const char *needle)
{
	char *p;

	assert(pos      != NULL);
	assert(haystack != NULL);
	assert(needle   != NULL);

	*pos = 0;
	p    = strstr(haystack, needle);

	if(p == NULL)
		return -1;

	while(haystack != p)
		++haystack, ++(*pos);

	return 0;
}

void substr_replace_r(      char *s, 
		      const char *haystack, 
		      const char *needle, 
		      const char *replace)
{
	size_t pos, len_n, len_r;

	assert(s        != NULL);
	assert(haystack != NULL);
	assert(needle   != NULL);
	assert(replace  != NULL);

	len_n = strlen(needle);
	len_r = strlen(replace);

	if(*needle) {
		while(substr_index(&pos, haystack, needle) >= 0) {
			strncpy(s, haystack, pos);
			s += pos;

			strcpy(s, replace);
			s += len_r;

			haystack += pos + len_n;
		}
	}

	strcpy(s, haystack);
}

char *substr_replace(const char *haystack,
		     const char *needle, 
		     const char *replace)
{
	size_t len;
	char *ret = NULL;

	assert(haystack != NULL);
	assert(needle   != NULL);
	assert(replace  != NULL);

	len = substr_replace_compute_size(haystack, needle, replace) + 1;

	ret = malloc(len);

	if(ret == NULL)
		return NULL;

	substr_replace_r(ret, haystack, needle, replace);

	return ret;
}

size_t par_size(char ***par)
{
	size_t size = 0;

	assert(par != NULL);

	if(*par == NULL)
		return 0;

	while((*par)[size++] != NULL);

	return size - 1;
}


int par_add(char ***par, const char *s)
{
	size_t size;

	assert(par != NULL);

	size = par_size(par) + 1;

	*par = realloc(*par, (size + 1) * sizeof(s));
	if(*par == NULL)
		return -1;

	(*par)[size - 1] = (char *)s;
	(*par)[size] = NULL;

	return 0;
}


void par_foreach(char ***par, void func(void *))
{
	char **p;

	assert(par != NULL);

	p = *par;

	while(p && *p)
		func(*p++);
}


void par_free(char ***par)
{
	assert(par != NULL);

	par_foreach(par, free);
	free(*par);

	*par = NULL;
}


/*
char **explode(const char *s, int c)
{
	char **par = NULL, *tmp;
	size_t start = 0, i;

	if(s == NULL)
		return NULL;

	for(i = 0; i <= strlen(s); i++) {
		if((s[i] == c) || (s[i] == '\0')) {
			tmp = substr(s, start, i - start);

			if(tmp == NULL) {
				par_free(&par);
				return NULL;
			}

			if(par_add(&par, tmp) < 0) {
				par_free(&par);
				return NULL;
			}
			start = i + 1;
		}
	}

	return(par);
}
*/

void explode_r(char **a, char *p, const char *str, const char *delim)
{
	size_t pos, len_d, i = 0;

	assert(p      != NULL);
	assert(a      != NULL);
	assert(str    != NULL);
	assert(delim  != NULL);

	len_d = strlen(delim);

	while(substr_index(&pos, str, delim) >= 0) {
		strncpy(p, str, pos);
		a[i++] = p;
		p += pos;
		*p++ = '\0';

		str += pos + len_d;
	}
	strcpy(p, str);
	a[i++] = p;
	a[i] = NULL;
}

char **explode(char *str, char *delim)
{
	size_t len, size;
	char *s, **p;

	assert(str   != NULL);
	assert(delim != NULL);

	size = substr_count(str, delim) + 2;
	len  = substr_replace_compute_size(str, delim, "") + size - 1;

	s = malloc(len * sizeof(*s));
	if(s == NULL)
		return(NULL);

	p = malloc(size * sizeof(*p));
	if(p == NULL) {
		free(s);
		return(NULL);
	}

	explode_r(p, s, str, delim);

	return(p);
}

void printstring(void *s)
{
	printf("---%s---\n", (char *)s);
}


int main()
{
	char **tok = NULL;
	char *p;
	char buf[] = "whihihi foodu wsnafu tralala xfooyw";
	int i;

	tok = explode("xxxfoo barxxxtralalaxxxxxx", "xxx");

	par_foreach(&tok, printstring);

	return(0);

	p = substr_replace(buf, "", "");
	printf("string: %s\nresult: %s\n", buf, p);

	return(0);
}

