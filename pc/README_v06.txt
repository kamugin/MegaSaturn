MegaSaturn PC v0.6 (incremental sobre v0.5)

Novos opcodes: JSR (An), JSR abs.l, RTS.
JSR empilha PC de retorno em A7 e RTS o recupera.
Sem tratamento de excecoes, modo supervisor ou ciclos precisos.

MSYS2 MINGW64:
gcc -std=c11 -O2 -Wall -Wextra -pedantic megasaturn_cpu.c megasaturn_bus.c megasaturn_exec.c test_subroutine_v06.c -o test_subroutine_v06.exe
./test_subroutine_v06.exe

Outros testes: test_cpu.c, test_bus.c, test_exec.c, test_move.c, test_move_v05.c.
