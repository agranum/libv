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

/**
 * @file
 * Linked lists header.
 * @ingroup lists
 */

#ifndef LIST_H_
#define LIST_H_

typedef struct node_l node_l;
struct node_l {
	node_l *next;
	void *data;
};

typedef struct testdat_ {
	int n;
	char str[1024];
} testdat;


int list_push(node_l **x, void *data);

void *list_pop(node_l **x);

int list_move(node_l **dest, node_l **src);

int list_reverse(node_l **x);

node_l *list_sub(const node_l *x, int pos);

int list_insert(node_l **x, int pos, void *data);

void *list_remove(node_l **x, int pos);

void *list_get(const node_l *x, int n);

size_t list_size(const node_l *x);

node_l *list_join(node_l *a, node_l *b);

void list_print(const node_l *a);

int list_split(node_l **src, node_l **front, node_l **back);

int list_merge(node_l **dest, node_l **a, node_l **b, int cmp(void *, void *));

int list_sort(node_l **x, int cmp(void *, void *));

int list_copy(const node_l *src, node_l **dest);

int list_realloc(node_l *x, size_t sz);

int list_dup(node_l *src, node_l **dest, size_t sz);

void list_delete(node_l **x);

void list_destroy(node_l **x);

int list_foreach(node_l *x, int func(void *));

#endif  /* ! LIST_H_ */
