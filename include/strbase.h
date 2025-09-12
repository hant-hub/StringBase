#ifndef STRBASE_H
#define STRBASE_H

#include <cutils.h>

/*
    INFO: This requires Cutils to be implemented
    and linked. StrBase.c will assume that somewhere there
    is a Cutils implementation.
*/

typedef struct StrBase {
    Allocator mem; // Assume Dynamic Memory

    // hashmap for deduplication
    u32 *stridx;
    u32 *meta; // meta data for robin hood hashing
    u32 *refs; // reference count

    u32 hashsize;
    u32 hashcap;

    // Stable Storage (index stability)
    SString *strstore;
    u32 *freeslots; // free list

    u32 freesize;
    u32 maxslots;
} StrBase;

#endif
