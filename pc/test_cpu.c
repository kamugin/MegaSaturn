#include <stdio.h>
#include <string.h>
#include "megasaturn_cpu.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FALHOU linha %d: %s\n", __LINE__, #x); return 1; } } while (0)
static int run(void) {
    ms_cpu_t c;
    static const uint8_t prog[] = {
        0x70,0x00, /* MOVEQ #0,D0 => Z */
        0x67,0x02, /* BEQ +2 */
        0x70,0x09, /* skipped */
        0x52,0x80, /* ADDQ.L #1,D0 => 1 */
        0x66,0x02, /* BNE +2 */
        0x70,0x09, /* skipped */
        0x53,0x80, /* SUBQ.L #1,D0 => 0 */
        0x4a,0x80, /* TST.L D0 => Z */
        0x60,0x00,0x00,0x06, /* BRA.W +6 => PC 24 */
        0x70,0x09,0x70,0x09, /* skipped */
        0x72,0xff /* MOVEQ #-1,D1 */
    };
    memset(&c,0,sizeof(c));
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 2 && (c.sr & 4));
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 6);
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 8 && c.d[0] == 1 && !(c.sr & 4));
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 12);
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 14 && c.d[0] == 0 && (c.sr & 4));
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 16 && (c.sr & 4));
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 24);
    CHECK(ms_step(&c,prog,sizeof(prog)) == 0 && c.pc == 26 && c.d[1] == 0xffffffffu && (c.sr & 8));
    CHECK(ms_step(&c,prog,sizeof(prog)) == -1);
    { static const uint8_t backward[] = {0x70,0x01,0x60,0xfc};
      memset(&c,0,sizeof(c));
      CHECK(ms_step(&c,backward,sizeof(backward)) == 0 && c.pc == 2);
      CHECK(ms_step(&c,backward,sizeof(backward)) == 0 && c.pc == 0);
    }
    { static const uint8_t bad[] = {0xff,0xff};
      memset(&c,0,sizeof(c));
      CHECK(ms_step(&c,bad,sizeof(bad)) == -2);
    }
    return 0;
}
int main(void) { if (run()) return 1; puts("MegaSaturn PC: testes iniciais APROVADOS"); return 0; }
