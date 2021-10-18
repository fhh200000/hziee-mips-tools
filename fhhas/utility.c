#include <utility.h>
#include <stdlib.h>
#ifdef _MSC_VER

#define bswap_32(x) _byteswap_ulong(x)

#elif defined(__APPLE__)
// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#elif defined(__sun) || defined(sun)
#include <sys/byteorder.h>
#define bswap_32(x) BSWAP_32(x)
#elif defined(__FreeBSD__)
#include <sys/endian.h>
#define bswap_32(x) bswap32(x)
#elif defined(__OpenBSD__)
#include <sys/types.h>
#define bswap_32(x) swap32(x)

#elif defined(__NetBSD__)
#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_32(x) bswap32(x)
#endif
#else
#define bswap_32(x) __builtin_bswap32(x)

#endif
U32 fix_byte_order(U32 in)
{
    //如果定义了大端，则进行字节交换。
    //如果没定义，什么也不做。
    #ifdef BIG_ENDIAN
    return bswap_32(in);
    #else
    return in;
    #endif
}
void seq_list_init(seq_list* list)
{
    list->data = malloc(sizeof(void*)*SEQ_LIST_DEFAULT_LENGTH);
    list->length = SEQ_LIST_DEFAULT_LENGTH;
}
void seq_list_append(seq_list* list,void* data)
{
    if(list->used+1==list->length) { //满了
        list->data = realloc(list->data,list->length+(sizeof(void*)*SEQ_LIST_DEFAULT_LENGTH));
        list->length += SEQ_LIST_DEFAULT_LENGTH;
    }
    (list->data)[list->used] = data;
    list->used +=1;
}
void seq_list_delete(seq_list* list)
{
    free(list->data);
}
