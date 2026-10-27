#include "megasaturn_bus.h"
#include <string.h>
void ms_bus_init(ms_bus_t *b, const uint8_t *rom, uint32_t size) {
    b->rom=rom; b->rom_size=size; memset(b->ram,0,sizeof(b->ram));
}
static int region(const ms_bus_t *b,uint32_t addr,uint32_t width,int write,uint32_t *offset) {
    addr &= 0xffffffu;
    if(width>1 && (addr&1u)) return -2;
    if(addr>=0xff0000u && addr<=0xffffffu && width<=0x1000000u-addr) {
        *offset=addr-0xff0000u; return 1;
    }
    if(addr<MS_ROM_MAX && width<=MS_ROM_MAX-addr) {
        if(write) return -3;
        if(!b->rom || addr>=b->rom_size || width>b->rom_size-addr) return -1;
        *offset=addr; return 0;
    }
    return -1;
}
int ms_read8(const ms_bus_t *b,uint32_t a,uint8_t *v) {
    uint32_t o; int r=region(b,a,1,0,&o); if(r<0)return r;
    *v=r?b->ram[o]:b->rom[o]; return 0;
}
int ms_read16(const ms_bus_t *b,uint32_t a,uint16_t *v) {
    uint32_t o; int r=region(b,a,2,0,&o); if(r<0)return r;
    const uint8_t *p=r?b->ram:b->rom;
    *v=((uint16_t)p[o]<<8)|p[o+1]; return 0;
}
int ms_read32(const ms_bus_t *b,uint32_t a,uint32_t *v) {
    uint32_t o; int r=region(b,a,4,0,&o); if(r<0)return r;
    const uint8_t *p=r?b->ram:b->rom;
    *v=((uint32_t)p[o]<<24)|((uint32_t)p[o+1]<<16)|((uint32_t)p[o+2]<<8)|p[o+3]; return 0;
}
int ms_write8(ms_bus_t *b,uint32_t a,uint8_t v) {
    uint32_t o; int r=region(b,a,1,1,&o); if(r<0)return r;
    b->ram[o]=v; return 0;
}
int ms_write16(ms_bus_t *b,uint32_t a,uint16_t v) {
    uint32_t o; int r=region(b,a,2,1,&o); if(r<0)return r;
    b->ram[o]=(uint8_t)(v>>8); b->ram[o+1]=(uint8_t)v; return 0;
}
int ms_write32(ms_bus_t *b,uint32_t a,uint32_t v) {
    uint32_t o; int r=region(b,a,4,1,&o); if(r<0)return r;
    b->ram[o]=(uint8_t)(v>>24); b->ram[o+1]=(uint8_t)(v>>16);
    b->ram[o+2]=(uint8_t)(v>>8); b->ram[o+3]=(uint8_t)v; return 0;
}
