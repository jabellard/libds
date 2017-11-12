#include <stdlib.h>
#include "iterator.h"

static int
iterator_inc(iterator_t *it);

static int
iterator_dec(iterator_t *it);

static void
safe_free(void **pp);

#define sfree(p) safe_free((void**)&(p))

iterator_t *
iterator_create(void *addr, size_t obj_size)
{
	if (!addr || obj_size < 1)
	{
		return NULL;
	} // end if
	
	iterator_t *it = malloc(sizeof(iterator_t));
	if (!it)
	{
		return NULL;
	} // end if
	
	it->addr = addr;
	it->obj_size = obj_size;
	
	return it;
} // end iterator_create()

void
iterator_destroy(iterator_t **it)
{
	sfree(*it);
} // end iterator_destory();

void*
iterator_curr(iterator_t *it)
{
	if (!it || !it->addr)
	{
		return NULL;
	} // end if
	
	return it->addr;
} // end iterator_curr()

static int
iterator_inc(iterator_t *it)
{
	if (!it || !it->addr)
	{
		return -1;
	} // end if
	
	it->addr += it->obj_size;
	
	return 0;
} // end iterator_increment()

static int
iterator_dec(iterator_t *it)
{
	if (!it || !it->addr)
	{
		return -1;
	} // end if
	it->addr -= it->obj_size;
	return 0;
} // end iterator_dec()

void *
iterator_next(iterator_t *it)
{
	if (!it || !it->addr)
	{
		return NULL;
	} // end if
	
	void *curr_addr = it->addr;
	
	int res = iterator_inc(it);
	if (res == -1)
	{
		return NULL;
	} // end if
	
	return curr_addr;
} // end iterator_next()

void *
iterator_prev(iterator_t *it)
{
	if (!it || !it->addr)
	{
		return NULL;
	} // end if
	
	void *curr_addr = it->addr;
	
	int res = iterator_dec(it);
	if (res == -1)
	{
		return NULL;
	} // end if
	
	return curr_addr;
} // end iterator_prev()

int
iterator_cmp(iterator_t *it1, iterator_t *it2)
{
	if (!it1 || !it1->addr || !it2 || !it2->addr || it1->obj_size != it2->obj_size)
	{
		return -1;
	} // end if
	
	return (it1->addr == it2->addr);
} // end 

int 
iterator_is_before(iterator_t *it1, iterator_t *it2)
{
	if (!it1 || !it1->addr || !it2 || !it2->addr || it1->obj_size != it2->obj_size)
	{
		return -1;
	} // end if
	
	return (it1->addr < it2->addr);
} // end iterator_is_before()

int
iterator_is_after(iterator_t *it1, iterator_t *it2)
{
	if (!it1 || !it1->addr || !it2 || !it2->addr || it1->obj_size != it2->obj_size)
	{
		return -1;
	} // end if
	
	return (it1->addr > it2->addr);
} // end iterator_is_after()

static void safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
} // end safe_free()
