#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>

class Platform {
public:
    Platform(char const* title, int windowWidth, int windowHeight,
             int textureWidth, int textureHeight) {

        if (!SDL_Init(SDL_INIT_VIDEO)) return;

        if (!TTF_Init()) {
            SDL_Quit();
            return;
        }

        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        menuBarHeight = 50;

        // Cria a janela
        window = SDL_CreateWindow(
            title, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE
        );
        
        renderer = SDL_CreateRenderer(window, nullptr);
        textEngine = TTF_CreateRendererTextEngine(renderer);
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
        
        // Cria os textos do menu
        text1 = TTF_CreateText(textEngine, font, "Arquivo", 0);
        text2 = TTF_CreateText(textEngine, font, "Emulação", 0);
        text3 = TTF_CreateText(textEngine, font, "Controles", 0);
        text4 = TTF_CreateText(textEngine, font, "Info", 0);

        // Define a cor dos textos
        if (text1) TTF_SetTextColor(text1, 0, 0, 0, 255);
        if (text2) TTF_SetTextColor(text2, 0, 0, 0, 255);
        if (text3) TTF_SetTextColor(text3, 0, 0, 0, 255);
        if (text4) TTF_SetTextColor(text4, 0, 0, 0, 255);

        // Cria a textura do CHIP-8
        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            textureWidth,
            textureHeight
        );

        if (!texture) return;

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    }

    ~Platform() {
        if (text1) TTF_DestroyText(text1);
        if (text2) TTF_DestroyText(text2);
        if (text3) TTF_DestroyText(text3);
        if (text4) TTF_DestroyText(text4);
        if (font) TTF_CloseFont(font);
        if (textEngine) TTF_DestroyRendererTextEngine(textEngine);

        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();
    }

    // Desenha o jogo e a interface
    void Update(void const* buffer, int pitch) {
        if (!window || !renderer || !texture) return;

        SDL_UpdateTexture(texture, nullptr, buffer, pitch);

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_FRect gameArea{};

        if (isFullscreen()) {
            gameArea = SDL_FRect{
                0.0f,
                0.0f,
                static_cast<float>(windowWidth),
                static_cast<float>(windowHeight)
            };
        } else {
            gameArea = SDL_FRect{
                0.0f,
                static_cast<float>(menuBarHeight),
                static_cast<float>(windowWidth),
                static_cast<float>(windowHeight - menuBarHeight)
            };

            drawMenuBar();
        }

        SDL_RenderTexture(renderer, texture, nullptr, &gameArea);
        SDL_RenderPresent(renderer);
    }

    // Recebe e processa comandos de teclado
    bool ProcessInput(uint8_t* keys) {
        bool quit = false;
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    switch (event.key.scancode) {
                        case SDL_SCANCODE_ESCAPE:
                            quit = true;
                            break;

                        case SDL_SCANCODE_F11: {
                            SDL_SetWindowFullscreen(window, !isFullscreen());
                            break;
                        }

                        case SDL_SCANCODE_X: keys[0] = 1; break;
                        case SDL_SCANCODE_1: keys[1] = 1; break;
                        case SDL_SCANCODE_2: keys[2] = 1; break;
                        case SDL_SCANCODE_3: keys[3] = 1; break;
                        case SDL_SCANCODE_Q: keys[4] = 1; break;
                        case SDL_SCANCODE_W: keys[5] = 1; break;
                        case SDL_SCANCODE_E: keys[6] = 1; break;
                        case SDL_SCANCODE_A: keys[7] = 1; break;
                        case SDL_SCANCODE_S: keys[8] = 1; break;
                        case SDL_SCANCODE_D: keys[9] = 1; break;
                        case SDL_SCANCODE_Z: keys[0xA] = 1; break;
                        case SDL_SCANCODE_C: keys[0xB] = 1; break;
                        case SDL_SCANCODE_4: keys[0xC] = 1; break;
                        case SDL_SCANCODE_R: keys[0xD] = 1; break;
                        case SDL_SCANCODE_F: keys[0xE] = 1; break;
                        case SDL_SCANCODE_V: keys[0xF] = 1; break;
                    }
                    break;

                case SDL_EVENT_KEY_UP:
                    switch (event.key.scancode) {
                        case SDL_SCANCODE_X: keys[0] = 0; break;
                        case SDL_SCANCODE_1: keys[1] = 0; break;
                        case SDL_SCANCODE_2: keys[2] = 0; break;
                        case SDL_SCANCODE_3: keys[3] = 0; break;
                        case SDL_SCANCODE_Q: keys[4] = 0; break;
                        case SDL_SCANCODE_W: keys[5] = 0; break;
                        case SDL_SCANCODE_E: keys[6] = 0; break;
                        case SDL_SCANCODE_A: keys[7] = 0; break;
                        case SDL_SCANCODE_S: keys[8] = 0; break;
                        case SDL_SCANCODE_D: keys[9] = 0; break;
                        case SDL_SCANCODE_Z: keys[0xA] = 0; break;
                        case SDL_SCANCODE_C: keys[0xB] = 0; break;
                        case SDL_SCANCODE_4: keys[0xC] = 0; break;
                        case SDL_SCANCODE_R: keys[0xD] = 0; break;
                        case SDL_SCANCODE_F: keys[0xE] = 0; break;
                        case SDL_SCANCODE_V: keys[0xF] = 0; break;
                    }
                    break;
            }
        }

        return quit;
    }

private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

    TTF_TextEngine* textEngine{};
    TTF_Font* font{};
    TTF_Text* text1{};
    TTF_Text* text2{};
    TTF_Text* text3{};
    TTF_Text* text4{};

    int windowWidth{};
    int windowHeight{};
    int menuBarHeight{};

    // Verifica se tá em tela cheia
    bool isFullscreen() {
        return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0;
    }

    // Desenha a barra de menu
    void drawMenuBar() {
        SDL_FRect rect{
            0.0f,
            0.0f,
            static_cast<float>(windowWidth),
            static_cast<float>(menuBarHeight)
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);

        if (text1) TTF_DrawRendererText(text1, 12.0f, 10.0f);
        if (text2) TTF_DrawRendererText(text2, 110.0f, 10.0f);
        if (text3) TTF_DrawRendererText(text3, 230.0f, 10.0f);
        if (text4) TTF_DrawRendererText(text4, 350.0f, 10.0f);
    }
};