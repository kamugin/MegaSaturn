#include "megasaturn_exec.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FALHOU linha %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void) {
    /* MOVEQ #5,D0; JSR $00000010; NOP; at 0x10 ADDQ.L #1,D0; RTS */
    static const uint8_t rom[32]={
      0x70,0x05, 0x4e,0xb9,0,0,0,0x10, 0x4e,0x71,
      [16]=0x52,[17]=0x80,[18]=0x4e,[19]=0x75
    };
    ms_bus_t bus; ms_cpu_t cpu; uint32_t ret;
    ms_bus_init(&bus,rom,sizeof(rom)); memset(&cpu,0,sizeof(cpu));cpu.a[7]=0xff1000u;
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==5 && cpu.pc==2);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==16 && cpu.a[7]==0xff0ffcu);
    CHECK(ms_read32(&bus,cpu.a[7],&ret)==0 && ret==8);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==6 && cpu.pc==18);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==8 && cpu.a[7]==0xff1000u);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==10);
    /* JSR (A0), then RTS, also preserves flags. */
    cpu.pc=2;cpu.a[0]=18;cpu.a[7]=0xff2000u;cpu.sr=0x15u;
    CHECK(ms_write16(&bus,0xff0000u,0x4e90u)==0);
    cpu.pc=0xff0000u;
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==18 && cpu.a[7]==0xff1ffcu);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==0xff0002u && cpu.a[7]==0xff2000u && cpu.sr==0x15u);
    /* Invalid stack address: CPU state must not change. */
    cpu.pc=2;cpu.a[7]=0x100u;
    CHECK(ms_step_bus(&cpu,&bus)!=0 && cpu.pc==2 && cpu.a[7]==0x100u);
    puts("MegaSaturn PC v0.6: JSR, RTS e PILHA APROVADOS");return 0;
}
