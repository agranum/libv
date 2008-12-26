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
#include "list.h"


/**
 * @file
 * Linked lists implementation.
 * @ingroup lists
 */


/** Adds a new node to the list. Creates a new node with the given data and
 *  pushes it onto the front of the list. The list is not passed in by its head
 *  pointer. Instead the list is passed in as a "reference" pointer to the head
 *  pointer -- this allows as to modify the caller's memory.
 *
 *  This is a constant time operation.
 *
 * @param _x reference to a list (adress might change)
 * @param _data the data item for the new node
 * @returns -1 on error (out of memory), 0 on success
 *
 * @ingroup lists
 */
int list_push(node_l **x, void *data)
{
	node_l *n = NULL;

	assert(x != NULL);

	if((n = (node_l *)malloc(sizeof(node_l))) == NULL) {
		return(-1);
	} else {
		n->data = data;
		n->next = *x;

		*x = n;
		return(0);
	}
}

/** Removes the first node from the list and returns its node data.
 *  Note that \c NULL is a valid node data. Also note that the function will
 *  return \c NULL also for the empty list.
 *
 * This is a constant time operation.
 * @param _x reference to a list (adress might change)
 * @returns a pointer to the data item of the removed node
 *
 * @ingroup lists
 */
void *list_pop(node_l **x)
{
	node_l *n = NULL;
	void *data;
	
	assert(x != NULL);

	if(*x == NULL)
		return(NULL);
		
	n = *x;
	*x  = n->next;
	data = n->data;
	free(n);

	return(data);
}

/** Moves a node from \c _src to \c _dest. The function removes the first node
 *  of \c _src, making it the first node of \c _dest. If \c _src and \c _dest
 *  are equal, the current implementation swaps the first two nodes.
 *  <em>This behaviour may change in future releases</em>, so don't rely on it.
 *
 * @param _dest the destination list
 * @param _src the source list
 * @returns -1 on error (\c _src is the empty list), 0 on success
 *
 * @ingroup lists
 */
int list_move(node_l **dest, node_l **src)
{
	node_l *n = NULL;
	
	assert((dest != NULL) && (src != NULL));

	if(*src == NULL)
		return(-1);

	n = *src;
	*src = n->next;
	n->next = *dest;
	*dest = n;

	return(0);
}

/** Reverses the order of a list. This function reverses the order of a list,
 *  making the last element the first, the second-last the second-first and so
 *  on... This is useful when creating a list with a few list_push() operations,
 *  because elements will then appear in reverse order.
 *
 * @param _x reference to a list (adress might change)
 * @returns 0 on success (currently this function can't possibly fail)
 *
 * @ingroup lists
 */
int list_reverse(node_l **x)
{
	node_l *n = NULL;

	assert(x != NULL);

	while(list_move(&n, x) >= 0);
	*x = n;

	return(0);
}

/** Returns a sub-list of \c _x. This function returns the sub-list of \c _x
 *  starting at position \c _pos+1.
 *
 * @param _x pointer to a list
 * @param _pos sub-list position
 * @returns pointer to the sub-list starting at position \c _pos+1
 *
 * @ingroup lists
 */
node_l *list_sub(const node_l *x, int pos)
{
	int i = 0;
	node_l *n = (node_l *)x;

	assert(x != NULL);

	if(pos < 0)
		return(NULL);

	while(i++ < pos) {
		if((n = n->next) == NULL)
			return(NULL);
	}

	return(n);
}

/** Inserts a new node at the specified position. This function creates a new
 *  node with the supplied \c _data and inserts it at position \c _pos+1 of
 *  \c _x. This is a linear time operation, for obvious reasons.
 *
 * @param _x reference to a list (address might change)
 * @param _pos position of new list node
 * @param _data data item of new list node
 * @returns -1 on error, 0 on success
 *
 * @ingroup lists
 */
int list_insert(node_l **x, int pos, void *data)
{
	node_l *p;

	assert(x != NULL);

	if(pos < 0)
		return(-1);
	        
	if(pos == 0)
	        return(list_push(x, data));

	if((p = list_sub(*x, pos-1)) == NULL)
		return(-1);

	return(list_push(&(p->next), data));
}

/** Removes element at position \c _pos+1 from the list, returning its data.
 *  This function removes the element at position \c _pos+1 from the list.
 *  If \c _pos is 0, its semantics are equivalent to list_pop().
 *
 * @param _x reference to a list (address might change)
 * @param _pos position of element to be removed
 * @returns pointer to data item of removed element
 *
 * @ingroup lists
 */
void *list_remove(node_l **x, int pos)
{
	node_l *current = *x;
	node_l *next = NULL;
	void *data;

	assert(x != NULL);

	if(pos < 0)
	        return(NULL);
	
	if(*x == NULL)
	        return(NULL);

	if(pos == 0)
	        return(list_pop(x));

	while(--pos) {
		current = current->next;
	}

	if((next = current->next) == NULL)
	        return(NULL);
	        
	data = next->data;
	current->next = next->next;

	free(next);
	return(data);
}

/** Get data item of a list element. This function returns a pointer to the
 *  data item of the list element at position \c _pos+1. It will return \c NULL
 *  on error. Note that \c NULL is also a valid data item.
 *
 * @param _x pointer to a list
 * @param _pos position of a list element
 * @returns \c NULL on error, pointer to a data item on success
 *
 * @ingroup lists
 */
void *list_get(const node_l *x, int pos)
{
	int i = 0;

	if(pos < 0)
	        return(NULL);

	while(x != NULL) {
		if(pos == i++)
		        return(x->data);
		x = x->next;
	}

	return(NULL);
}

/** Computes the size of a list. This function computes the size of a list.
 *  The code should be relatively self-explanatory.
 *
 * @param _x pointer to a list
 * @returns the size of the list
 *
 * @ingroup lists
 */
size_t list_size(const node_l *x)
{
	size_t s = 0;

	for(; x != NULL; s++, x = x->next);
	return(s);
}

/** Appends the list \c _src to the list \c _dest. If \c _dest is empty, it
 *  simply returns \c _src, else it makes the last element of \c _dest point to
 *  the first element of \c _src, modifying \c _dest.
 *
 * @param _dest the destination list
 * @param _src the source list
 * @returns a pointer to \c _dest, with _src appended
 *
 * @ingroup lists
 */
node_l *list_join(node_l *dest, node_l *src)
{
	node_l *n = dest;

	if(dest == NULL)
	        return(src);

	if(src == NULL)
	        return(dest);

	while(dest->next != NULL)
	        dest = dest->next;
	        
	dest->next = src;

	return(n);
}

/** Prints list elements. <em>This function will change or vanish in future
 *  releases</em>, so don't use it. It will be replaced by list_foreach(), which
 *  shall execute a user-supplied function for each list element.
 *
 * @param _a pointer to a list
 * @returns nothing
 *
 * @ingroup lists
 */
void list_print(const node_l *a)
{
	testdat *foo;
	int i = 0;

        while(a != NULL) {
		foo = (testdat *)a->data;
		printf("%03d: %3d - %s\n", i++, foo->n, foo->str);
		a = a->next;
        }
}

/** Splits a list in two equal-sized sublists. This function uses the
 *  hare-and-tortoise approach, also known as the Classic Floyd's Cycle Finding
 *  Algorithm to split a list in two rougly equal-sized sublists (see
 *  http://en.wikipedia.org/wiki/Floyd's_cycle-finding_algorithm for details).
 *  You should make sure that the list doesn't contain cycles using
 *  list_cyclic() (to be implemented).
 *
 * @param _src reference to a list (will be replaced by \c NULL)
 * @param _front reference to a list (will be replaced by the first sublist)
 * @param _back reference to a list (will be replaced by the second sublist)
 * @returns 0 on success (this function cannot possibly fail)
 *
 * @ingroup lists
 */
/* hare and tortoise approach / classic Floyd’s Cycle Finding Algorithm */
int list_split(node_l **src, node_l **front, node_l **back)
{
	node_l *fast, *slow;

	assert((src != NULL) && (front != NULL) && (back != NULL));

	if((*src == NULL) || ((*src)->next == NULL)) {
		*back = NULL;
	} else {
		slow = *src;
		fast = slow->next;
		
		while(fast != NULL) {
			fast = fast->next;
			if(fast != NULL) {
				slow = slow->next;
				fast = fast->next;
			}
		}
		*back = slow->next;
		slow->next = NULL;
	}

	*front = *src;
	*src = NULL;
	return(0);
}

/** Merges two lists using \c _cmp to compare nodes. This function merges the
 *  two lists \c _a and \c _b into the new list \c _dest using the function
 *  \c _cmp to decide which element to choose next, the first element of \c _a
 *  or the first element of \c _b. \c _cmp is a comparison function and shall
 *  return -1, 0 or 1 if _a->data is less than, equal to or greater than
 *  _b->data.
 *
 *  This function is mainly used in list_sort() to merge two already sorted
 *  sublists together.
 *
 * @param _dest a reference to the resulting list
 * @param _a reference to list A
 * @param _b reference to list B
 * @param _cmp comparison function, takes two data items and returns \c int
 *
 * @ingroup lists
 */
int list_merge(node_l **dest, node_l **a, node_l **b, int cmp(void *, void *))
{
	node_l *ret = NULL;
	node_l **last = &ret;

	assert((dest != NULL) && (a != NULL) && (b != NULL));

	for(;;) {
		if(*a == NULL) {
			*last = *b;
			break;
		}

		if(*b == NULL) {
		        *last = *a;
		        break;
		}
		
		if(cmp((*a)->data, (*b)->data) <= 0) {
			list_move(last, a);
		} else {
			list_move(last, b);
		}
		
		last = &((*last)->next);
	}

	*dest = ret;
	return(0);
}

/** Sorts a list. This function sorts a list using mergesort (see
 *  http://en.wikipedia.org/wiki/Merge_sort for details). The comparison
 *  function \c _cmp determines the sort order: \c _cmp takes two data items
 *  A and B and returns -1, 0 or 1 if A is less than, equal to or greater than
 *  B respectively.
 *
 * @param _x reference to a list (address might change)
 * @param _cmp comparison function
 *
 * @ingroup lists
 */
int list_sort(node_l **x, int cmp(void *, void *))
{
	node_l *l, *r;

	assert(x != NULL);

	if(*x == NULL || (*x)->next == NULL)
	        return(0);

	list_split(x, &l, &r);

	list_sort(&l, cmp);
	list_sort(&r, cmp);
	
	list_merge(x, &l, &r, cmp);

	return(0);
}

/** Makes a copy of a list, not including its data items (see list_dup()).
 *  This function creates a copy of a list, \e NOT
 *  duplicating the list's data items. The resulting list uses the same data
 *  items as the original list. If you want to duplicate the data items too,
 *  use list_dup().
 *
 *  On error, -1 is returned and \c _dest is the copy of \c _src up to the
 *  point where no more memory was available. <em>In all cases, you have to
 *  list_free() \c _dest.</em>
 *
 * @param _src pointer to a list
 * @param _dest reference to the list copy
 * @returns -1 on error (out of memory), 0 on success
 *
 * @ingroup lists
 */
int list_copy(const node_l *src, node_l **dest)
{
	assert(src != NULL);

	*dest = NULL;

	while(src) {
		if(list_push(dest, src->data)) {
			return(-1);
		} else {
			src = src->next;
		}
	}
	return(list_reverse(dest));
}

/** Re-allocates storage for a list. This function re-allocates all data items
 *  in \c _x, assigning the new address to its nodes.
 *
 *  This function is mainly used in list_dup().
 *
 * @param _x pointer to a list
 * @param _sz size of the list's data items
 * @returns -1 on error (out of memory), 0 on success
 *
 * @ingroup lists
 */
int list_realloc(node_l *x, size_t sz)
{
	void *data;

	assert(x != NULL);

	while(x) {
		if((data = malloc(sz)) == NULL) {
			return(-1);
		} else {
			x->data = memcpy(data, x->data, sz);
			x = x->next;
		}
	}
	return(0);
}

/** Duplicates a list, including its data items (see list_copy()).
 *  Makes a copy of \c _src, saving a pointer to the result in \c _dest.
 *
 *  This function merely calls list_copy(), followed by list_realloc().
 *
 * @param _src pointer to a list
 * @param _dest reference to a list (address might change)
 * @param _sz size of the list's data items
 * @returns -1 on error (not enough memory), 0 on success
 *
 * @ingroup lists
 */
int list_dup(node_l *src, node_l **dest, size_t sz)
{
	assert(src != NULL);

	if(list_copy(src, dest) < 0)
	        return(-1);

	if(list_realloc(*dest, sz) < 0)
	        return(-1);
	        
	return(0);
}

/** Deletes a list, not including its data items. This function deletes
 *  a list, not including its data items. This will result in memory leaks if
 *  you don't have a copy of the list, because all references to the data items
 *  are lost.
 *
 * @param _x reference to a list (address might change)
 * @returns nothing
 *
 * @ingroup lists
 */
void list_delete(node_l **x)
{
	assert(x != NULL);
	
	while(*x) {
		list_pop(x);
	}
}

/** Destroys a list, including its data items. This function deletes a list,
 *  including its data items. After a call to list_destroy(), the list and
 *  its data items will be lost. If you have a copy of the list, it will be
 *  invalid (use list_delete() on it).
 *
 * @param _x reference to a list (address might change)
 * @returns nothing
 *
 * @ingroup lists
 */
void list_destroy(node_l **x)
{
	assert(x != NULL);

	while(*x) {
		free(list_pop(x));
	}
}

/** Auxiliary function for examples. This function creates a short list, see
 *  examples for details.
 *
 *  FIXME: add examples
 * @returns a pointer to the example list
 *
 * @ingroup lists
 */
/* auxiliary function to create a short list */
node_l *list_create123()
{
	node_l *n = NULL;
	testdat *a=malloc(sizeof(testdat)),
	        *b=malloc(sizeof(testdat)),
	        *c=malloc(sizeof(testdat));

	a->n = 1;
	strcpy(a->str, "foo");
	b->n = 2;
	strcpy(b->str, "bar");
	c->n = 3;
	strcpy(c->str, "snafu");

	list_push(&n, a);
	list_push(&n, b);
	list_push(&n, c);
	return(n);
}

/** Auxiliary function for examples. This function compares to data items and
 *  returns -1, 0 or 1 if the first argument is less than, equal to or greater
 *  than the second argument respectively.
 *
 * @param _a data item
 * @param _b data item
 * @returns -1, 0 or 1
 *
 * @ingroup lists
 */
/* auxiliary function for testing list_sort() */
int cmp(void *a, void *b)
{
	testdat *la = (testdat *)a, *lb = (testdat *)b;

	if(la->n < lb->n) return(-1);
	if(la->n > lb->n) return(1);
	return(0);
}

/** Calls a function for each list element. list_foreach() executes the
 *  user-supplied function \c _func for each element of the list pointed to by
 *  \c _x, passing the element's data to \c _func. list_foreach() returns the
 *  sum of all return values of \c _func.
 *  This is useful if you want to count all list elements that match to certain
 *  criteria, but you may also use list_foreach() for much simpler operations,
 *  e.g. print functions.
 *
 * @param _x a list
 * @param _func a function
 * @returns the sum of \c _func return values
 *
 * @ingroup lists
 */
int list_foreach(node_l *x, int func(void *))
{
	int ret = 0;

	while(x) {
	        ret *= func(x->data);
		x = x->next;
	}
	        
	return(ret);
}

