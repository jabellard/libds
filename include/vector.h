#ifndef VECTOR_H
#define VECTOR_H

#include "iterator.h"

#define VECTOR_MIN_CAPACITY 2
#define VECTOR_GROWTH_FACTOR 2

typedef iterator_t vector_iterator_t;
typedef int (*data_dtor_func_t)(void *);
typedef void * (*data_ctor_func_t)(void *);

typedef struct
{
	data_ctor_func_t ctor;
	data_dtor_func_t dtor;
}vector_data_ops_t;

typedef struct
{
	size_t capacity;
	size_t size;
	size_t obj_size;
	void *data;
	vector_data_ops_t data_ops;
}vector_t;

vector_t *
vector_create(size_t capacity, size_t obj_size, vector_data_ops_t ops);

vector_t *
vector_copy(vector_t *src);

vector_t *
vector_copy_assign(vector_t *dest, vector_t *src);

int
vector_destroy(vector_t *v);

int
vector_insert_at(vector_t *v, size_t i, void *obj);

int
vector_push_back(vector_t *v, void *obj);

int
vector_push_front(vector_t *v, void *obj);

int
vector_set(vector_t *v, size_t i, void *obj);

int
vector_set_back(vector_t *v, void *obj);

int
vector_set_front(vector_t *v, void *obj);

int
vector_delete_at(vector_t *v, size_t i);

int
vector_pop_back(vector_t *v);

int
vector_pop_front(vector_t *v);

void *
vector_get(vector_t *v, size_t i);

void *
vector_front(vector_t *v);

void *
vector_back(vector_t *v);

size_t
vector_size(vector_t *v);

size_t
vector_capacity(vector_t *v);

vector_iterator_t *
vector_get_iterator(vector_t *v, size_t i);

vector_iterator_t *
vector_begin(vector_t *v);

vector_iterator_t *
vector_end(vector_t *v);

size_t
vector_iterator_index(vector_t *v, vector_iterator_t *it);

int 
vector_shrink_to_fit(vector_t *v);

#endif
