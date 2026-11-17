#include "Chip8.h"
#include "Platform.h"
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
	
	constexpr int VIDEO_SCALE = 20;
	constexpr int CYCLE_DELAY = 4;

	// Cria a janela SDL2 (64*escala x 32*escala pixels)
	Platform platform(
		"Emulador Chip-8",
		VIDEO_WIDTH  * VIDEO_SCALE,
		VIDEO_HEIGHT * VIDEO_SCALE,
		VIDEO_WIDTH,
		VIDEO_HEIGHT
	);

	// Inicializa o emulador e carrega a ROM
	Chip8 chip8;

	// pitch = largura em bytes de uma linha do buffer de vídeo
	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;
	bool romLoaded = false;

	while (!quit) {
		// Processa eventos de teclado e verifica se deve sair
		quit = platform.ProcessInput(chip8.keypad);
		platform.Update(chip8.video, videoPitch);

		std::vector<uint8_t> data = platform.GetROM();

		if (!data.empty()) { // vê se ele não pegou nenhum dado
			std::cout << "Bytes recebidos: " << data.size() << '\n';

			if (chip8.LoadROM(data)) {
				std::cout << "ROM carregada!\n";
				romLoaded = true;
			}
			else {
				std::cout << "Erro ao carregar ROM!\n";
			}
		}

		if (romLoaded) {
			auto  currentTime = std::chrono::high_resolution_clock::now();
			float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

			// Executa um ciclo apenas quando o delay passou
			if (dt > CYCLE_DELAY)
			{
				lastCycleTime = currentTime;

				chip8.Cycle();

				// Atualiza a tela
				platform.Update(chip8.video, videoPitch);

				if (platform.closeROM) {
					chip8.Reset();
					romLoaded = false;
					platform.closeROM = false;
				}
			}
		}
	}

	return 0;
}
