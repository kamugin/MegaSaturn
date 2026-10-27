# Ambiente verificado

Windows, CMake 4.4 e GCC MinGW 16.2.0. Seis testes originais aprovados com -std=c11 -O2 -Wall -Wextra -pedantic. Suíte CMake atual: 7/7 aprovados.

## Windows/MSYS2

Na raiz do checkout:
```powershell
$env:PATH = 'C:\msys64\mingw64\bin;C:\msys64\usr\bin;' + $env:PATH
cmake -S . -B build/native -G 'Unix Makefiles' -DCMAKE_C_COMPILER=C:/msys64/mingw64/bin/gcc.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/usr/bin/make.exe
cmake --build build/native
ctest --test-dir build/native --output-on-failure
```

O make MSYS2 precisou executar fora da restrição de processos do agente. Nenhuma instalação ou alteração do sistema foi necessária.

## SH-2

Compilador existente sh2eb-elf-gcc, opções -m2 -mb -std=c11 -Os -Wall -Wextra -Wpedantic -Werror -ffreestanding -c: módulos cpu/bus/exec compilados sem avisos. Apenas objetos; não houve link ou execução em hardware.

O protótipo recebido usa YAUL_INSTALL_ROOT e build.pre.mk, build.tga.mk, build.post.iso-cue.mk. Seu build completo não foi reproduzido.
