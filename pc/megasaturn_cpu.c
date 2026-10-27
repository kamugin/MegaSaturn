#include "megasaturn_cpu.h"

#define FLAG_C 0x0001u
#define FLAG_V 0x0002u
#define FLAG_Z 0x0004u
#define FLAG_N 0x0008u
#define FLAG_X 0x0010u

static void set_nz32(ms_cpu_t *cpu, uint32_t result)
{
    cpu->sr &= (uint16_t)~(FLAG_N | FLAG_Z | FLAG_V | FLAG_C);
    if (result == 0) cpu->sr |= FLAG_Z;
    if (result & 0x80000000u) cpu->sr |= FLAG_N;
}

int ms_step(ms_cpu_t *cpu, const uint8_t *rom, uint32_t rom_size)
{
    uint32_t pc = cpu->pc;
    uint16_t op;
    if (pc > rom_size || rom_size - pc < 2) return -1;
    op = ((uint16_t)rom[pc] << 8) | rom[pc + 1];
    if (op == 0x4E71) { cpu->pc = pc + 2; return 0; } /* NOP */
    if ((op & 0xF100u) == 0x7000u) { /* MOVEQ #imm,Dn */
        unsigned reg = (op >> 9) & 7u;
        uint32_t value = (uint32_t)(int32_t)(int8_t)(op & 0xff);
        cpu->d[reg] = value;
        set_nz32(cpu, value);
        cpu->pc = pc + 2;
        return 0;
    }
    if ((op & 0xF000u) == 0x6000u) { /* BRA, BNE, BEQ */
        unsigned condition = (op >> 8) & 15u;
        uint32_t base = pc + 2;
        int32_t disp;
        int taken;
        if (condition != 0 && condition != 6 && condition != 7) return -2;
        if ((op & 0xffu) == 0) {
            uint16_t ext;
            if (base > rom_size || rom_size - base < 2) return -1;
            ext = ((uint16_t)rom[base] << 8) | rom[base + 1];
            disp = (int16_t)ext;
        } else {
            disp = (int8_t)(op & 0xff);
        }
        taken = (condition == 0) || (condition == 6 && !(cpu->sr & FLAG_Z)) ||
                (condition == 7 && (cpu->sr & FLAG_Z));
        cpu->pc = taken ? base + (uint32_t)disp : base + ((op & 0xffu) == 0 ? 2u : 0u);
        return 0;
    }
    if ((op & 0xF1F8u) == 0x5080u || (op & 0xF1F8u) == 0x5180u) {
        /* ADDQ.L / SUBQ.L #1..8,Dn; other sizes/addressing modes pending */
        unsigned reg = op & 7u;
        uint32_t a = cpu->d[reg];
        uint32_t amount = (op >> 9) & 7u;
        uint32_t result;
        int subtract = !!(op & 0x0100u);
        int overflow, carry;
        if (amount == 0) amount = 8;
        result = subtract ? a - amount : a + amount;
        if (subtract) {
            carry = a < amount;
            overflow = ((a ^ amount) & (a ^ result) & 0x80000000u) != 0;
        } else {
            carry = result < a;
            overflow = (~(a ^ amount) & (a ^ result) & 0x80000000u) != 0;
        }
        cpu->d[reg] = result;
        set_nz32(cpu, result);
        cpu->sr &= (uint16_t)~FLAG_X;
        if (overflow) cpu->sr |= FLAG_V;
        if (carry) cpu->sr |= FLAG_C | FLAG_X;
        cpu->pc = pc + 2;
        return 0;
    }
    if ((op & 0xFFF8u) == 0x4A80u) { /* TST.L Dn */
        set_nz32(cpu, cpu->d[op & 7u]);
        cpu->pc = pc + 2;
        return 0;
    }
    return -2;
}
