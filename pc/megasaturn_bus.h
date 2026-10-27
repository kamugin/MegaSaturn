#ifndef MEGASATURN_BUS_H
#define MEGASATURN_BUS_H
#include <stdint.h>
#include <stddef.h>
#define MS_RAM_SIZE 65536u
#define MS_ROM_MAX 0x400000u
typedef struct {
    const uint8_t *rom;
    uint32_t rom_size;
    uint8_t ram[MS_RAM_SIZE];
} ms_bus_t;
/* 0 success; -1 unmapped/out of ROM; -2 misaligned word/long; -3 ROM write */
void ms_bus_init(ms_bus_t *bus, const uint8_t *rom, uint32_t rom_size);
int ms_read8(const ms_bus_t *bus, uint32_t addr, uint8_t *out);
int ms_read16(const ms_bus_t *bus, uint32_t addr, uint16_t *out);
int ms_read32(const ms_bus_t *bus, uint32_t addr, uint32_t *out);
int ms_write8(ms_bus_t *bus, uint32_t addr, uint8_t value);
int ms_write16(ms_bus_t *bus, uint32_t addr, uint16_t value);
int ms_write32(ms_bus_t *bus, uint32_t addr, uint32_t value);
#endif
