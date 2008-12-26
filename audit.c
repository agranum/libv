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

/* Properly check for add/sub/mul overflow.
 * Stolen from http://c-faq.com/misc/sd26.html 
 * See also http://gcc.gnu.org/bugzilla/show_bug.cgi?id=30475
 */

#include <stdio.h>
#include <limits.h>
#include "audit.h"

int valid_add_int(int a, int b)
{
	if(b < 0)
		return valid_sub_int(a, -b);

	if(INT_MAX - b < a) {
		fputs("\n*** int overflow ***\n", stderr);
		return INT_MAX;
	}

	return a + b;
}

int valid_sub_int(int a, int b)
{
	if(b < 0)
		return valid_add_int(a, -b);

	if(INT_MIN + b > a) {
		fputs("\n*** int underflow ***\n", stderr);
		return INT_MIN;
	}

	return a - b;
}

int valid_mul_int(int a, int b)
{
	int sign = 1;

	if(a == 0 || b == 0) return 0;
	if(a < 0) { a = -a; sign = -sign; }
	if(b < 0) { b = -b; sign = -sign; }
	if(INT_MAX / b < a) {
		fputs("\n*** int overflow ***\n", stderr);
		return (sign > 0) ? INT_MAX : INT_MIN;
	}

	return sign * a * b;
}

/* FIXME: add checks for uint and *long */

