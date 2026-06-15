#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

// Platform: abstrai a janela, renderização e input via SDL2
class Platform
{
public:
	// Cria janela com tamanho windowWidth x windowHeight
	// A textura interna tem tamanho textureWidth x textureHeight (64x32 para Chip-8)
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
	{
		SDL_Init(SDL_INIT_VIDEO);

		window = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowWidth,
			windowHeight,
			SDL_WINDOW_SHOWN
		);

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		// Textura RGBA onde vamos escrever o buffer de vídeo do Chip-8
		texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			textureWidth,
			textureHeight
		);
	}

	~Platform()
	{
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	// Atualiza a textura com o buffer de vídeo do Chip-8 e renderiza na tela
	void Update(void const* buffer, int pitch)
	{
		SDL_UpdateTexture(texture, nullptr, buffer, pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr); // SDL escala automaticamente
		SDL_RenderPresent(renderer);
	}

	// Processa eventos de input; retorna true se o usuário quiser sair
	// Mapeamento do teclado Chip-8:
	//   Chip-8:  1 2 3 C       Teclado: 1 2 3 4
	//            4 5 6 D                Q W E R
	//            7 8 9 E                A S D F
	//            A 0 B F                Z X C V
	bool ProcessInput(uint8_t* keys)
	{
		bool quit = false;
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE: quit     = true; break;
						case SDLK_x:      keys[0]  = 1;   break;
						case SDLK_1:      keys[1]  = 1;   break;
						case SDLK_2:      keys[2]  = 1;   break;
						case SDLK_3:      keys[3]  = 1;   break;
						case SDLK_q:      keys[4]  = 1;   break;
						case SDLK_w:      keys[5]  = 1;   break;
						case SDLK_e:      keys[6]  = 1;   break;
						case SDLK_a:      keys[7]  = 1;   break;
						case SDLK_s:      keys[8]  = 1;   break;
						case SDLK_d:      keys[9]  = 1;   break;
						case SDLK_z:      keys[0xA] = 1;  break;
						case SDLK_c:      keys[0xB] = 1;  break;
						case SDLK_4:      keys[0xC] = 1;  break;
						case SDLK_r:      keys[0xD] = 1;  break;
						case SDLK_f:      keys[0xE] = 1;  break;
						case SDLK_v:      keys[0xF] = 1;  break;
					}
					break;

				case SDL_KEYUP:
					switch (event.key.keysym.sym)
					{
						case SDLK_x:      keys[0]  = 0;   break;
						case SDLK_1:      keys[1]  = 0;   break;
						case SDLK_2:      keys[2]  = 0;   break;
						case SDLK_3:      keys[3]  = 0;   break;
						case SDLK_q:      keys[4]  = 0;   break;
						case SDLK_w:      keys[5]  = 0;   break;
						case SDLK_e:      keys[6]  = 0;   break;
						case SDLK_a:      keys[7]  = 0;   break;
						case SDLK_s:      keys[8]  = 0;   break;
						case SDLK_d:      keys[9]  = 0;   break;
						case SDLK_z:      keys[0xA] = 0;  break;
						case SDLK_c:      keys[0xB] = 0;  break;
						case SDLK_4:      keys[0xC] = 0;  break;
						case SDLK_r:      keys[0xD] = 0;  break;
						case SDLK_f:      keys[0xE] = 0;  break;
						case SDLK_v:      keys[0xF] = 0;  break;
					}
					break;
			}
		}

		return quit;
	}

private:
	SDL_Window*   window{};
	SDL_Renderer* renderer{};
	SDL_Texture*  texture{};
};
