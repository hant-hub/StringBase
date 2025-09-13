#include "cutils.h"
#include <strbase.h>
#include <string.h>

// hashmap
static void HashResize(StrBase *b) {
    if (b->hashsize < b->hashcap * STRBASE_LOAD_MAX)
        return;

    u32 oldsize = b->hashcap;
    while (b->hashsize >= b->hashcap * STRBASE_LOAD_MAX) {
        b->hashcap = b->hashcap ? b->hashcap * 2 : 16;
    }

    u32 *oldkeys = b->stridx;
    u32 *oldmeta = b->meta;

    b->stridx = Alloc(b->mem, b->hashcap * sizeof(u32));
    b->meta = Alloc(b->mem, b->hashcap * sizeof(u32));

    memset(b->meta, -1, b->hashcap * sizeof(u32));

    // TODO(ELI): rehash
    for (u32 i = 0; i < oldsize; i++) {
        if (oldmeta[i] == -1)
            continue;

        //hash insert
    }

    Free(b->mem, oldkeys, oldsize * sizeof(u32));
    Free(b->mem, oldmeta, oldsize * sizeof(u32));
}

// dyn array

static u32 AllocSlot(StrBase *base) {
    if (base->freesize == 0) {
        u32 oldsize = base->maxslots;
        base->maxslots = base->maxslots ? base->maxslots * 2 : STRBASE_MIN_SIZE;

        base->strstore =
            Realloc(base->mem, base->strstore, oldsize * sizeof(SString),
                    base->maxslots * sizeof(SString));
        memset(base->strstore, 0, base->maxslots * sizeof(SString));

        base->refs = Realloc(base->mem, base->refs, oldsize * sizeof(u32),
                             base->maxslots * sizeof(u32));

        base->freeslots =
            Realloc(base->mem, base->freeslots, oldsize * sizeof(u32),
                    base->maxslots * sizeof(u32));

        for (u32 i = oldsize; i < base->maxslots; i++) {
            base->freeslots[base->freesize++] = i;
        }
    }

    return base->freeslots[--base->freesize];
}

// TODO(ELI): Deletion

// Will copy string into internally managed table
// free string memory afterward
StrID StrBaseAdd(StrBase *base, SString s) {
    HashResize(base);

    u32 idx = FNVHash32((u8 *)s.data, s.len) % base->hashcap;
    u32 counter = 0;

    u32 out = STRBASE_INAVLID_STR;

    for (u32 i = 0; i < base->hashcap; i++) {
        if (base->meta[idx] == STRBASE_INAVLID_STR) {
            // empty
            u32 slot = AllocSlot(base);

            base->meta[idx] = counter;
            base->stridx[idx] = slot;

            base->strstore[slot] = Sstrdup(base->mem, s);
            base->refs[slot] = 1;

            return slot;
        }

        if (base->meta[idx] < counter) {
            // steal
            break;
        }

        if (Sstrcmp(s, base->strstore[base->stridx[idx]])) {
            // duplicate
            base->refs[base->stridx[idx]]++;
            return base->stridx[idx];
        }

        idx = (idx + 1) % base->hashcap;
        counter++;
    }

    // robin hood
    u32 key = base->stridx[idx];
    {
        u32 tmpcounter = base->meta[idx];

        u32 slot = AllocSlot(base);

        base->meta[idx] = counter;
        base->stridx[idx] = slot;

        base->strstore[slot] = Sstrdup(base->mem, s);
        base->refs[slot] = 1;

        counter = tmpcounter;
        out = slot;
    }

    for (u32 i = 0; i < base->hashcap; i++) {
        if (base->meta[idx] == STRBASE_INAVLID_STR) {
            // empty
            base->meta[idx] = counter;
            base->stridx[idx] = key;

            return out;
        }

        if (base->meta[idx] < counter) {
            // steal
            u32 tmpcounter = base->meta[idx];
            u32 tmpslot = base->stridx[idx];

            base->meta[idx] = counter;
            base->stridx[idx] = key;

            counter = tmpcounter;
            key = tmpslot;
        }

        idx = (idx + 1) % base->hashcap;
        counter++;
    }

    return STRBASE_INAVLID_STR;
}

// Returns Zero on miss (this should never happen)
SString StrBaseGet(StrBase *base, StrID s) { return (SString){0}; }

// Decrement reference counter (free when zero)
void StrBaseDel(StrBase *base, StrID s) {}

void StrBaseFree(StrBase *base) {
    for (u32 i = 0; i < base->maxslots; i++) {
        Free(base->mem, base->strstore[i].data, base->strstore[i].len);
    }

    Free(base->mem, base->strstore, base->maxslots * sizeof(SString));
    Free(base->mem, base->refs, base->maxslots * sizeof(u32));
    Free(base->mem, base->freeslots, base->maxslots * sizeof(u32));

    Free(base->mem, base->stridx, base->hashcap * sizeof(u32));
    Free(base->mem, base->meta, base->hashcap * sizeof(u32));
}
