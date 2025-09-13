#define CU_IMPL
#include <cutils.h>

#include <strbase.h>

int main() {
    StrBase *data = &(StrBase){GlobalAllocator};

    StrID s = StrBaseAdd(data, sstring("test"));
    StrID s1 = StrBaseAdd(data, sstring("test"));
    StrID s2 = StrBaseAdd(data, sstring("test2"));
    StrID s3 = StrBaseAdd(data, sstring("test3"));
    StrID s4 = StrBaseAdd(data, sstring("test4"));
    StrID s5 = StrBaseAdd(data, sstring("test5"));
    StrID s6 = StrBaseAdd(data, sstring("test6"));
    StrID s7 = StrBaseAdd(data, sstring("test7"));
    StrID s8 = StrBaseAdd(data, sstring("test8"));

    assert(s != STRBASE_INAVLID_STR);
    assert(s == s1);
    assert(data->refs[s] == 2);


    printlog("Internal Table State:\n");
    for (u32 i = 0; i < data->hashcap; i++) {
        if (data->meta[i] == -1)
            printlog("\tempty\n");
        else
            printlog("\t(%s,%d)\t%d\n", GetStr(data, data->stridx[i]),
                     data->meta[i], data->refs[data->stridx[i]]);
    }


    StrBaseFree(data);
    return 0;
}
