/**
 * @file dynamic_arrays/dptrarray.c
 * @brief dynamic array specifically designed to store pointers
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "attributes.h"
#include "utils.h"
#include "dptrarray.h"
#include "nearest_power.h"

#define D_PTR_ARRAY_INCREMENT 8

#define mem_new(type)           malloc((sizeof(type)))
#define mem_new_n(type, n)      malloc((sizeof(type) * (n)))
#define mem_renew(ptr, type, n) realloc((ptr), (sizeof(type) * (n)))

static void dptrarray_maybe_resize_to(DPtrArray *this, size_t total_length) /* NONNULL() */
{
    assert(NULL != this);

    if (total_length >= this->allocated) {
        size_t i;

        i = this->allocated;
        this->allocated = ((total_length / D_PTR_ARRAY_INCREMENT) + 1) * D_PTR_ARRAY_INCREMENT;
        this->data = mem_renew(this->data, *this->data, this->allocated);

        while (i < this->allocated) {
            this->data[i++] = /*this->duper*/(this->default_value);
        }
    }
}

static void dptrarray_maybe_resize_of(DPtrArray *this, size_t additional_length) /* NONNULL() */
{
    assert(NULL != this);

    dptrarray_maybe_resize_to(this, this->length + additional_length);
}

/**
 * XXX
 *
 * @param length
 * @param duper
 * @param dtor_func
 * @param default_value
 *
 * @return
 */
DPtrArray *dptrarray_sized_new(size_t length, DupFunc duper, DtorFunc dtor_func, void *default_value) /* WARN_UNUSED_RESULT */
{
    DPtrArray *this;

    this = mem_new(*this);
    this->data = NULL;
    this->length = this->allocated = 0;
    this->default_value = default_value;
    dptrarray_maybe_resize_to(this, length);
    this->duper = duper;
    this->dtor_func = dtor_func;

    return this;
}

/**
 * XXX
 *
 * @param duper
 * @param dtor_func
 * @param default_value
 *
 * @return
 */
DPtrArray *dptrarray_new(DupFunc duper, DtorFunc dtor_func, void *default_value) /* WARN_UNUSED_RESULT */
{
    return dptrarray_sized_new(D_PTR_ARRAY_INCREMENT, duper, dtor_func, default_value);
}

/**
 * XXX
 *
 * @param this
 */
void dptrarray_destroy(DPtrArray *this) /* NONNULL() */
{
    assert(NULL != this);

    if (NULL != this->dtor_func) {
        size_t i;

        for (i = 0; i < this->length; i++) {
            this->dtor_func(this->data[i]);
        }
#if 0
        if (NULL != this->default_value) {
            this->dtor_func(this->default_value);
        }
#endif
    }
    free(this->data);
    free(this);
}

/**
 * XXX
 *
 * @param this
 */
void dptrarray_clear(DPtrArray *this) /* NONNULL() */
{
    assert(NULL != this);

    if (NULL != this->dtor_func) {
        size_t i;

        for (i = 0; i < this->length; i++) {
            this->dtor_func(this->data[i]);
        }
    }
    this->length = 0;
}

/**
 * XXX
 *
 * @param this
 *
 * @return
 */
void *dptrarray_pop(DPtrArray *this) /* NONNULL() */
{
    assert(this->length > 0);

    return this->data[--this->length];
}

/**
 * XXX
 *
 * @param this
 *
 * @return
 */
void *dptrarray_shift(DPtrArray *this) /* NONNULL() */
{
    void *data;

    assert(this->length > 0);

    data = this->data[0];
    memmove(this->data, this->data + 1, (this->length - 1) * sizeof(*this->data));
    --this->length;

    return data;
}

/**
 * XXX
 *
 * @param this
 * @param data
 *
 * @return
 */
void *dptrarray_push(DPtrArray *this, void *data) /* NONNULL(1) */
{
    assert(NULL != this);

    dptrarray_maybe_resize_of(this, 1);
    return this->data[this->length++] = NULL == this->duper ? data : this->duper(data);
}

/**
 * XXX
 *
 * @param this
 * @param data
 */
void dptrarray_unshift(DPtrArray *this, void *data) /* NONNULL(1) */
{
    assert(NULL != this);

    dptrarray_maybe_resize_of(this, 1);
    memmove(this->data + 1, this->data, this->length * sizeof(*this->data));
    this->data[0] = NULL == this->duper ? data : this->duper(data);
    ++this->length;
}

/**
 * XXX
 *
 * @param this
 * @param offset
 * @param data
 */
void dptrarray_insert(DPtrArray *this, size_t offset, void *data) /* NONNULL(1) */
{
    assert(NULL != this);
    assert(offset <= this->length);

    dptrarray_maybe_resize_of(this, 1);
    if (offset != this->length) {
        memmove(this->data + offset + 1, this->data + offset, (this->length - offset) * sizeof(*this->data));
    }
    this->data[offset] = NULL == this->duper ? data : this->duper(data);
    ++this->length;
}

/**
 * XXX
 *
 * @param this
 * @param offset
 * @param call_dtor
 *
 * @return
 */
void *dptrarray_remove_at(DPtrArray *this, size_t offset, bool call_dtor) /* NONNULL() */
{
    void *data;

    assert(NULL != this);
    assert(offset < this->length);

    if (call_dtor && NULL != this->dtor_func) {
        data = NULL;
        this->dtor_func(this->data[offset]);
    } else {
        data = this->data[offset];
    }
    memmove(this->data + offset, this->data + offset + 1, (this->length - offset - 1) * sizeof(*this->data));
    --this->length;

    return data;
}

/**
 * XXX
 *
 * @param this
 * @param from
 * @param to
 */
void dptrarray_remove_range(DPtrArray *this, size_t from, size_t to) /* NONNULL() */
{
    size_t diff;

    assert(NULL != this);
    assert(from < this->length);
    assert(to < this->length);
    assert(from <= to);

    diff = to - from + 1;
    if (NULL != this->dtor_func) {
        size_t i;

        for (i = from; i <= to; i++) {
            this->dtor_func(this->data[i]);
        }
    }
    memmove(this->data + from, this->data + to + 1, (this->length - diff) * sizeof(*this->data));
    this->length -= diff;
}

/**
 * XXX
 *
 * @param this
 * @param offset
 *
 * @return
 */
void *dptrarray_at(DPtrArray *this, size_t offset) /* NONNULL() */
{
    assert(NULL != this);

    if (offset < this->length) {
        return this->data[offset];
    } else {
        return NULL;
    }
}

/**
 * XXX
 *
 * @param this
 * @param offset1
 * @param offset2
 */
void dptrarray_swap(DPtrArray *this, size_t offset1, size_t offset2) /* NONNULL() */
{
    void *tmp;

    assert(NULL != this);
    assert(offset1 < this->length);
    assert(offset2 < this->length);

    tmp = this->data[offset1];
    this->data[offset1] = this->data[offset2];
    this->data[offset2] = tmp;
}

/**
 * XXX
 *
 * @param this
 * @param length
 */
void dptrarray_set_size(DPtrArray *this, size_t length) /* NONNULL() */
{
    assert(NULL != this);

    if (length < this->length) {
        if (NULL != this->dtor_func) {
            size_t i;

            for (i = length - 1; i < this->length; i++) {
                this->dtor_func(this->data[i]);
            }
        }
        this->length = length;
    } else {
        dptrarray_maybe_resize_to(this, length);
    }
}

/**
 * XXX
 *
 * @param this
 *
 * @return
 */
size_t dptrarray_length(DPtrArray *this) /* NONNULL() */
{
    assert(NULL != this);

    return this->length;
}

/**
 * XXX
 *
 * @param this
 * @param cmpfn
 * @param arg
 */
void dptrarray_sort(DPtrArray *this, CmpFuncArg cmpfn, void *arg)
{
    assert(NULL != this);
    assert(NULL != cmpfn);

    QSORT_R(this->data, this->length, sizeof(*this->data), cmpfn, arg);
}

/**
 * XXX
 *
 * @param this
 * @param copy
 * @param null_terminated
 *
 * @return
 */
void *dptrarray_to_array(DPtrArray *this, bool copy, bool null_terminated) /* NONNULL() */
{
    void **ary;

    assert(NULL != this);

    ary = mem_new_n(*this->data, this->length + !!null_terminated);
    if (copy) {
        size_t i;

        for (i = 0; i < this->length; i++) {
            ary[i] = NULL == this->duper ? this->data[i] : this->duper(this->data[i]);
        }
    } else {
        memcpy(ary, this->data, this->length * sizeof(*this->data));
    }
    if (null_terminated) {
        ary[this->length] = NULL;
    }

    return ary;
}

#ifndef WITHOUT_ITERATOR
static void dptrarray_iterator_first(const void *collection, void **state)
{
    assert(NULL != collection);
    assert(NULL != state);

    *(void ***) state = ((DPtrArray *) collection)->data;
}

static void dptrarray_iterator_last(const void *collection, void **state)
{
    DPtrArray *ary;

    assert(NULL != collection);
    assert(NULL != state);

    ary = (DPtrArray *) collection;
    *(void ***) state = ary->data + ary->length - 1;
}

static bool dptrarray_iterator_is_valid(const void *collection, void **state)
{
    DPtrArray *ary;

    assert(NULL != collection);
    assert(NULL != state);

    ary = (DPtrArray *) collection;

    return *((void ***) state) >= ary->data && *((void ***) state) < (ary->data + ary->length);
}

static void dptrarray_iterator_current(const void *collection, void **state, void **value, void **key)
{
    DPtrArray *ary;

    assert(NULL != collection);
    assert(NULL != state);
    assert(NULL != value);

    ary = (DPtrArray *) collection;
    if (NULL != key) {
        *((int *) key) = *((void ***) state) - ary->data;
    }
    *value = **(void ***) state;
}

static void dptrarray_iterator_next(const void *UNUSED(collection), void **state)
{
    assert(NULL != state);

    ++*((void ***) state);
}

static void dptrarray_iterator_previous(const void *UNUSED(collection), void **state)
{
    assert(NULL != state);

    --*((void ***) state);
}

/**
 * XXX
 *
 * @param it
 * @param this
 */
void dptrarray_to_iterator(Iterator *it, DPtrArray *this)
{
    iterator_init(
        it, this, NULL,
        dptrarray_iterator_first, dptrarray_iterator_last,
        dptrarray_iterator_current,
        dptrarray_iterator_next, dptrarray_iterator_previous,
        dptrarray_iterator_is_valid,
        NULL
    );
}
#endif /* !WITHOUT_ITERATOR */
