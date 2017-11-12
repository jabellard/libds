#ifndef ITERATOR_H
#define ITERATOR_H

typedef struct
{
	void *addr;
	size_t obj_size;
}iterator_t;

iterator_t *
iterator_create(void *addr, size_t element_size);

void
iterator_destroy(iterator_t **it);

void*
iterator_curr(iterator_t *it);

void *
iterator_next(iterator_t *it);

void *
iterator_prev(iterator_t *it);

int
iterator_cmp(iterator_t *it1, iterator_t *it2);

int 
iterator_is_before(iterator_t *it1, iterator_t *it2);

int
iterator_is_after(iterator_t *it1, iterator_t *it2);

#endif
