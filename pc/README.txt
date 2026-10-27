MegaSaturn PC v0.5
==================
Versao experimental de interpretador Motorola 68000 para PC.
Nao executa jogos comerciais. Sem necessidade de Ymir.

Novidades: MOVE.B/W/L e MOVEA.W/L; modos Dn, (An), (An)+, -(An), d16(An),
enderecamento absoluto curto/longo e imediato (origem). A origem An direta
em MOVE e modos adicionais do 68000 ainda nao estao implementados.

No MSYS2 MINGW64:
  cd /c/SaturnDev/MegaSaturn_PC_v0_5
  gcc -std=c11 -O2 -Wall -Wextra -pedantic megasaturn_cpu.c megasaturn_bus.c megasaturn_exec.c test_move_v05.c -o test_move_v05.exe
  ./test_move_v05.exe

Resultado esperado:
  MegaSaturn PC v0.5: MOVE.B/W/L, MOVEA e enderecamento APROVADOS

Para executar os testes antigos, compile test_cpu.c apenas com
megasaturn_cpu.c; test_bus.c apenas com megasaturn_bus.c; e test_exec.c
ou test_move.c com os tres modulos.

Limites conhecidos: barramento simplificado, instrucoes 68000 incompletas,
sem VDP/audio/perifericos, sem temporizacao/ciclos ou tratamento de excecoes.
