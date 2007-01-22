/* $Id$ */
#ifndef ARRAY_INCLUDED
#define ARRAY_INCLUDED
#define T Array_T
typedef struct T *T;
extern T    Array_new (int length, int size);
extern void Array_free(T *array);
extern int Array_length(T array);
extern int Array_size  (T array);
extern void *Array_get(T array, int i);
extern void *Array_put(T array, int i, void *elem);
extern void Array_resize(T array, int length);
extern T    Array_copy  (T array, int length);
#undef T
#endif
