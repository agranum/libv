/*  Copyright (c) 2009, Philip Busch <philip@0xe3.com>
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
#include <time.h>
#include <sys/time.h>
#include "timer.h"
#include "debug.h"

timer *timer_init()
{
	timer *t = malloc(sizeof(timer));

	if(t != NULL) {
		t->user = 0;
		t->real.tv_sec = 0;
		t->real.tv_usec = 0;
	}

	return t;
}

void timer_destroy(timer *t)
{
	assert(t != NULL);

	free(t);
}

void timer_start(timer *t)
{
	assert(t != NULL);

	t->user = clock();
	gettimeofday(&(t->real), NULL);
}

void timer_stop(timer *t)
{
	struct timeval my_tv;

	assert(t != NULL);

	gettimeofday(&my_tv, NULL);

	t->user = clock() - t->user;
	t->real.tv_sec  = my_tv.tv_sec  - t->real.tv_sec;
	t->real.tv_usec = my_tv.tv_usec - t->real.tv_usec;
}

double timer_get_user(timer *t)
{
	assert(t != NULL);

	return((double)t->user / (double)CLOCKS_PER_SEC);
}

double timer_get_real(timer *t)
{
	assert(t != NULL);

	return t->real.tv_sec + t->real.tv_usec / 1000000.;
}

void timer_print(timer *t)
{
	assert(t != NULL);

	printf("real: %f\n", timer_get_real(t));
	printf("user: %f\n", timer_get_user(t));
}
