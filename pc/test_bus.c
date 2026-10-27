#include <stdio.h>
#include <string.h>
#include "megasaturn_bus.h"
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FALHOU linha %d: %s\n",__LINE__,#x);return 1;} }while(0)
int main(void) {
    static ms_bus_t b;
    const uint8_t rom[]={0x12,0x34,0x56,0x78,0x9a};
    uint8_t x8=0; uint16_t x16=0; uint32_t x32=0;
    ms_bus_init(&b,rom,sizeof rom);
    CHECK(ms_read8(&b,0,&x8)==0 && x8==0x12);
    CHECK(ms_read16(&b,0,&x16)==0 && x16==0x1234);
    CHECK(ms_read32(&b,0,&x32)==0 && x32==0x12345678u);
    CHECK(ms_read16(&b,1,&x16)==-2);
    CHECK(ms_read32(&b,3,&x32)==-2);
    CHECK(ms_read32(&b,2,&x32)==-1);
    CHECK(ms_read8(&b,5,&x8)==-1);
    CHECK(ms_write8(&b,0,0xff)==-3);
    CHECK(rom[0]==0x12);
    CHECK(ms_write32(&b,0xff0000u,0xdeadbeefu)==0);
    CHECK(ms_read32(&b,0xff0000u,&x32)==0 && x32==0xdeadbeefu);
    CHECK(ms_read16(&b,0xff0002u,&x16)==0 && x16==0xbeef);
    CHECK(ms_write16(&b,0xfffffeu,0x1357)==0);
    CHECK(ms_read16(&b,0xfffffeu,&x16)==0 && x16==0x1357);
    CHECK(ms_write32(&b,0xfffffeu,0)==-1);
    CHECK(ms_write16(&b,0xff0001u,0)==-2);
    CHECK(ms_read8(&b,0xe00000u,&x8)==-1);
    CHECK(ms_read8(&b,0x1000000u,&x8)==0 && x8==0x12);
    CHECK(ms_write8(&b,0x1ff0000u,0x5a)==0);
    CHECK(ms_read8(&b,0xff0000u,&x8)==0 && x8==0x5a);
    ms_bus_init(&b,rom,sizeof rom);
    CHECK(ms_read8(&b,0xff0000u,&x8)==0 && x8==0);
    puts("MegaSaturn PC v0.2: testes de MEMORIA APROVADOS");
    return 0;
}
