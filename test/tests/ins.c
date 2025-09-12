#define CU_IMPL
#include <cutils.h>

#include <strbase.h>


int main() {
    StrBase data = {GlobalAllocator};

    StrID s = StrBaseAdd(&data, sstring("test"));
    assert(s != STRBASE_INAVLID_STR);

    return 0;
}
