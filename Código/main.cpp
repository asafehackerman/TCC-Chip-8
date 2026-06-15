#include "Chip8.h"
#include "Platform.h"
#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{
	// Uso: chip8 <escala> <delay_ms> <arquivo.ch8>
	// Exemplo: chip8 10 1 test_opcode.ch8
	if (argc != 4)
	{
		std::cerr << "Uso: " << argv[0] << " <escala> <delay_ms> <ROM>\n";
		std::cerr << "  escala   : fator de escala da janela (ex: 10 para 640x320)\n";
		std::cerr << "  delay_ms : tempo em ms entre ciclos (ex: 1 para rapido, 3 para jogos)\n";
		std::cerr << "  ROM      : caminho para o arquivo .ch8\n";
		std::exit(EXIT_FAILURE);
	}

	int         videoScale  = std::stoi(argv[1]);
	int         cycleDelay  = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	// Cria a janela SDL2 (64*escala x 32*escala pixels)
	Platform platform(
		"CHIP-8 Emulator",
		VIDEO_WIDTH  * videoScale,
		VIDEO_HEIGHT * videoScale,
		VIDEO_WIDTH,
		VIDEO_HEIGHT
	);

	// Inicializa o emulador e carrega a ROM
	Chip8 chip8;
	chip8.LoadROM(romFilename);

	// pitch = largura em bytes de uma linha do buffer de vídeo
	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		// Processa eventos de teclado e verifica se deve sair
		quit = platform.ProcessInput(chip8.keypad);

		auto  currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(
			currentTime - lastCycleTime
		).count();

		// Executa um ciclo apenas quando o delay passou
		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.Cycle();

			// Atualiza a tela com o buffer de vídeo atual
			platform.Update(chip8.video, videoPitch);
		}
	}

	return 0;
}
