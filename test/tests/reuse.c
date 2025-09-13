#define CU_IMPL
#include <cutils.h>

#include <strbase.h>

int main() {
    StrBase *data = &(StrBase){GlobalAllocator};

    StrID strs[10] = {0};

    for (u32 i = 0; i < 10; i++) {
        for (u32 i = 0; i < ARRAY_SIZE(strs); i++) {
            static char buf[5];
            sformat((SString){.data = (i8 *)buf, .len = ARRAY_SIZE(buf)}, "test%d", i);
            strs[i] = StrBaseAdd(data, (SString){.data = (i8 *)buf, .len = ARRAY_SIZE(buf)});
            strs[i] = StrBaseAdd(data, (SString){.data = (i8 *)buf, .len = ARRAY_SIZE(buf)});
        }

        printlog("string status:\n");
        for (u32 i = 0; i < ARRAY_SIZE(strs); i++) { printlog("\t%s\n", GetStr(data, strs[i])); }

        printlog("Internal Table State:\n");
        for (u32 i = 0; i < data->hashcap; i++) {
            if (data->meta[i] == -1)
                printlog("\tempty\n");
            else
                printlog("\t(%s,%d)\t%d\n", GetStr(data, data->stridx[i]), data->meta[i],
                         data->refs[data->stridx[i]]);
        }

        for (u32 i = 0; i < ARRAY_SIZE(strs); i++) {
            static char buf[5];
            sformat((SString){.data = (i8 *)buf, .len = ARRAY_SIZE(buf)}, "test%d", i);
            StrBaseDel(data, strs[i]);
            StrBaseDel(data, strs[i]);
            assert(data->refs[strs[i]] == 0);
            assert(data->strstore[strs[i]].data == NULL);
            assert(data->strstore[strs[i]].len == 0);
        }

        printlog("string status:\n");
        for (u32 i = 0; i < ARRAY_SIZE(strs); i++) { printlog("\t%s\n", GetStr(data, strs[i])); }

        printlog("Internal Table State:\n");
        for (u32 i = 0; i < data->hashcap; i++) {
            if (data->meta[i] == -1)
                printlog("\tempty\n");
            else
                printlog("\t(%s,%d)\t%d\n", GetStr(data, data->stridx[i]), data->meta[i],
                         data->refs[data->stridx[i]]);
        }
    }

    StrBaseFree(data);
    return 0;
}
