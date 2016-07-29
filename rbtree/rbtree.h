#pragma once

#include <stdbool.h>
#include <stdint.h> /* uint\d+_t */

#include "attributes.h"
#include "defs.h"

#define MAINTAIN_FIRST_LAST

#define RBTREE_INSERT_ON_DUP_KEY_PRESERVE (1<<1)

typedef void (*TravFunc)(const void *, void *);    /* Foreach callback (value) */

typedef enum
{
    IN_ORDER,  /* Infixed   */
    PRE_ORDER, /* Prefixed  */
    POST_ORDER /* Postfixed */
} TraverseMode;

typedef struct _RBTree RBTree;
typedef struct _RBTreeNode RBTreeNode;

void rbtree_clear(RBTree *) NONNULL();
void rbtree_destroy(RBTree *) NONNULL();
bool rbtree_empty(RBTree *) NONNULL();
bool rbtree_exists(RBTree *, const void *) NONNULL(1);
bool rbtree_get(RBTree *, const void *, void **) NONNULL(1, 3);
bool rbtree_insert(RBTree *, uint32_t, const void *, void *, void **) NONNULL(1);
bool rbtree_max(RBTree *, const void **, void **) NONNULL(1);
bool rbtree_min(RBTree *, const void **, void **) NONNULL(1);
RBTree *rbtree_new(CmpFunc, DupFunc, DupFunc, DtorFunc, DtorFunc) NONNULL(1) WARN_UNUSED_RESULT;
bool rbtree_remove(RBTree *, const void *, bool) NONNULL(1);
bool rbtree_replace(RBTree *, const void *, void *, bool) NONNULL(1);
void rbtree_traverse(RBTree *, TraverseMode, TravFunc) NONNULL();

#if 0
#if defined(MAINTAIN_FIRST_LAST) && !defined(WITHOUT_ITERATOR)
#include "iterator.h"

void rbtree_to_iterator(Iterator *, RBTree *);
#endif /* MAINTAIN_FIRST_LAST && !WITHOUT_ITERATOR */
#endif
