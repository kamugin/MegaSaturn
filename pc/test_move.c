#include <stdio.h>
#include <string.h>
#include "megasaturn_exec.h"
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FALHOU linha %d: %s\n",__LINE__,#x);return 1;} }while(0)
int main(void){
 static ms_bus_t bus; ms_cpu_t cpu; uint32_t value;
 /* MOVE.L #0x12345678,D0; MOVE.L D0,$FF0100; MOVE.L $FF0100,D1; MOVE.L D1,(A0) */
 static const uint8_t rom[]={
 0x20,0x3c,0x12,0x34,0x56,0x78,
 0x23,0xc0,0x00,0xff,0x01,0x00,
 0x22,0x39,0x00,0xff,0x01,0x00,
 0x20,0x81,
 0x4e,0x71};
 ms_bus_init(&bus,rom,sizeof(rom)); memset(&cpu,0,sizeof(cpu)); cpu.a[0]=0xff0200;
 CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==0x12345678 && cpu.pc==6);
 CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==12);
 CHECK(ms_read32(&bus,0xff0100,&value)==0 && value==0x12345678);
 CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[1]==value && cpu.pc==18);
 CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==20);
 CHECK(ms_read32(&bus,0xff0200,&value)==0 && value==0x12345678);
 CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==22);
 cpu.pc=0; cpu.sr=0x10; CHECK(ms_step_bus(&cpu,&bus)==0 && (cpu.sr&0x10));
 puts("MegaSaturn PC v0.4: MOVE.L e RAM APROVADOS"); return 0;
}
