#ifndef MEGASATURN_EXEC_H
#define MEGASATURN_EXEC_H
#include "megasaturn_cpu.h"
#include "megasaturn_bus.h"
/* Executes one instruction fetched via the 24-bit bus. Returns 0 on success,
   -1 on bus fetch failure, -2 on unsupported opcode. */
int ms_step_bus(ms_cpu_t *cpu, ms_bus_t *bus);
#endif
