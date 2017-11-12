#include <string.h>
#include <stdlib.h>
#include "vector.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static int
vector_move_right(vector_t *v, size_t i);

static int
vector_move_left(vector_t *v, size_t i);

static int
vector_resize(vector_t *v , size_t new_capacity);

static int 
vector_resize_up(vector_t *v);

static void *
vector_addr_at_index(vector_t *v, size_t i);

static int 
vector_assign(vector_t *v, size_t i, void *obj);

static void
safe_free(void **pp);

#define sfree(p) safe_free((void**)&(p))

vector_t *
vector_create(size_t capacity, size_t obj_size, vector_data_ops_t ops)
{
	vector_t *v = malloc(sizeof(vector_t));
	if (!v)
	{
		return NULL;
	} // end if
	
	size_t _capacity = MAX(VECTOR_MIN_CAPACITY, capacity);
	v->data = malloc(_capacity * obj_size);
	if (!v->data)
	{
		sfree(v);
		return NULL;
	} // end if
	
	v->capacity = _capacity;
	v->size = 0;
	v->obj_size = obj_size;
	v->data_ops = ops;
	
	return v;
} // end vector_create()

vector_t *
vector_copy(vector_t *src)
{
	if (!src || !src->data)
	{
		return NULL;
	} // end if
	
	vector_t *dest = vector_create(src->capacity, src->obj_size, src->data_ops);
	if (!dest)
	{
		return NULL;
	} // end if
	
	vector_iterator_t *start = vector_begin(src);
	vector_iterator_t *end = vector_end(src);
	if (!start || !end)
	{
		vector_destroy(dest);
		return NULL;
	} // end if
	
	while (!iterator_cmp(start, end))
	{
		void *addr = iterator_next(start);
		if (!addr)
		{
			vector_destroy(dest);
			return NULL;
		} // end if
		
		int res = vector_push_back(dest, addr);
		if (res == -1)
		{
			vector_destroy(dest);
			return NULL;
		} // end 
	} // end while
	
	return dest;
} // end vector_copy()

vector_t *
vector_copy_assign(vector_t *dest, vector_t *src)
{
	if (!dest || !dest->data || !src || !src->data)
	{
		return dest;
	} // end if
	
	int res = vector_destroy(dest);
	if (res == -1)
	{
		return dest;
	} // end if
	
	dest = vector_copy(src);
	return dest;
} // end vector_copy_assign()

int
vector_destroy(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	vector_iterator_t *start = vector_begin(v);
	vector_iterator_t *end = vector_end(v);
	if (!start || !end)
	{
		return -1;
	} // end if
	
	while (!iterator_cmp(start, end))
	{
		void *addr = iterator_next(start);
		if (!addr)
		{
			return -1;
		} // end if
		
		int res = v->data_ops.dtor(addr);
		if (res == -1)
		{
			return -1;
		} // end 
	} // end while
	
	sfree(v->data);
	sfree(v);
	
	return 0;
} // end vector_destroy()

int
vector_insert_at(vector_t *v, size_t i, void *obj)
{
	if (!v || !v->data || i > v->size || !obj)
	{
		return -1;
	} // end if
	
	int res = 0;
	
	if (v->size == v->capacity)
	{
		res = vector_resize_up(v);
		if (res == -1)
		{
			return -1;
		} // end if
	} // end if
	
	res = vector_move_right(v, i);
	if (res == -1)
	{
		return -1;
	} // end if
	
	void *_obj = v->data_ops.ctor(obj);
	
	res = vector_assign(v, i, _obj);
	if (res == -1)
	{
		sfree(_obj);
		return -1;
	} // end if
	
	sfree(_obj);
	
	(v->size)++;
	return 0;
} // end vector_insert_at()

int
vector_push_back(vector_t *v, void *obj)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	return vector_insert_at(v, v->size, obj);
} // end vector_push_back()

int
vector_push_front(vector_t *v, void *obj)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	return vector_insert_at(v, 0, obj);
} // end vector_push_front()

int
vector_set(vector_t *v, size_t i, void *obj)
{
	if (!v || !v->data || i >= v->size || !obj)
	{
		return -1;
	} // end if
	
	return vector_assign(v, i, obj);
} // end vector_set()

int
vector_set_back(vector_t *v, void *obj)
{
	if (!v || !v->data || !obj)
	{
		return -1;
	} // end if
	
	return vector_set(v, v->size - 1, obj);
} // end vector_set_back()

int
vector_set_front(vector_t *v, void *obj)
{
	if (!v || !v->data || !obj)
	{
		return -1;
	} // end if
	
	return vector_set(v, 0, obj);
} // end vector_set_front()

int
vector_delete_at(vector_t *v, size_t i)
{
	if (!v || !v->data || i >= v->size)
	{
		return -1;
	} // end if
	
	void *del_addr = vector_addr_at_index(v, i);
	if (!del_addr)
	{
		return -1;
	} // end if

	int res = v->data_ops.dtor(del_addr);
	if (res == -1)
	{
		return -1;
	} // end 	
	
	res = vector_move_left(v, i);
	if (res == -1)
	{
		return -1;
	} // end if
	
	(v->size)--;
	return 0;
} // end vector_delete_at()

int
vector_pop_back(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	return vector_delete_at(v, v->size - 1);
} // end vector_pop_back()

int
vector_pop_front(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	return vector_delete_at(v, 0);
} // end vector_pop_front()

void *
vector_get(vector_t *v, size_t i)
{
	if (!v || !v->data || i >= v->size)
	{
		return NULL;
	} // end if
	
	return vector_addr_at_index(v, i);
} // end vector_get()

void *
vector_front(vector_t *v)
{
	if (!v)
	{
		return NULL;
	} // end if
	
	return vector_get(v, 0);
} // end vector_front()

void *
vector_back(vector_t *v)
{
	if (!v)
	{
		return NULL;
	} // end if
	
	return vector_get(v, v->size - 1);
} // end vector_back()

size_t
vector_size(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	return v->size;
} // end vector_size()

size_t
vector_capacity(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	return v->capacity;
} // end vector_capacity()

vector_iterator_t *
vector_get_iterator(vector_t *v, size_t i)
{
	if (!v || !v->data || i > v->size)
	{
		return NULL;
	} // end if
	
	vector_iterator_t *it = iterator_create(vector_addr_at_index(v, i), v->obj_size);
	return it;
} // end vector_get_iterator()

vector_iterator_t *
vector_begin(vector_t *v)
{
	if (!v || !v->data)
	{
		return NULL;
	} // end if
	
	return vector_get_iterator(v, 0);
} // end vector_begin()

vector_iterator_t *
vector_end(vector_t *v)
{
	if (!v || !v->data)
	{
		return NULL;
	} // end if
	
	return vector_get_iterator(v, v->size);
} // end vector_end()

size_t
vector_iterator_index(vector_t *v, vector_iterator_t *it)
{
	if (!v || !v->data || !it || !it->addr || v->obj_size != it->obj_size)
	{
		return -1; // potential problem
	} // end if
	
	return (it->addr - v->data) / (v->obj_size);
} // end vector_iterator_index()

static int
vector_move_right(vector_t *v, size_t i)
{
	size_t mv_bytes = (v->size - i) * v->obj_size;
	if (mv_bytes == 0)
	{
		return 0;
	} // end if
	
	void *mv_addr = vector_addr_at_index(v, i);
	if (!mv_addr)
	{
		return -1;
	} // end if
	
	memmove(mv_addr + v->obj_size, mv_addr, mv_bytes);
	return 0;
} // end vector_move_right()

static int
vector_move_left(vector_t *v, size_t i)
{
	size_t mv_bytes = (v->size - i - 1) * v->obj_size;
	if (mv_bytes == 0)
	{
		return 0;
	} // end if
	
	void *mv_addr = vector_addr_at_index(v, i);
	if (!mv_addr)
	{
		return -1;
	} // end if
	
	memmove(mv_addr, mv_addr + v->obj_size, mv_bytes);
	return 0;
} // end vector_move_left()

static int
vector_resize(vector_t *v , size_t new_capacity)
{
	if (!v || !v->data || new_capacity < VECTOR_MIN_CAPACITY)
	{
		return -1;
	} // end if
	
	void *old_data = v->data;
	
	v->data = malloc(new_capacity * v->obj_size);
	if (!v->data)
	{
		return -1;
	} // end if
	
	memcpy(v->data, old_data, v->size * v->obj_size);
	
	v->capacity = new_capacity;
	
	free(old_data);
	
	return 0;
} // end vector_resize()

static int 
vector_resize_up(vector_t *v)
{
	 if (!v || !v->data)
	 {
	 	return -1;
	 } // end if
	 
	 size_t new_capacity = v->capacity * VECTOR_GROWTH_FACTOR;
	 
	 return vector_resize(v, new_capacity);
} // end vector_resize_up()

int 
vector_shrink_to_fit(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	size_t new_capacity = MAX(v->size, VECTOR_MIN_CAPACITY);
	
	return vector_resize(v, new_capacity);
} // end vector_shrink_to_fit()

static void *
vector_addr_at_index(vector_t *v, size_t i)
{
	if (!v || !v->data || i > v->size)
	{
		return NULL;
	} // end if
	
	return (v->data + (i * v->obj_size));
} // end vector_addr_at_index()

static int 
vector_assign(vector_t *v, size_t i, void *obj)
{
	if (!v || i > v->size || !obj)
	{
		return -1;
	} // end if
	
	void *addr = vector_addr_at_index(v, i);
	if (!addr)
	{
		return -1;
	} // end if
	
	memcpy(addr, obj, v->obj_size); // potential error
	return 0;
} // end vector_assign()

static void
safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
} // end safe_free()
