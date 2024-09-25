/**
 * @file lists/dlist.c
 * @brief double linked list
 */

#include <stdlib.h>
#include <assert.h>

#include "attributes.h"
#include "dlist.h"
#include "error.h"

static inline DListElement *alloc_element(DList *list, void *data, char **error)
{
    DListElement *el;

    if (NULL == (el = malloc(sizeof(*el)))) {
        set_malloc_error(error, sizeof(*el));
    } else {
        if (NULL == list->dup) {
            el->data = data;
        } else {
            el->data = list->dup(data);
        }
    }

    return el;
}

/**
 * Creates (heap allocated) a double linked list
 *
 * @param dup
 * @param dtor
 * @param error
 *
 * @return the dynamic allocated double linked list
 */
DList *dlist_new(DupFunc dup, DtorFunc dtor, char **error)
{
    DList *list;

    if (NULL == (list = malloc(sizeof(*list)))) {
        set_malloc_error(error, sizeof(*list));
    } else {
        dlist_init(list, dup, dtor);
    }

    return list;
}

/**
 * Initializes a (stack allocated) double linked list
 *
 * @param list the double linked list
 * @param dup
 * @param dtor
 */
void dlist_init(DList *list, DupFunc dup, DtorFunc dtor)
{
    assert(NULL != list);

    list->length = 0;
    list->head = list->tail = NULL;
    list->dup = dup;
    list->dtor = dtor;
}

/**
 * Get the length of the double linked list *list*
 * This information is maintained into the list, its
 * elements are not traversed at each call.
 *
 * @param list the list
 *
 * @return the number of elements in the list
 */
size_t dlist_length(DList *list)
{
    assert(NULL != list);

    return list->length;
}

/**
 * Destroy the elements of a double linked list but not
 * the list itself. Used to clear/empty it before reuse.
 *
 * If a destructor was set at the creation of the list,
 * it will be invoked for each data.
 *
 * @param list the list to clear
 */
void dlist_clear(DList *list)
{
    DListElement *tmp, *last;

    assert(NULL != list);

    tmp = list->head;
    while (NULL != tmp) {
        last = tmp;
        tmp = tmp->next;
        if (NULL != list->dtor) {
            list->dtor(last->data);
        }
        free(last);
    }
    list->length = 0;
    list->head = list->tail = NULL;
}

/**
 * Clear and free a dynamic allocated double linked list
 * (must only be used for a list created by `dlist_new`
 * else use `dlist_clear` instead).
 *
 * If a destructor was set at the creation of the list,
 * it will be invoked for each data.
 *
 * @param list the list to clear
 */
void dlist_destroy(DList *list)
{
    assert(NULL != list);

    dlist_clear(list);
    free(list);
}

/**
 * Append *data* to *list*
 *
 * @param list the double linked list
 * @param data
 * @param error
 *
 * @return `true` on success, `false` on error (memory allocation failure)
 */
bool dlist_append(DList *list, void *data, char **error)
{
    bool ok;

    assert(NULL != list);
    ok = false;
    do {
        DListElement *tmp;

        if (NULL == (tmp = alloc_element(list, data, error))) {
            break;
        }
        tmp->next = NULL;
        if (NULL != list->tail) {
            list->tail->next = tmp;
            tmp->prev = list->tail;
        } else {
            tmp->prev = NULL;
            list->head = tmp;
        }
        list->tail = tmp;
        ++list->length;
        ok = true;
    } while (false);

    return ok;
}

/**
 * Find the first element in a list to match according a callback
 *
 * @param list the double linked list
 * @param cmp a callback to compare datas (data from the current item to *data*)
 *            returns 0 if both are equals, a negative value if *data* is greater,
 *            posivite if *data* is smaller.
 * @param data the data to be compared to
 *
 * @return `NULL` if there is no match else the matching list element
 */
DListElement *dlist_find_first(DList *list, CmpFunc cmp, void *data)
{
    DListElement *el;

    assert(NULL != list);
    assert(NULL != cmp);

    for (el = list->head; NULL != el; el = el->next) {
        if (0 == cmp(el->data, data)) {
            return el;
        }
    }

    return NULL;
}

/**
 * Find the last element in a list to match according a callback
 *
 * @param list the double linked list
 * @param cmp a callback to compare datas (data from the current item to *data*)
 *            returns 0 if both are equals, a negative value if *data* is greater,
 *            posivite if *data* is smaller.
 * @param data the data to be compared to
 *
 * @return `NULL` if there is no match else the matching list element
 */
DListElement *dlist_find_last(DList *list, CmpFunc cmp, void *data)
{
    DListElement *el;

    assert(NULL != list);
    assert(NULL != cmp);

    for (el = list->tail; NULL != el; el = el->prev) {
        if (0 == cmp(el->data, data)) {
            return el;
        }
    }

    return NULL;
}

/**
 * Inserts data before a specific element in the list
 *
 * @param list the double linked list
 * @param sibling
 * @param data the data to be added
 * @param error
 *
 * @return `true` on success, `false` on error (memory allocation failure)
 */
bool dlist_insert_before(DList *list, DListElement *sibling, void *data, char **error)
{
    bool ok;

    assert(NULL != list);
    assert(NULL != sibling);

    if (sibling == list->head) {
        ok = dlist_prepend(list, data, error);
    } else {
        ok = false;
        do {
            DListElement *tmp;

            if (NULL == (tmp = alloc_element(list, data, error))) {
                break;
            }
            tmp->next = sibling;
            tmp->prev = sibling->prev;
            sibling->prev->next = tmp;
            sibling->prev = tmp;
            ok = true;
        } while (false);
    }

    return ok;
}

/**
 * Inserts data after a specific element in the list
 *
 * @param list the double linked list
 * @param sibling
 * @param data the data to be added
 * @param error
 *
 * @return `true` on success, `false` on error (memory allocation failure)
 */
bool dlist_insert_after(DList *list, DListElement *sibling, void *data, char **error)
{
    bool ok;

    assert(NULL != list);
    assert(NULL != sibling);

    if (sibling == list->tail) {
        ok = dlist_append(list, data, error);
    } else {
        ok = false;
        do {
            DListElement *tmp;

            if (NULL == (tmp = alloc_element(list, data, error))) {
                break;
            }
            sibling->next->prev = tmp;
            tmp->next = sibling->next;
            tmp->prev = sibling;
            sibling->next = tmp;
            ok = true;
        } while (false);
    }

    return ok;
}

/**
 * Gets list element at a given position
 *
 * @param list the double linked list
 * @param n the position of the item in the list.
 *          If negative, it is counted from the tail
 *          of the list.
 *
 * @return `NULL` if *n* is out of bounds else the
 *         list element at that location
 */
DListElement *dlist_link_at(DList *list, int n)
{
    size_t offset;
    DListElement *el;

    assert(NULL != list);

    if (n < 0) {
        n = -n;
        el = list->tail;
        offset = offsetof(DListElement, prev);
    } else {
        el = list->head;
        offset = offsetof(DListElement, next);
    }
    if (n <= (int) (list->length - (el == list->head))) {
        for (; NULL != el && n >= 0; n--) {
            el = (DListElement *) (((char *) el) + offset);
        }
    }

    return el;
}

/**
 * Inserts data at (before) a given position.
 *
 * @param list the double linked list
 * @param n the position of the item in the list.
 *          If negative, it is counted from the tail
 *          of the list.
 * @param data the data to be inserted
 * @param error
 *
 * @return `true` on success, `false` on error (memory allocation
 *         failure or if *n* is out of bounds)
 */
bool dlist_insert_at(DList *list, int n, void *data, char **error)
{
    DListElement *el;

    assert(NULL != list);

    if (NULL == (el = dlist_link_at(list, n))) {
        return false;
    } else {
        return dlist_insert_before(list, el, data, error);
    }
}

/**
 * Removes element at a given position.
 *
 * @param list the double linked list
 * @param n the position of the item in the list.
 *          If negative, it is counted from the tail
 *          of the list.
 *
 * @return `true` on success, `false` on error (*n* is out of bounds)
 */
bool dlist_remove_at(DList *list, int n)
{
    DListElement *el;

    assert(NULL != list);

    if (NULL == (el = dlist_link_at(list, n))) {
        return false;
    } else {
        dlist_remove_link(list, el);
        return true;
    }
}

/**
 * Is the double linked list empty?
 *
 * @param list the double linked list
 *
 * @return `true` if *list* is empty else `false`
 */
bool dlist_empty(DList *list)
{
    assert(NULL != list);

    return NULL == list->head;
}

/**
 * Prepend *data* to *list*
 *
 * @param list the double linked list
 * @param data
 * @param error
 *
 * @return `true` on success, `false` on error (memory allocation failure)
 */
bool dlist_prepend(DList *list, void *data, char **error)
{
    bool ok;

    assert(NULL != list);

    ok = false;
    do {
        DListElement *tmp;

        if (NULL == (tmp = alloc_element(list, data, error))) {
            break;
        }
        tmp->prev = NULL;
        if (NULL != list->head) {
            list->head->prev = tmp;
            tmp->next = list->head;
        } else {
            tmp->next = NULL;
            list->tail = tmp;
        }
        list->head = tmp;
        ++list->length;
        ok = true;
    } while (false);

    return ok;
}

/**
 * Removes data at the head of a list
 *
 * @param list the double linked list
 *
 * @note if a destructor has been set it will be called
 *       to free/clean the removed item(s)
 */
void dlist_remove_head(DList *list)
{
    DListElement *tmp;

    assert(NULL != list);

    if (NULL != list->head) {
        tmp = list->head;
        list->head = list->head->next;
        if (NULL != list->head) {
            list->head->prev = NULL;
        } else {
            list->head = list->tail = NULL;
        }
        if (NULL != list->dtor) {
            list->dtor(tmp->data);
        }
        free(tmp);
        --list->length;
    }
}

/**
 * Removes an element from the list
 *
 * @param list the double linked list
 * @param element the element to remove
 *
 * @note if a destructor has been set it will be called
 *       to free/clean the removed item(s)
 */
void dlist_remove_link(DList *list, DListElement *element)
{
    assert(NULL != list);
    assert(NULL != element);

    if (NULL != element->prev) {
        element->prev->next = element->next;
    }
    if (NULL != element->next) {
        element->next->prev = element->prev;
    }
    if (element == list->head) {
        list->head = list->head->next;
        if (NULL != list->head) {
            list->head->prev = NULL;
        }
    }
    if (element == list->tail) {
        list->tail = list->tail->prev;
        if (NULL != list->tail) {
            list->tail->next = NULL;
        }
    }
    if (NULL != list->dtor) {
        list->dtor(element->data);
    }
    free(element);
    --list->length;
}

/**
 * Removes data at the tail of a list
 *
 * @param list the double linked list
 *
 * @note if a destructor has been set it will be called
 *       to free/clean the removed item(s)
 */
void dlist_remove_tail(DList *list)
{
    DListElement *tmp;

    assert(NULL != list);

    if (list->tail) {
        tmp = list->tail;
        list->tail = list->tail->prev;
        if (NULL != list->tail) {
            list->tail->next = NULL;
        } else {
            list->head = list->tail = NULL;
        }
        if (NULL != list->dtor) {
            list->dtor(tmp->data);
        }
        free(tmp);
        --list->length;
    }
}

static DListElement *resolve_position(DList *list, int n)
{
    int c;
    DListElement *cur;

    assert(NULL != list);

    c = n < 0 ? -n : n;
    cur = n < 0 ? list->tail : list->head;
    while (NULL != cur && 0 != c) {
        cur = n < 0 ? cur->prev : cur->next;
        --c;
    }

    return (0 == c && NULL != cur) ? cur : NULL;
}

/**
 * Gets data at a given position into the list
 *
 * @param list the double linked list
 * @param n
 * @param data
 *
 * @return `false` if *n* is out of bounds else `true`
 */
bool dlist_at(DList *list, int n, void **data)
{
    DListElement *cur;

    assert(NULL != list);
    assert(NULL != data);

    if (NULL != (cur = resolve_position(list, n))) {
        *data = cur->data;
    }

    return NULL != cur;
}

/**
 * Sorts the items of a list
 *
 * @param list the double linked list
 * @param cmp
 */
void dlist_sort(DList *list, CmpFunc cmp)
{
    assert(NULL != list);
    assert(NULL != cmp);

    if (NULL != list->head) {
        bool swapped;
        DListElement *cur, *tmp;

        tmp = NULL;
        do {
            swapped = false;
            cur = list->head;
            while (cur->next != tmp) {
                if (cmp(cur->data, cur->next->data) > 0) {
                    void *data;

                    data = cur->data;
                    cur->data = cur->next->data;
                    cur->next->data = data;
                    swapped = true;
                }
                cur = cur->next;
            }
            tmp = cur;
        } while (swapped);
    }
}

/* <stack - LIFO - operations> */

/**
 * Removes and returns data at top/head of a stack/list
 *
 * @param list the double linked list
 * @param data
 *
 * @return `false` if *list* is empty else `true`
 *
 * @note returns a boolean to be able to distinguish a `NULL`
 *       data to an empty list
 *
 * @note if a destructor has been set it will not be called against *\*data*
 *       (caller is responsible to free/clean)
 *
 * @see dlist_remove_head
 */
bool dlist_pop(DList *list, void **data)
{
    bool had_any;
    DListElement *tmp;

    assert(NULL != list);
    assert(NULL != data);

    if ((had_any = (NULL != list->head))) {
        tmp = list->head;
        list->head = list->head->next;
        if (NULL != list->head) {
            list->head->prev = NULL;
        } else {
            list->head = list->tail = NULL;
        }
        *data = tmp->data;
/*
        // don't call the destructor (but keep the code as a "reminder")
        if (NULL != list->dtor) {
            list->dtor(tmp->data);
        }
*/
        free(tmp);
        --list->length;
    }

    return had_any;
}

/**
 * @see dlist_prepend
 */
bool dlist_push(DList *list, void *data, char **error)
{
    return dlist_prepend(list, data, error);
}

/**
 * Gets data at top/head of a stack/list
 *
 * @param list the double linked list
 * @param data
 *
 * @return `false` if *list* is empty else `true`
 *
 * @note returns a boolean to be able to distinguish a `NULL`
 *       data to an empty list
 */
bool dlist_top(DList *list, void **data)
{
    bool any;

    assert(NULL != list);
    assert(NULL != data);

    if ((any = (NULL != list->head))) {
        *data = list->head->data;
    }

    return any;
}
/* </stack - LIFO - operations> */

#ifndef WITHOUT_ITERATOR
static void dlist_iterator_first(const void *collection, void **state)
{
    assert(NULL != collection);
    assert(NULL != state);

    *state = ((const DList *) collection)->head;
}

static void dlist_iterator_last(const void *collection, void **state)
{
    assert(NULL != collection);
    assert(NULL != state);

    *state = ((const DList *) collection)->tail;
}

static bool dlist_iterator_is_valid(const void *UNUSED(collection), void **state)
{
    assert(NULL != state);

    return NULL != *state;
}

static void dlist_iterator_current(const void *UNUSED(collection), void **state, void **UNUSED(key), void **value)
{
    assert(NULL != state);

    if (NULL != value) {
        *value = ((DListElement *) *state)->data;
    }
}

static void dlist_iterator_next(const void *UNUSED(collection), void **state)
{
    DListElement *el;

    assert(NULL != state);

    el = (DListElement *) *state;
    *state = el->next;
}

static void dlist_iterator_previous(const void *UNUSED(collection), void **state)
{
    DListElement *el;

    assert(NULL != state);

    el = (DListElement *) *state;
    *state = el->prev;
}

static void dlist_iterator_delete(const void *collection, void **state)
{
    DList *list;
    DListElement *el;

    assert(NULL != collection);
    assert(NULL != state);

    list = (DList *) collection;
    el = (DListElement *) *state;
    // TODO: handle backward traversing?
    *state = NULL == el->prev ? list->head : el->prev;
    dlist_remove_link(list, el);
}

/**
 * Initialize an *Iterator* to loop, in both directions, on the elements
 * of a double linked list.
 *
 * @param it the iterator to initialize
 * @param da the double linked list to traverse
 *
 * @note iterator directions: forward and backward
 * @note there is no key
 **/
void dlist_to_iterator(Iterator *it, DList *list)
{
    assert(NULL != it);
    assert(NULL != list);

    iterator_init(
        it, list, NULL,
        dlist_iterator_first, dlist_iterator_last,
        dlist_iterator_current,
        dlist_iterator_next, dlist_iterator_previous,
        dlist_iterator_is_valid,
        NULL,
        (iterator_count_t) dlist_length, NULL, dlist_iterator_delete
    );
}

static void dlist_collectable_into(void *collection, void *UNUSED(key), void *value)
{
    assert(NULL != collection);

    dlist_append(((DList *) collection), value, NULL);
}

/**
 * TODO
 **/
void dlist_to_collectable(Collectable *collectable, DList *list)
{
    assert(NULL != collectable);
    assert(NULL != list);

    collectable_init(collectable, list, dlist_collectable_into);
}
#endif /* !WITHOUT_ITERATOR */
