#include "megasaturn_exec.h"

/* MOVE.B/W/L and MOVEA.W/L, selected 68000 effective-address modes.
   Address register increments/decrements are committed only after a successful
   instruction. PC is a 24-bit bus address; address registers remain 32-bit. */
static int fetch16(ms_bus_t *bus, uint32_t *pc, uint16_t *v) {
    if (ms_read16(bus,*pc,v)) return -1;
    *pc=(*pc+2u)&0xffffffu;
    return 0;
}
static int fetch32(ms_bus_t *bus, uint32_t *pc, uint32_t *v) {
    if (ms_read32(bus,*pc,v)) return -1;
    *pc=(*pc+4u)&0xffffffu;
    return 0;
}
static int read_size(ms_bus_t *bus,uint32_t addr,unsigned size,uint32_t *v) {
    uint8_t b; uint16_t w;
    if(size==1) { if(ms_read8(bus,addr,&b))return -1; *v=b; }
    else if(size==2) { if(ms_read16(bus,addr,&w))return -1; *v=w; }
    else if(ms_read32(bus,addr,v)) return -1;
    return 0;
}
static int write_size(ms_bus_t *bus,uint32_t addr,unsigned size,uint32_t v) {
    if(size==1)return ms_write8(bus,addr,(uint8_t)v)?-1:0;
    if(size==2)return ms_write16(bus,addr,(uint16_t)v)?-1:0;
    return ms_write32(bus,addr,v)?-1:0;
}
/* mode 0 Dn, 1 An (word/long source only), 2 (An), 3 (An)+, 4 -(An), 5 d16(An),
   7/0 absolute short, 7/1 absolute long, 7/4 immediate (source only). */
static int operand(ms_cpu_t *c,ms_bus_t *b,uint32_t *pc,unsigned mode,
                   unsigned reg,unsigned size,int source,uint32_t *value) {
    uint32_t addr=0,delta=(size==1 && reg==7)?2u:size;
    uint16_t ext;
    if(mode==1) {
        if(!source || size==1)return -2;
        *value=c->a[reg] & (size==2?0xffffu:0xffffffffu);
        return 0;
    }
    if(mode==0) {
        if(source) *value=c->d[reg] & (size==1?0xffu:size==2?0xffffu:0xffffffffu);
        else {
            uint32_t mask=size==1?0xffu:size==2?0xffffu:0xffffffffu;
            c->d[reg]=(c->d[reg]&~mask)|(*value&mask);
        }
        return 0;
    }
    if(mode==2 || mode==3) addr=c->a[reg];
    else if(mode==4) addr=c->a[reg]-delta;
    else if(mode==5) {
        if(fetch16(b,pc,&ext))return -1;
        addr=c->a[reg]+(uint32_t)(int32_t)(int16_t)ext;
    } else if(mode==7 && reg==0) {
        if(fetch16(b,pc,&ext))return -1;
        addr=(uint32_t)(int32_t)(int16_t)ext;
    } else if(mode==7 && reg==1) {
        if(fetch32(b,pc,&addr))return -1;
    } else if(mode==7 && reg==4 && source) {
        if(size==4)return fetch32(b,pc,value);
        if(fetch16(b,pc,&ext))return -1;
        *value=size==1?(ext&0xffu):ext;
        return 0;
    } else return -2;
    if(source) { if(read_size(b,addr,size,value))return -1; }
    else if(write_size(b,addr,size,*value))return -1;
    if(mode==3)c->a[reg]+=delta;
    if(mode==4)c->a[reg]-=delta;
    return 0;
}
static int move_instruction(ms_cpu_t *cpu,ms_bus_t *bus,uint16_t op) {
    unsigned size,sm=(op>>3)&7u,sr=op&7u,dm=(op>>6)&7u,dr=(op>>9)&7u;
    uint32_t pc=(cpu->pc+2u)&0xffffffu,value=0,mask;
    ms_cpu_t next=*cpu;
    int rc;
    switch(op>>12) {case 1:size=1;break;case 2:size=4;break;case 3:size=2;break;default:return -2;}
    /* MOVEA: destination address register, byte size invalid; CCR unchanged. */
    if(dm==1) {
        if(size==1)return -2;
        rc=operand(&next,bus,&pc,sm,sr,size,1,&value);
        if(rc)return rc;
        next.a[dr]=size==2?(uint32_t)(int32_t)(int16_t)value:value;
        next.pc=pc;*cpu=next;return 0;
    }
    if(sm==1 && size==1)return -2; /* 68000 forbids byte reads from An. */
    /* Validate destination before reading source (avoid unsupported partial changes). */
    if(!(dm==0 || dm==2 || dm==3 || dm==4 || dm==5 || (dm==7 && dr<=1)))return -2;
    rc=operand(&next,bus,&pc,sm,sr,size,1,&value);
    if(rc)return rc;
    rc=operand(&next,bus,&pc,dm,dr,size,0,&value);
    if(rc)return rc;
    mask=size==1?0xffu:size==2?0xffffu:0xffffffffu;
    value&=mask;
    next.sr &= (uint16_t)~0x000fu;
    if(value==0)next.sr|=0x0004u;
    if(value & (size==1?0x80u:size==2?0x8000u:0x80000000u))next.sr|=0x0008u;
    next.pc=pc;*cpu=next;return 0;
}
int ms_step_bus(ms_cpu_t *cpu, ms_bus_t *bus) {
    uint16_t opcode,extension;
    uint8_t instruction[4];
    ms_cpu_t next;
    uint32_t origin=cpu->pc;
    int result;
    if(ms_read16(bus,origin,&opcode))return -1;
    /* JSR (An) / JSR absolute long, and RTS: supervisor mode not yet modeled. */
    if (opcode == 0x4e75u) { /* RTS */
        uint32_t return_pc;
        if (ms_read32(bus,cpu->a[7],&return_pc)) return -1;
        next=*cpu;
        next.a[7]+=4u;
        next.pc=return_pc&0xffffffu;
        *cpu=next;
        return 0;
    }
    if ((opcode&0xfff8u)==0x4e90u || opcode==0x4eb9u) { /* JSR (An) / abs.l */
        uint32_t target,return_pc=(origin+2u)&0xffffffu;
        if(opcode==0x4eb9u) {
            if(ms_read32(bus,return_pc,&target))return -1;
            return_pc=(return_pc+4u)&0xffffffu;
        } else target=cpu->a[opcode&7u];
        if(ms_write32(bus,cpu->a[7]-4u,return_pc))return -1;
        next=*cpu;
        next.a[7]-=4u;
        next.pc=target&0xffffffu;
        *cpu=next;
        return 0;
    }
    if((opcode>>12)>=1 && (opcode>>12)<=3)return move_instruction(cpu,bus,opcode);
    instruction[0]=(uint8_t)(opcode>>8);
    instruction[1]=(uint8_t)opcode;
    instruction[2]=instruction[3]=0;
    if((opcode&0xf000u)==0x6000u && (opcode&0xffu)==0) {
        if(ms_read16(bus,origin+2u,&extension))return -1;
        instruction[2]=(uint8_t)(extension>>8);
        instruction[3]=(uint8_t)extension;
    }
    next=*cpu;next.pc=0;
    result=ms_step(&next,instruction,sizeof(instruction));
    if(result)return result;
    next.pc=(origin+next.pc)&0xffffffu;
    *cpu=next;return 0;
}
