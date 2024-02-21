#ifndef SNES_LIBSA1_INCLUDE
#define SNES_LIBSA1_INCLUDE

#include <stddef.h>
#include <stdint.h>

#define I_RAM_OFFSET ((uint32_t) 0x3000)

void call_s_cpu(void (*func)(), size_t args_size, ...);
void listen_call_from_sa1(void);
void call_s_cpu_target_func(void);

#endif
