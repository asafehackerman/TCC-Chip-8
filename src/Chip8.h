#pragma once

#include <cstdint>
#include <chrono>
#include <random>

// Dimensões do display do Chip-8
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

class Chip8
{
public:
	Chip8();
	void LoadROM(char const* filename);
	void Cycle();

	// Memória de vídeo: cada pixel é uint32 (0xFFFFFFFF = ligado, 0x00000000 = desligado)
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};

	// Estado do teclado: 16 teclas, 1 = pressionada, 0 = solta
	uint8_t keypad[16]{};

private:
	// ---- Componentes do Chip-8 ----
	uint8_t  registers[16]{};   // 16 registradores V0–VF
	uint8_t  memory[4096]{};    // 4KB de memória
	uint16_t index{};           // Registrador de índice (I)
	uint16_t pc{};              // Program Counter
	uint16_t stack[16]{};       // Pilha de 16 níveis
	uint8_t  sp{};              // Stack Pointer
	uint8_t  delayTimer{};      // Timer de delay
	uint8_t  soundTimer{};      // Timer de som
	uint16_t opcode{};          // Opcode atual

	// ---- Gerador de números aleatórios ----
	std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

	// ---- Tabelas de function pointers ----
	typedef void (Chip8::*Chip8Func)();
	Chip8Func table[0xF + 1];
	Chip8Func table0[0xE + 1];
	Chip8Func table8[0xE + 1];
	Chip8Func tableE[0xE + 1];
	Chip8Func tableF[0x65 + 1];

	// Funções de despacho para grupos de opcodes
	void Table0();
	void Table8();
	void TableE();
	void TableF();
	void OP_NULL();

	// ---- As 34 instruções do Chip-8 ----
	void OP_00E0(); // CLS  - Limpa a tela
	void OP_00EE(); // RET  - Retorna de subrotina
	void OP_1nnn(); // JP   - Salta para endereço
	void OP_2nnn(); // CALL - Chama subrotina
	void OP_3xkk(); // SE   - Pula se Vx == kk
	void OP_4xkk(); // SNE  - Pula se Vx != kk
	void OP_5xy0(); // SE   - Pula se Vx == Vy
	void OP_6xkk(); // LD   - Carrega kk em Vx
	void OP_7xkk(); // ADD  - Vx += kk
	void OP_8xy0(); // LD   - Vx = Vy
	void OP_8xy1(); // OR   - Vx |= Vy
	void OP_8xy2(); // AND  - Vx &= Vy
	void OP_8xy3(); // XOR  - Vx ^= Vy
	void OP_8xy4(); // ADD  - Vx += Vy (com carry)
	void OP_8xy5(); // SUB  - Vx -= Vy (com borrow)
	void OP_8xy6(); // SHR  - Vx >>= 1
	void OP_8xy7(); // SUBN - Vx = Vy - Vx
	void OP_8xyE(); // SHL  - Vx <<= 1
	void OP_9xy0(); // SNE  - Pula se Vx != Vy
	void OP_Annn(); // LD I - I = nnn
	void OP_Bnnn(); // JP   - PC = V0 + nnn
	void OP_Cxkk(); // RND  - Vx = random & kk
	void OP_Dxyn(); // DRW  - Desenha sprite na tela
	void OP_Ex9E(); // SKP  - Pula se tecla Vx pressionada
	void OP_ExA1(); // SKNP - Pula se tecla Vx NÃO pressionada
	void OP_Fx07(); // LD   - Vx = delay timer
	void OP_Fx0A(); // LD   - Espera tecla, guarda em Vx
	void OP_Fx15(); // LD   - delay timer = Vx
	void OP_Fx18(); // LD   - sound timer = Vx
	void OP_Fx1E(); // ADD  - I += Vx
	void OP_Fx29(); // LD F - I = endereço do sprite do dígito Vx
	void OP_Fx33(); // LD B - Armazena BCD de Vx em I, I+1, I+2
	void OP_Fx55(); // LD   - Salva V0..Vx na memória a partir de I
	void OP_Fx65(); // LD   - Lê V0..Vx da memória a partir de I
};