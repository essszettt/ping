// limit the size of printf
// #pragma printf = "%s %c %d %u"
#pragma printf = "%s %d %u"

// limit the size of scanf
#pragma scanf = "%u"

// room for one exit function
#pragma output CLIB_EXIT_STACK_SIZE = 1

// create heap of given site
#pragma output CLIB_MALLOC_HEAP_SIZE = 0
