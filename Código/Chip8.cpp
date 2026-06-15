#include "Chip8.h"
#include <fstream>
#include <cstring>

// Endereço inicial do programa na memória
const unsigned int START_ADDRESS = 0x200;

// Endereço onde as fontes são carregadas
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;

// Sprites dos 16 caracteres hexadecimais (0–F), 5 bytes cada
// Cada bit representa um pixel: 1 = ligado, 0 = desligado
uint8_t fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// ============================================================
// CONSTRUTOR
// ============================================================
Chip8::Chip8()
	: randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	// O programa começa no endereço 0x200
	pc = START_ADDRESS;

	// Carrega as fontes na memória a partir de 0x50
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Inicializa o gerador de bytes aleatórios (0–255)
	randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

	// ---- Tabela principal: indexed pelo primeiro nibble do opcode ----
	table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;

	// Inicializa subtabelas com OP_NULL (opcode inválido = não faz nada)
	for (size_t i = 0; i <= 0xE; i++)
	{
		table0[i] = &Chip8::OP_NULL;
		table8[i] = &Chip8::OP_NULL;
		tableE[i] = &Chip8::OP_NULL;
	}

	// Subtabela 0x0: diferencia 00E0 e 00EE pelo último nibble
	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

	// Subtabela 0x8: operações entre registradores
	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;

	// Subtabela 0xE: input de teclado
	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

	// Subtabela 0xF: vários (timers, BCD, memória)
	for (size_t i = 0; i <= 0x65; i++)
	{
		tableF[i] = &Chip8::OP_NULL;
	}
	tableF[0x07] = &Chip8::OP_Fx07;
	tableF[0x0A] = &Chip8::OP_Fx0A;
	tableF[0x15] = &Chip8::OP_Fx15;
	tableF[0x18] = &Chip8::OP_Fx18;
	tableF[0x1E] = &Chip8::OP_Fx1E;
	tableF[0x29] = &Chip8::OP_Fx29;
	tableF[0x33] = &Chip8::OP_Fx33;
	tableF[0x55] = &Chip8::OP_Fx55;
	tableF[0x65] = &Chip8::OP_Fx65;
}

// ============================================================
// CARREGA ROM NA MEMÓRIA A PARTIR DE 0x200
// ============================================================
void Chip8::LoadROM(char const* filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		delete[] buffer;
	}
}

// ============================================================
// CICLO: FETCH → DECODE → EXECUTE
// ============================================================
void Chip8::Cycle()
{
	// FETCH: lê 2 bytes da memória e monta o opcode de 16 bits
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	// Incrementa o PC antes de executar (algumas instruções vão modificar o PC)
	pc += 2;

	// DECODE + EXECUTE: usa o primeiro nibble como índice na tabela
	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

	// Decrementa timers se estiverem ativos
	if (delayTimer > 0) { --delayTimer; }
	if (soundTimer > 0) { --soundTimer; }
}

// ============================================================
// FUNÇÕES DE DESPACHO DAS SUBTABELAS
// ============================================================

// Dispatcha opcodes que começam com 0x0 usando o último nibble
void Chip8::Table0() { ((*this).*(table0[opcode & 0x000Fu]))(); }

// Dispatcha opcodes que começam com 0x8 usando o último nibble
void Chip8::Table8() { ((*this).*(table8[opcode & 0x000Fu]))(); }

// Dispatcha opcodes que começam com 0xE usando o último nibble
void Chip8::TableE() { ((*this).*(tableE[opcode & 0x000Fu]))(); }

// Dispatcha opcodes que começam com 0xF usando os últimos 2 nibbles
void Chip8::TableF() { ((*this).*(tableF[opcode & 0x00FFu]))(); }

// Opcode inválido — não faz nada
void Chip8::OP_NULL() {}

// ============================================================
// IMPLEMENTAÇÃO DAS 34 INSTRUÇÕES
// ============================================================

// 00E0 - CLS: limpa o buffer de vídeo
void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

// 00EE - RET: retorna de subrotina (pop da pilha)
void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];
}

// 1nnn - JP addr: salta para o endereço nnn
void Chip8::OP_1nnn()
{
	uint16_t address = opcode & 0x0FFFu;
	pc = address;
}

// 2nnn - CALL addr: chama subrotina em nnn (push do PC na pilha)
void Chip8::OP_2nnn()
{
	uint16_t address = opcode & 0x0FFFu;
	stack[sp] = pc;
	++sp;
	pc = address;
}

// 3xkk - SE Vx, byte: pula próxima instrução se Vx == kk
void Chip8::OP_3xkk()
{
	uint8_t Vx   = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if (registers[Vx] == byte) { pc += 2; }
}

// 4xkk - SNE Vx, byte: pula se Vx != kk
void Chip8::OP_4xkk()
{
	uint8_t Vx   = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	if (registers[Vx] != byte) { pc += 2; }
}

// 5xy0 - SE Vx, Vy: pula se Vx == Vy
void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] == registers[Vy]) { pc += 2; }
}

// 6xkk - LD Vx, byte: carrega kk em Vx
void Chip8::OP_6xkk()
{
	uint8_t Vx   = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] = byte;
}

// 7xkk - ADD Vx, byte: Vx += kk
void Chip8::OP_7xkk()
{
	uint8_t Vx   = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] += byte;
}

// 8xy0 - LD Vx, Vy: Vx = Vy
void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vy];
}

// 8xy1 - OR Vx, Vy: Vx |= Vy
void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] |= registers[Vy];
}

// 8xy2 - AND Vx, Vy: Vx &= Vy
void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] &= registers[Vy];
}

// 8xy3 - XOR Vx, Vy: Vx ^= Vy
void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] ^= registers[Vy];
}

// 8xy4 - ADD Vx, Vy: Vx += Vy, VF = carry
void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	// Se somou mais de 255, houve overflow → VF = 1 (carry)
	registers[0xF] = (sum > 255U) ? 1 : 0;
	registers[Vx]  = sum & 0xFFu;
}

// 8xy5 - SUB Vx, Vy: Vx -= Vy, VF = NOT borrow
void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	// VF = 1 se Vx > Vy (não houve borrow), 0 caso contrário
	registers[0xF] = (registers[Vx] > registers[Vy]) ? 1 : 0;
	registers[Vx] -= registers[Vy];
}

// 8xy6 - SHR Vx: shift direita por 1, VF = LSB
void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Salva o bit menos significativo em VF antes de deslocar
	registers[0xF] = registers[Vx] & 0x1u;
	registers[Vx] >>= 1;
}

// 8xy7 - SUBN Vx, Vy: Vx = Vy - Vx, VF = NOT borrow
void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[0xF] = (registers[Vy] > registers[Vx]) ? 1 : 0;
	registers[Vx]  = registers[Vy] - registers[Vx];
}

// 8xyE - SHL Vx: shift esquerda por 1, VF = MSB
void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Salva o bit mais significativo em VF antes de deslocar
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
	registers[Vx] <<= 1;
}

// 9xy0 - SNE Vx, Vy: pula se Vx != Vy
void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] != registers[Vy]) { pc += 2; }
}

// Annn - LD I, addr: I = nnn
void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;
	index = address;
}

// Bnnn - JP V0, addr: PC = V0 + nnn
void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;
	pc = registers[0] + address;
}

// Cxkk - RND Vx, byte: Vx = random & kk
void Chip8::OP_Cxkk()
{
	uint8_t Vx   = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx] = randByte(randGen) & byte;
}

// Dxyn - DRW Vx, Vy, n: desenha sprite de n bytes a partir de I na posição (Vx, Vy)
// VF = 1 se houver colisão (pixel apagado), 0 caso contrário
void Chip8::OP_Dxyn()
{
	uint8_t Vx     = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy     = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap: se ultrapassar a borda, volta ao início
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0; // Sem colisão por padrão

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			// Verifica cada bit do sprite (da esquerda pra direita)
			uint8_t  spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			if (spritePixel)
			{
				// Colisão: pixel do sprite ligado e pixel da tela também
				if (*screenPixel == 0xFFFFFFFF) { registers[0xF] = 1; }

				// XOR: inverte o pixel da tela
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

// Ex9E - SKP Vx: pula se a tecla com valor Vx estiver pressionada
void Chip8::OP_Ex9E()
{
	uint8_t Vx  = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if (keypad[key]) { pc += 2; }
}

// ExA1 - SKNP Vx: pula se a tecla com valor Vx NÃO estiver pressionada
void Chip8::OP_ExA1()
{
	uint8_t Vx  = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if (!keypad[key]) { pc += 2; }
}

// Fx07 - LD Vx, DT: Vx = delay timer
void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delayTimer;
}

// Fx0A - LD Vx, K: espera uma tecla ser pressionada, armazena em Vx
// Implementado decrementando o PC para re-executar essa instrução até uma tecla ser detectada
void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (int i = 0; i < 16; ++i)
	{
		if (keypad[i])
		{
			registers[Vx] = i;
			return;
		}
	}

	// Nenhuma tecla pressionada: "roda" no lugar
	pc -= 2;
}

// Fx15 - LD DT, Vx: delay timer = Vx
void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delayTimer = registers[Vx];
}

// Fx18 - LD ST, Vx: sound timer = Vx
void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	soundTimer = registers[Vx];
}

// Fx1E - ADD I, Vx: I += Vx
void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += registers[Vx];
}

// Fx29 - LD F, Vx: I = endereço do sprite do dígito Vx
// Cada caractere tem 5 bytes, começando em FONTSET_START_ADDRESS
void Chip8::OP_Fx29()
{
	uint8_t Vx    = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];
	index = FONTSET_START_ADDRESS + (5 * digit);
}

// Fx33 - LD B, Vx: armazena representação BCD de Vx em I, I+1, I+2
// Ex: Vx = 234 → memory[I]=2, memory[I+1]=3, memory[I+2]=4
void Chip8::OP_Fx33()
{
	uint8_t Vx    = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	memory[index + 2] = value % 10; value /= 10; // unidades
	memory[index + 1] = value % 10; value /= 10; // dezenas
	memory[index]     = value % 10;               // centenas
}

// Fx55 - LD [I], Vx: salva V0..Vx na memória a partir de I
void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}

// Fx65 - LD Vx, [I]: lê V0..Vx da memória a partir de I
void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
}