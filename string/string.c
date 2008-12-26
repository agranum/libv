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

/* Some string functions. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "string.h"

/* Remove leading whitespaces */
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

/* Remove trailing whitespaces */
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

/* Remove leading and trailing whitespaces */
char *trim(char *const s)
{
	rtrim(s);
	ltrim(s);

	return s;
}

/* return a malloc'd copy of the substring of length len starting at start */
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

/* count occurrences of needle in haystack */
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

/* compute size of the string that results in replacing all occurences
 * for replace in haystack */
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

/* saves the integral position of needle within haystack in pos */
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

/* replaces all occurences of "needle" for "replace" in "haystack" and copies
 * the result to "s" */
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

/* wrapper for substr_replace_r(), allocates memory accordingly */
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

/* Computes an array of strings, each of which is a substring of "str" formed
 * by splitting it on boundaries formed by "delim" and saves the result
 * in "a" */
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

/* Wrapper for explode_r(), allocates memory accordingly */
char **explode(const char *str, const char *delim)
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

/* check whether str only contains digits, preceeded by +/- */
int isint(const char *str)
{
	char *p = (char *)str;

	if(p == NULL)
		return 0;

	while(*p) {
		if(!isdigit(*p)) {
			if(p != str)
				return 0;

			if((*p != '+') && (*p != '-'))
				return 0;
		}

		p++;
	}

	if(str == p)
		return 0;

	return 1;
}

/* replaces occurrences of any of the chars in "chrs" with "c" */
void str_unify(char *s, const char *chrs, int c)
{
	char *p = s;

	while(p && *p) {
		if(strchr(chrs, *p) != NULL)
			*p = c;

		++p;
	}
}

