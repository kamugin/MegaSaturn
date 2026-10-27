#include <stdio.h>
#include <string.h>
#include "megasaturn_exec.h"
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FALHOU linha %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void) {
    static ms_bus_t bus;
    ms_cpu_t cpu;
    uint8_t b;
    uint16_t w;
    /* MOVE.B #$80,D0; MOVE.W #$1234,D0; MOVEA.W #$FF80,A1;
       MOVE.B D0,(A0)+; MOVE.W D0,-(A0); MOVE.W (A0)+,D2;
       MOVE.B #0,-(A7); MOVE.L #$12345678,D3; NOP */
    static const uint8_t rom[]={
      0x10,0x3c,0x00,0x80, 0x30,0x3c,0x12,0x34,
      0x32,0x7c,0xff,0x80, 0x10,0xc0, 0x31,0x00,
      0x34,0x18, 0x1f,0x3c,0x00,0x00,
      0x26,0x3c,0x12,0x34,0x56,0x78, 0x4e,0x71
    };
    ms_bus_init(&bus,rom,sizeof(rom));memset(&cpu,0,sizeof(cpu));
    cpu.a[0]=0xff0100;cpu.a[7]=0xff0200;cpu.sr=0x10;
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==0x80 && (cpu.sr&0x18)==0x18);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==0x1234 && !(cpu.sr&0x0f));
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.a[1]==0xffffff80u && (cpu.sr&0x10));
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.a[0]==0xff0101);
    CHECK(ms_read8(&bus,0xff0100,&b)==0 && b==0x34);
    /* MOVE.W D0,-(A0): 0xff0101 -> 0xff00ff, odd word address: must fail */
    CHECK(ms_step_bus(&cpu,&bus)==-1 && cpu.a[0]==0xff0101 && cpu.pc==14);
    cpu.a[0]=0xff0102;
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.a[0]==0xff0100);
    CHECK(ms_read16(&bus,0xff0100,&w)==0 && w==0x1234);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[2]==0x1234 && cpu.a[0]==0xff0102);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.a[7]==0xff01fe && (cpu.sr&4));
    CHECK(ms_read8(&bus,0xff01fe,&b)==0 && b==0);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[3]==0x12345678 && cpu.pc==28);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==30);
    puts("MegaSaturn PC v0.5: MOVE.B/W/L, MOVEA e enderecamento APROVADOS");
    return 0;
}
