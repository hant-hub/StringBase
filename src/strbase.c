#include <strbase.h>
#include <string.h>

static void HashResize(StrBase *b) {
    if (b->hashsize < b->hashcap * STRBASE_LOAD_MAX)
        return;

    u32 oldsize = b->hashcap;
    while (b->hashsize >= b->hashcap * STRBASE_LOAD_MAX) {
        b->hashcap = b->hashcap ? b->hashcap * 2 : 16;
    }

    u32* oldkeys = b->stridx; 
    u32* oldmeta = b->meta;

    b->stridx = Alloc(b->mem, b->hashcap * sizeof(u32));
    b->meta = Alloc(b->mem, b->hashcap * sizeof(u32));

    memset(b->meta, -1, b->hashcap * sizeof(u32));

    //TODO(ELI): rehash
    for (u32 i = 0; i < oldsize; i++) {
        if (oldmeta[i] == -1) continue;
    }


    Free(b->mem, oldkeys, oldsize * sizeof(u32));
    Free(b->mem, oldmeta, oldsize * sizeof(u32));
}

//TODO(ELI): Dynamic Array for String storage

//TODO(ELI): Deletion

// Will copy string into internally managed table
// free string memory afterward
StrID StrBaseAdd(StrBase *base, SString s) { 
    HashResize(base);
    return STRBASE_INAVLID_STR; 
}

//Returns Zero on miss (this should never happen)
SString StrBaseGet(StrBase *base, StrID s) { return (SString){0}; }

//Decrement reference counter (free when zero)
void StrBaseDel(StrBase* base, StrID s) {}
