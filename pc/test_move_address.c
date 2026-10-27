#include "megasaturn_exec.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void) {
    /* MOVE.L A0,D1; MOVE.W A0,D2; MOVEA.W A0,A1; MOVEA.L A0,A2;
       MOVE.L A0,(A3); MOVE.W A0,(A3); MOVE.B A0,D0 (illegal). */
    static const uint8_t rom[] = {
        0x22,0x08, 0x34,0x08, 0x32,0x48, 0x24,0x48,
        0x26,0x88, 0x36,0x88, 0x10,0x08
    };
    static ms_bus_t bus;
    ms_cpu_t cpu = {0}, before;
    uint32_t value;
    uint16_t word;
    ms_bus_init(&bus, rom, sizeof rom);
    cpu.a[0] = 0x12348001u;
    cpu.a[3] = 0xff0100u;
    cpu.d[2] = 0xabcd0000u;
    cpu.sr = 0x201fu;
    CHECK(ms_step_bus(&cpu, &bus) == 0);
    CHECK(cpu.d[1] == 0x12348001u && cpu.sr == 0x2010u);
    CHECK(ms_step_bus(&cpu, &bus) == 0);
    CHECK(cpu.d[2] == 0xabcd8001u && cpu.sr == 0x2018u);
    cpu.sr = 0x201fu;
    CHECK(ms_step_bus(&cpu, &bus) == 0);
    CHECK(cpu.a[1] == 0xffff8001u && cpu.sr == 0x201fu);
    CHECK(ms_step_bus(&cpu, &bus) == 0);
    CHECK(cpu.a[2] == 0x12348001u && cpu.sr == 0x201fu);
    CHECK(ms_step_bus(&cpu, &bus) == 0);
    CHECK(ms_read32(&bus, 0xff0100u, &value) == 0 && value == 0x12348001u);
    CHECK(ms_step_bus(&cpu, &bus) == 0);
    CHECK(ms_read16(&bus, 0xff0100u, &word) == 0 && word == 0x8001u);
    before = cpu;
    CHECK(ms_step_bus(&cpu, &bus) == -2);
    CHECK(memcmp(&cpu, &before, sizeof cpu) == 0);
    /* The same addressing mode must handle zero and preserve X. */
    cpu.pc = 0; cpu.a[0] = 0; cpu.sr = 0x201fu;
    CHECK(ms_step_bus(&cpu, &bus) == 0 && cpu.d[1] == 0 && cpu.sr == 0x2014u);
    /* A failed memory destination must not commit CPU state. */
    cpu.pc = 8; cpu.a[3] = 0x100u; before = cpu;
    CHECK(ms_step_bus(&cpu, &bus) == -1);
    CHECK(memcmp(&cpu, &before, sizeof cpu) == 0);
    puts("MOVE/MOVEA from address registers: PASS");
    return 0;
}
