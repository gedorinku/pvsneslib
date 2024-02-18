#ifndef SNES_LIBSA1_INCLUDE
#define SNES_LIBSA1_INCLUDE

#include <stddef.h>

void call_s_cpu(void (*func)(), size_t ret_size, void *ret, size_t args_size, ...);

#endif
