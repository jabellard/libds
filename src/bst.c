#include <stdlib.h>
#include "bst.h"

static void
bst_destroy_nodes(bst_node_t *root);

static bst_node_t **
bst_find_min(bst_node_t **root);

static bst_node_t *
_bst_delete(bst_node_t **root, void *data);

static void
safe_free(void **pp);

#define sfree(p) safe_free((void**)&(p))

bst_node_t *
bst_node_create(void *data)
{
	bst_node_t *n = malloc(sizeof(bst_node_t));
	if (!n)
	{
		return NULL;
	} // end if
	
	n->container = NULL;
	n->data = data;
	n->left = NULL;
	n->right = NULL;
	
	return n;
} // end bst_node_create();

void
bst_node_destroy(bst_node_t *n)
{
	if (!n)
	{
		return;
	} // end if
	
	if (n->data)
	{
		if (n->container && n->container->data_dtor)
		{
			n->container->data_dtor(n->data);
		} // end if
	} // end if
	
	sfree(n);
	
	return;
} // end bst_node_destroy()

bst_t *
bst_create(bst_cmp_func_t cmp, bst_data_ctor_func_t ctor, bst_data_dtor_func_t dtor)
{
	if (!cmp || !ctor || !dtor)
	{
		return NULL;
	} // end if
	
	bst_t *bst = malloc(sizeof(bst_t));
	if (!bst)
	{
		return NULL;
	} // end if
	
	bst->root = NULL;
	bst->size = 0;
	bst->data_cmp = cmp;
	bst->data_ctor = ctor;
	bst->data_dtor = dtor;
	
	return bst;
} // end bst_create()

static void
bst_destroy_nodes(bst_node_t *root)
{
	if (!root)
	{
		return;
	} // end if
	
	bst_destroy_nodes(root->left);
	bst_destroy_nodes(root->right);
	bst_node_destroy(root);
	
	return;
} // end bst_destroy_nodes()

void
bst_destroy(bst_t *bst)
{
	if (!bst || !bst->root || !bst->data_dtor)
	{
		return;
	} // end if
	
	bst_destroy_nodes(bst->root);
	
	sfree(bst);
	return;
} // end bst_destroy()

int
bst_insert(bst_t **bst, bst_node_t *n, int flags)
{
	if (!*bst || !(*bst)->data_cmp || !(*bst)->data_dtor || !n)
	{
		return -1;
	} // end if
	
	n->left = NULL;
	n->right = NULL;
	
	bst_node_t **curr = &((*bst)->root);
	
	if (!(*bst)->root)
	{
		*curr = n;
		n->container = *bst;
		((*bst)->size)++;
		return 0;
	} // end if
	else
	{
		curr = &((*bst)->root);
	} // end else
	
	while (*curr)
	{
		if (((*bst)->data_cmp(n->data, (*curr)->data)) == 0)
		{
			if (flags == 1)
			{
				(*bst)->data_dtor((*curr)->data);
				
				(*curr)->data = n->data;
				return 0;
			} // end if
			else
			{
				return -1;
			} // end else
		} // end if
		else if (((*bst)->data_cmp(n->data, (*curr)->data)) < 0)
		{
			curr = &((*curr)->left);
		} // end else if
		else
		{
			curr = &((*curr)->right);
		} // end else
	} // end while
	
	n->container = *bst;
	*curr = n;
	((*bst)->size)++;
	
	return 0;
} // end bst_insert()

bst_node_t *
_bst_search(bst_t *bst, void *data)
{
	if (!bst || !bst->root || !bst->data_cmp || !data)
	{
		return NULL;
	} // end if
	
	bst_node_t *curr = bst->root;
	
	while (curr)
	{
		int cmp_res = bst->data_cmp(data, curr->data);
		if (cmp_res == 0)
		{
			return curr;
		} // end if
		else if (cmp_res < 0)
		{
			curr = curr->left;
		} // end else if
		else
		{
			curr = curr->right;
		} // end else
	} // end while
	
	return NULL;
} // end _bst_search()

void *
bst_search(bst_t *bst, void *data)
{
	bst_node_t *n = _bst_search(bst, data);
	if (n)
	{
		return n->data;
	} // end if
	
	return (void *)n;
} // end bst_search()

static bst_node_t **
bst_find_min(bst_node_t **root)
{
	if (!root || !(*root))
	{
		return NULL;
	} // end if
	
	bst_node_t **curr = root;
	
	bst_node_t **next = root;
	
	while (*next)
	{
		curr = next;
		
		next = &((*next)->left);
	} // end while
	
	return curr;
} // end bst_find_min()

static bst_node_t *
_bst_delete(bst_node_t **root, void *data)
{
	if (!root || !(*root) || !(*root)->container || !(*root)->container->data_cmp)
	{
		return NULL;
	} // end if
	
	else if ((*root)->container->data_cmp(data, (*root)->data) < 0)
	{
		(*root)->left = _bst_delete(&((*root)->left), data);
	} // end else if
	else if ((*root)->container->data_cmp(data, (*root)->data) > 0)
	{
		(*root)->right = _bst_delete(&((*root)->right), data);
	} // end else if
	else
	{
		if (!(*root)->left && !(*root)->right)
		{
			bst_node_destroy((*root));
			((*root)->container->size)--;
			return NULL;
		} // end if
		else if (!(*root)->left)
		{
			bst_node_t **tmp = (root);
			root = &((*root)->right);
			bst_node_destroy(*tmp);
			((*root)->container->size)--;
		} // end else if
		else if (!(*root)->right)
		{
			bst_node_t **tmp = (root);
			root = &((*root)->left);
			bst_node_destroy(*tmp);
			((*root)->container->size)--;
		} // end else if
		else
		{
			bst_node_t **tmp = bst_find_min(&((*root)->right));
			
			void *obj = (*root)->container->data_ctor((*tmp)->data);
			(*root)->data = obj; 
			(*root)->right = _bst_delete(&((*root)->right), (*tmp)->data);
		} // end else
	} // end else
	
	return (*root);
} // end _bst_delete()

bst_node_t *
bst_delete(bst_t **bst, void *data)
{
	if (!bst || !(*bst) || !(*bst)->root)
	{
		return NULL;
	} // end if
	
	return _bst_delete(&((*bst)->root), data);
} // end bst_delete()

static void safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
} // end safe_free()
