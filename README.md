# Emulador CHIP-8

## Como executar

O emulador já está compilado e pronto para uso.

Abra o Prompt de Comando na pasta do projeto e acesse:

```
cd Executável
```

### Executar Pong

```
chip8.exe 10 5 "..\Jogos e Testes\Pong.ch8"
```

### Executar Tetris

```
chip8.exe 10 5 "..\Jogos e Testes\Tetris.ch8"
```

### Executar teste de opcodes

```
chip8.exe 10 5 "..\Jogos e Testes\test_opcode.ch8"
```

## Controles

O teclado do CHIP-8 foi mapeado da seguinte forma:

```
CHIP-8        Teclado
1 2 3 C       1 2 3 4
4 5 6 D       Q W E R
7 8 9 E       A S D F
A 0 B F       Z X C V
```

## Descrição

Este projeto implementa um emulador da máquina virtual CHIP-8, criada em 1977 para facilitar o desenvolvimento de jogos em diferentes computadores da época.

O emulador é capaz de carregar ROMs CHIP-8, interpretar seus opcodes e reproduzir o comportamento da plataforma original, incluindo vídeo, teclado, memória, registradores e execução de instruções.
