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

#ifndef STRING_H_
#define STRING_H_

char *ltrim(char *const s);
char *rtrim(char *const s);
char *trim(char *const s);
char *substr(const char *s, size_t start, size_t len);
size_t substr_count(const char *haystack, const char *needle);
size_t substr_replace_compute_size(const char *haystack,
				   const char *needle,
				   const char *replace);
int substr_index(size_t *pos, const char *haystack, const char *needle);
void substr_replace_r(      char *s, 
		      const char *haystack, 
		      const char *needle, 
		      const char *replace);
char *substr_replace(const char *haystack,
		     const char *needle, 
		     const char *replace);
size_t par_size(char ***par);
int par_add(char ***par, const char *s);
void par_foreach(char ***par, void func(void *));
void par_free(char ***par);
void explode_r(char **a, char *p, const char *str, const char *delim);
char **explode(const char *str, const char *delim);
int isint(const char *str);
void str_unify(char *s, const char *chrs, int c);

#endif  /* ! STRING_H_ */
