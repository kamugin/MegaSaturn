#include <stdio.h>
#include <string.h>
#include "megasaturn_exec.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FALHOU linha %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(void) {
    static ms_bus_t bus;
    ms_cpu_t cpu;
    static const uint8_t rom[] = {0x70,0x05,0x52,0x80,0x52,0x80,0x4e,0x71};
    ms_bus_init(&bus,rom,sizeof(rom));
    memset(&cpu,0,sizeof(cpu));
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==5 && cpu.pc==2);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==6 && cpu.pc==4);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==7 && cpu.pc==6);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==8);
    CHECK(ms_step_bus(&cpu,&bus)==-1 && cpu.pc==8);
    CHECK(ms_write16(&bus,0xff0000,0x70ff)==0);
    cpu.pc=0xff0000;
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.d[0]==0xffffffffu && cpu.pc==0xff0002);
    CHECK(ms_write16(&bus,0xff0002,0x60fc)==0);
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==0xff0000);
    CHECK(ms_write16(&bus,0xff0002,0x6000)==0);
    CHECK(ms_write16(&bus,0xff0004,0x0004)==0);
    cpu.pc=0xff0002;
    CHECK(ms_step_bus(&cpu,&bus)==0 && cpu.pc==0xff0008);
    CHECK(ms_write16(&bus,0xff0010,0xffff)==0);
    cpu.pc=0xff0010;
    CHECK(ms_step_bus(&cpu,&bus)==-2 && cpu.pc==0xff0010);
    cpu.pc=0xff0001;
    CHECK(ms_step_bus(&cpu,&bus)==-1 && cpu.pc==0xff0001);
    puts("MegaSaturn PC v0.3: integracao CPU + MEMORIA APROVADA");
    return 0;
}
