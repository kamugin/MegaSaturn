#ifndef MEGASATURN_CPU_H
#define MEGASATURN_CPU_H
#include <stdint.h>
typedef struct {
    uint32_t d[8];
    uint32_t a[8];
    uint32_t pc;
    uint16_t sr;
} ms_cpu_t;
/* Returns 0 on success, -1 for truncated fetch, -2 for unsupported opcode. */
int ms_step(ms_cpu_t *cpu, const uint8_t *rom, uint32_t rom_size);
#endif
