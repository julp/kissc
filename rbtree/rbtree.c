/**
 * @file rbtree/rbtree.c
 * @brief red black tree
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "utils.h"
#include "rbtree.h"

#define clone(duper, value) \
    (NULL == duper ? value : duper(value))

typedef enum {
    BLACK = 0,
    RED   = 1
} RBTreeColor;

struct _RBTreeNode {
    const void *key;
    void *value;
    RBTreeColor color;
    RBTreeNode *left;
    RBTreeNode *right;
    RBTreeNode *parent;
};

struct _RBTree {
    RBTreeNode nil;
    RBTreeNode *root;
#ifdef MAINTAIN_FIRST_LAST
    RBTreeNode *first;
    RBTreeNode *last;
#endif /* MAINTAIN_FIRST_LAST */
    CmpFunc cmp_func;
    DtorFunc key_dtor;
    DtorFunc value_dtor;
    DupFunc key_duper;
    DupFunc value_duper;
};

static RBTreeNode *rbtreenode_new(const void *key, void *value)
{
    RBTreeNode *node;

    node = malloc(sizeof(*node));
    node->right = node->left = node->parent = NULL;
    node->color = RED;
    node->key = key;
    node->value = value;

    return node;
}

static RBTreeNode *rbtreenode_nil_init(RBTreeNode *nil)
{
    nil->right = nil->left = nil->parent = nil;
    nil->color = BLACK;
    nil->key = NULL;
    nil->value = NULL;

    return nil;
}

RBTree *rbtree_new(
    CmpFunc cmp_func,
    DupFunc key_duper,
    DupFunc value_duper,
    DtorFunc key_dtor,
    DtorFunc value_dtor
) /* NONNULL(1) WARN_UNUSED_RESULT */ {
    RBTree *tree;

    assert(NULL != cmp_func);

    tree = malloc(sizeof(*tree));
    tree->key_duper = key_duper;
    tree->value_duper = value_duper;
    tree->root = rbtreenode_nil_init(&tree->nil);
#ifdef MAINTAIN_FIRST_LAST
    tree->first = tree->last = &tree->nil;
#endif
    tree->cmp_func = cmp_func;
    tree->key_dtor = key_dtor;
    tree->value_dtor = value_dtor;

    return tree;
}

/**
 * Is the tree empty?
 *
 * @param tree the RB tree
 *
 * @return true if the tree does not contain any element
 */
bool rbtree_empty(RBTree *tree) /* NONNULL() */
{
    assert(NULL != tree);

    return &tree->nil == tree->root;
}

static void rbtree_rotate_left(RBTree *tree, RBTreeNode *node) /* NONNULL() */
{
    RBTreeNode *p;

    p = node->right;
    node->right = p->left;
    if (p->left != &tree->nil) {
        p->left->parent = node;
    }
    p->parent = node->parent;
    if (node->parent == &tree->nil) {
        tree->root = p;
    } else if (node == node->parent->left) {
        node->parent->left = p;
    } else {
        node->parent->right = p;
    }
    p->left = node;
    node->parent = p;
}

static void rbtree_rotate_right(RBTree *tree, RBTreeNode *node) /* NONNULL() */
{
    RBTreeNode *p;

    p = node->left;
    node->left = p->right;
    if (p->right != &tree->nil) {
        p->right->parent = node;
    }
    p->parent = node->parent;
    if (node->parent == &tree->nil) {
        tree->root = p;
    } else if (node == node->parent->right) {
        node->parent->right = p;
    } else {
        node->parent->left = p;
    }
    p->right = node;
    node->parent = p;
}

static RBTreeNode *rbtreenode_max(RBTree *tree, RBTreeNode *node) /* NONNULL() */
{
    while (node->right != &tree->nil) {
        node = node->right;
    }

    return node;
}

static RBTreeNode *rbtreenode_previous(RBTree *tree, RBTreeNode *node) /* NONNULL() */
{
    if (node->left != &tree->nil) {
        return rbtreenode_max(tree, node->left);
    } else {
        RBTreeNode *y;

        y = node->parent;
        while (y != &tree->nil && node == y->left) {
            node = y;
            y = y->parent;
        }

        return y;
    }
}

static RBTreeNode *rbtreenode_min(RBTree *tree, RBTreeNode *node) /* NONNULL() */
{
    while (node->left != &tree->nil) {
        node = node->left;
    }

    return node;
}

static RBTreeNode *rbtreenode_next(RBTree *tree, RBTreeNode *node) /* NONNULL() */
{
    if (node->right != &tree->nil) {
        return rbtreenode_min(tree, node->right);
    } else {
        RBTreeNode *y;

        y = node->parent;
        while (y != &tree->nil && node == y->right) {
            node = y;
            y = y->parent;
        }

        return y;
    }
}

/**
 * Insert a new pair key/value into the tree or replace the value associated to the key
 * if the last one is already in the tree
 *
 * @param tree the RB tree
 * @param flags a mask of the following options:
 *   - RBTREE_INSERT_ON_DUP_KEY_PRESERVE: if the key already exists, do not overwrite its current value
 * @param key the key of the element to insert or replace
 * @param value the value to insert or replace
 * @param odlvalue if this pointer is not NULL, it will receive the previous value associated to the key
 *
 * @return true if any change took place (a new node was inserted or the value was overwritten)
 */
bool rbtree_insert(RBTree *tree, uint32_t flags, const void *key, void *value, void **oldvalue) /* NONNULL(1) */
{
    int cmp;
    RBTreeNode *y, *x, *new;

    y = &tree->nil;
    x = tree->root;
    while (x != &tree->nil) {
        y = x;
        if (0 == (cmp = tree->cmp_func(key, x->key))) {
            if (NULL != oldvalue) {
                *oldvalue = x->value;
            }
            if (!HAS_FLAG(flags, RBTREE_INSERT_ON_DUP_KEY_PRESERVE)) {
                if (NULL != tree->value_dtor) {
                    tree->value_dtor(x->value);
                }
                x->value = clone(tree->value_duper, value);
                return true;
            }
            return false;
        } else if (cmp < 0) {
            x = x->left;
        } else /*if (cmp > 0)*/ {
            x = x->right;
        }
    }
    new = rbtreenode_new((const void *) clone(tree->key_duper, key), clone(tree->value_duper, value));
    new->parent = y;
    new->left = &tree->nil;
    new->right = &tree->nil;
    //new->color = RED; // done in rbtreenode_new
    if (y == &tree->nil) {
        tree->root = new;
#ifdef MAINTAIN_FIRST_LAST
        tree->first = tree->last = new;
#endif /* MAINTAIN_FIRST_LAST */
    } else {
        cmp = tree->cmp_func(new->key, y->key);
        if (cmp < 0) {
            y->left = new;
#ifdef MAINTAIN_FIRST_LAST
            if (y == tree->first) {
                tree->first = new;
            }
#endif /* MAINTAIN_FIRST_LAST */
        } else /*if (cmp > 0)*/ {
            y->right = new;
#ifdef MAINTAIN_FIRST_LAST
            if (y == tree->last) {
                tree->last = new;
            }
#endif /* MAINTAIN_FIRST_LAST */
        }
    }

    while (RED == new->parent->color) {
        RBTreeNode *y;

        if (new->parent == new->parent->parent->left) {
            y = new->parent->parent->right;
            if (RED == y->color) {
                new->parent->color = BLACK;
                y->color = BLACK;
                new->parent->parent->color = RED;
                new = new->parent->parent;
            } else {
                if (new == new->parent->right) {
                    new = new->parent;
                    rbtree_rotate_left(tree, new);
                }
                new->parent->color = BLACK;
                new->parent->parent->color = RED;
                rbtree_rotate_right(tree, new->parent->parent);
            }
        } else /*if (new->parent == new->parent->parent->right)*/ {
            y = new->parent->parent->left;
            if (RED == y->color) {
                new->parent->color = BLACK;
                y->color = BLACK;
                new->parent->parent->color = RED;
                new = new->parent->parent;
            } else {
                if (new == new->parent->left) {
                    new = new->parent;
                    rbtree_rotate_right(tree, new);
                }
                new->parent->color = BLACK;
                new->parent->parent->color = RED;
                rbtree_rotate_left(tree, new->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;

    return true;
}

static RBTreeNode *rbtree_lookup(RBTree *tree, const void *key) /* NONNULL(1) */
{
    int cmp;
    RBTreeNode *y, *x;

    y = &tree->nil;
    x = tree->root;
    while (x != &tree->nil) {
        y = x;
        if (0 == (cmp = tree->cmp_func(key, x->key))) {
            return x;
        } else if (cmp < 0) {
            x = x->left;
        } else /*if (cmp > 0)*/ {
            x = x->right;
        }
    }

    return NULL;
}

/**
 * Fetch the current value of a key
 *
 * @param tree the RB tree
 * @param key the key of the element from which to retrieve its value
 * @param value if this pointer is not NULL, it will receive the previous value associated to the key
 *
 * @return false if the key is not present in the tree
 */
bool rbtree_get(RBTree *tree, const void *key, void **value) /* NONNULL(1, 3) */
{
    RBTreeNode *node;

    if (NULL != (node = rbtree_lookup(tree, key))) {
        *value = node->value;
        return true;
    }

    return false;
}

/**
 * Determine if a key already exists
 *
 * @param tree the RB tree
 * @param key the key to looking for
 *
 * @return true if the key is registered
 */
bool rbtree_exists(RBTree *tree, const void *key) /* NONNULL(1) */
{
    return NULL != rbtree_lookup(tree, key);
}

/**
 * Replace the value associated to a given key
 * The key have to be previously registered
 *
 * @param tree the RB tree
 * @param key the key from which to change the value
 * @param newvalue the new value associated to key
 * @param call_dtor set it to false to skip value destructor
 *
 * @return false if the key is not in the tree
 */
bool rbtree_replace(RBTree *tree, const void *key, void *newvalue, bool call_dtor)  /* NONNULL(1) */
{
    RBTreeNode *node;

    if (NULL != (node = rbtree_lookup(tree, key))) {
        if (call_dtor && NULL != tree->value_dtor) {
            tree->value_dtor(node->value);
        }
        node->value = clone(tree->value_duper, newvalue);
        return true;
    }

    return false;
}

/**
 * Get the key and value associated to the least key
 *
 * @param tree the RB tree
 * @param key if not NULL, key will point on the key of the least element in the tree
 * @param value if this pointer is not NULL, it is set to the current value associated to *key*
 *
 * @return false if the tree is empty
 */
bool rbtree_min(RBTree *tree, const void **key, void **value) /* NONNULL(1) */
{
    assert(NULL != tree);

#ifdef MAINTAIN_FIRST_LAST
    if (tree->first == &tree->nil) {
        return false;
    } else {
        if (NULL != key) {
            *key = tree->first->key;
        }
        if (NULL != value) {
            *value = tree->first->value;
        }

        return true;
    }
#else
    if (tree->root == &tree->nil) {
        return false;
    } else {
        RBTreeNode *min;

        min = rbtreenode_min(tree, tree->root);
        if (NULL != key) {
            *key = min->key;
        }
        if (NULL != value) {
            *value = min->value;
        }

        return true;
    }
#endif /* MAINTAIN_FIRST_LAST */
}

/**
 * Get the key and value associated to the greatest key
 *
 * @param tree the RB tree
 * @param key if not NULL, key will point on the key of the greatest element in the tree
 * @param value if this pointer is not NULL, it is set to the current value associated to *key*
 *
 * @return false if the tree is empty
 */
bool rbtree_max(RBTree *tree, const void **key, void **value) /* NONNULL(1) */
{
    assert(NULL != tree);

#ifdef MAINTAIN_FIRST_LAST
    if (tree->last == &tree->nil) {
        return false;
    } else {
        if (NULL != key) {
            *key = tree->last->key;
        }
        if (NULL != value) {
            *value = tree->last->value;
        }

        return true;
    }
#else
    if (tree->root == &tree->nil) {
        return false;
    } else {
        RBTreeNode *max;

        max = rbtreenode_max(tree, tree->root);
        if (NULL != key) {
            *key = max->key;
        }
        if (NULL != value) {
            *value = max->value;
        }

        return true;
    }
#endif /* MAINTAIN_FIRST_LAST */
}

static void rbtree_transplante(RBTree *tree, RBTreeNode *u, RBTreeNode *v) /* NONNULL() */
{
    if (u->parent == &tree->nil) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

/**
 * Remove an element of the tree from its key
 *
 * @param tree the RB tree
 * @param key the key of the element to remove
 * @param call_dtor set it to false to skip value destructor
 *
 * @return false if the key is absent from the tree
 */
bool rbtree_remove(RBTree *tree, const void *key, bool call_dtor) /* NONNULL(1) */
{
    RBTreeColor ycolor;
    RBTreeNode *y, *x, *w, *z;

    if (NULL == (z = rbtree_lookup(tree, key))) {
        return false;
    }
#ifdef MAINTAIN_FIRST_LAST
    if (z == tree->first) {
        tree->first = rbtreenode_next(tree, z);
    } else if (z == tree->last) {
        tree->last = rbtreenode_previous(tree, z);
    }
#endif /* MAINTAIN_FIRST_LAST */
    y = z;
    ycolor = y->color;
    if (z->left == &tree->nil) {
        x = z->right;
        rbtree_transplante(tree, z, z->right);
    } else if (z->right == &tree->nil) {
        x = z->left;
        rbtree_transplante(tree, z, z->left);
    } else {
        y = rbtreenode_min(tree, z->right);
        ycolor = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            rbtree_transplante(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rbtree_transplante(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (BLACK == ycolor) {
        while (x != tree->root && BLACK == x->color) {
            if (x == x->parent->left) {
                w = x->parent->right;
                if (RED == w->color) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rbtree_rotate_left(tree, x->parent);
                    w = x->parent->right;
                }
                if (BLACK == w->left->color && BLACK == w->right->color) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (BLACK == w->right->color) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rbtree_rotate_right(tree, w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    rbtree_rotate_left(tree, x->parent);
                    x = tree->root;
                }
            } else {
                w = x->parent->left;
                if (RED == w->color) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rbtree_rotate_right(tree, x->parent);
                    w = x->parent->left;
                }
                if (BLACK == w->right->color && BLACK == w->left->color) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (BLACK == w->left->color) {
                        w->right->color = BLACK;
                        w->color = RED;
                        rbtree_rotate_left(tree, w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rbtree_rotate_right(tree, x->parent);
                    x = tree->root;
                }
            }
        }
        x->color = BLACK;
    }
    if (NULL != tree->value_dtor) {
        tree->value_dtor(z->value);
    }
    if (NULL != tree->key_dtor) {
        tree->key_dtor((void *) z->key);
    }
    free(z);

    return true;
}

static void _rbtree_destroy(RBTree *tree, RBTreeNode *node) /* NONNULL(1) */
{
    assert(NULL != tree);

    if (node != &tree->nil) {
        _rbtree_destroy(tree, node->right);
        _rbtree_destroy(tree, node->left);
        if (NULL != tree->value_dtor) {
            tree->value_dtor(node->value);
        }
        if (NULL != tree->key_dtor) {
            tree->key_dtor((void *) node->key);
        }
        free(node);
    }
}

/**
 * Empty a tree to be reused
 *
 * @param tree the RB tree to clear
 */
void rbtree_clear(RBTree *tree) /* NONNULL() */
{
    assert(NULL != tree);

    _rbtree_destroy(tree, tree->root);
    tree->root = &tree->nil;
}

/**
 * Destroy (free memory used by) a tree
 *
 * @param tree the RB tree to destroy
 */
void rbtree_destroy(RBTree *tree) /* NONNULL() */
{
    assert(NULL != tree);

    _rbtree_destroy(tree, tree->root);
    free(tree);
}

static void _rbtree_traverse_in_order(RBTree *tree, RBTreeNode *node, TravFunc trav_func) /* NONNULL(2) */
{
    if (node != &tree->nil) {
        _rbtree_traverse_in_order(tree, node->left, trav_func);
        trav_func(node->key, node->value);
        _rbtree_traverse_in_order(tree, node->right, trav_func);
    }
}

static void _rbtree_traverse_pre_order(RBTree *tree, RBTreeNode *node, TravFunc trav_func) /* NONNULL(2) */
{
    if (node != &tree->nil) {
        trav_func(node->key, node->value);
        _rbtree_traverse_pre_order(tree, node->left, trav_func);
        _rbtree_traverse_pre_order(tree, node->right, trav_func);
    }
}

static void _rbtree_traverse_post_order(RBTree *tree, RBTreeNode *node, TravFunc trav_func) /* NONNULL(2) */
{
    if (node != &tree->nil) {
        _rbtree_traverse_post_order(tree, node->left, trav_func);
        _rbtree_traverse_post_order(tree, node->right, trav_func);
        trav_func(node->key, node->value);
    }
}

/**
 * Traverse a tree by calling a function for each element of the tree
 *
 * @param tree the RB tree to traverse
 * @param mode the way to traverse the tree. One of the constants: IN_ORDER (infixed), PRE_ORDER (prefixed) and POST_ORDER (postfixed)
 * @param trav_func the callback to call for each element
 */
void rbtree_traverse(RBTree *tree, TraverseMode mode, TravFunc trav_func) /* NONNULL() */
{
    assert(NULL != tree);
    assert(NULL != trav_func);

    switch (mode) {
        case IN_ORDER:
            _rbtree_traverse_in_order(tree, tree->root, trav_func);
            break;
        case PRE_ORDER:
            _rbtree_traverse_pre_order(tree, tree->root, trav_func);
            break;
        case POST_ORDER:
            _rbtree_traverse_post_order(tree, tree->root, trav_func);
            break;
    }
}

#if 0
#if defined(MAINTAIN_FIRST_LAST) && !defined(WITHOUT_ITERATOR)
static void rbtree_iterator_first(const void *collection, void **state)
{
    //
}

static void rbtree_iterator_last(const void *collection, void **state)
{
    //
}

static bool rbtree_iterator_is_valid(const void *collection, void **state)
{
    return false;
}

static void rbtree_iterator_current(const void *collection, void **state, void **key, void **value)
{
    //
}

static void rbtree_iterator_next(const void *collection, void **state)
{
    //
}

static void rbtree_iterator_previous(const void *collection, void **state)
{
    //
}

void rbtree_to_iterator(Iterator *it, RBTree *tree)
{
    iterator_init(
        it, tree, NULL,
        rbtree_iterator_first, rbtree_iterator_last,
        rbtree_iterator_current,
        rbtree_iterator_next, rbtree_iterator_previous,
        rbtree_iterator_is_valid,
        NULL
    );
}
#endif /* MAINTAIN_FIRST_LAST && !WITHOUT_ITERATOR */
#endif
