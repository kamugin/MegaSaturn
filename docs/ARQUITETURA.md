# Arquitetura recuperada

- pc/megasaturn_cpu.c: subconjunto básico do 68000 sobre buffer de instruções.
- pc/megasaturn_bus.c: acessos big-endian de 8/16/32 bits, endereços de 24 bits, ROM até 4 MiB e RAM de 64 KiB em FF0000–FFFFFF. Acessos word/long ímpares são rejeitados. Mapa simplificado sem espelhos e dispositivos.
- pc/megasaturn_exec.c: fetch pelo barramento, MOVE/MOVEA, JSR (An)/abs.l e RTS. Confirma estado da CPU após sucesso.
- pc/test_*.c: testes sintéticos, sem ROM comercial.

CPU: D0–D7, A0–A7, PC e SR. Subconjunto inclui NOP, MOVEQ, BRA/BNE/BEQ, ADDQ.L/SUBQ.L em Dn e TST.L em Dn. Não há ciclos precisos, reset por vetores, exceções ou máquina completa.

O protótipo Saturn recebido usa libyaul, bitmap VDP2 e faixa colorida para indicar teste básico. Usa núcleo distinto da base PC. A interface contém copyright de exemplo e remete a LICENSE ausente no RAR; interface, assets e binários não foram importados publicamente.

Próxima integração: compartilhar o núcleo com harness Saturn e manter o barramento em armazenamento estático, evitando 64 KiB na pilha do SH-2. Compilação cruzada dos três módulos aprovada; link e execução continuam pendentes.
