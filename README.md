# MegaSaturn

Emulador experimental de Mega Drive para Sega Saturn.

Os fontes PC v0.6 foram recuperados e preservados no commit `40c9e89`, antes das alterações. Os seis testes originais passaram após recompilação. Nenhuma tag de release foi criada.

Primeiro avanço: MOVE.W/L e MOVEA.W/L agora aceitam origem em An. A suíte atual passa em 7/7 testes. Os três módulos do núcleo compilam para SH-2 big-endian; execução no Saturn ainda não foi validada.

Ainda não executa jogos: faltam instruções, exceções, temporização, VDP, áudio e periféricos.

## Compilar e testar

Com CMake e um compilador C11:
```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Documentação

- [Histórico](docs/HISTORICO.md)
- [Arquitetura](docs/ARQUITETURA.md)
- [Ambiente](docs/AMBIENTE.md)
- [Pendências](docs/PENDENCIAS.md)
- [Preservação v0.6](docs/ESTADO_V0_6.md)
- [Migração](docs/MIGRACAO.md)
- [Instruções para agentes](AGENTS.md)

Licença geral pendente. O protótipo Saturn recebido usa exemplo de terceiros e permanece separado, aguardando revisão da licença e assets antes de importação pública.
