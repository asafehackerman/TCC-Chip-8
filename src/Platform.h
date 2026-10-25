#pragma once

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <string>

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
        textArquivo = TTF_CreateText(textEngine, font, "Arquivo", 0);
        textEmulacao = TTF_CreateText(textEngine, font, "Emulação", 0);
        textControles = TTF_CreateText(textEngine, font, "Controles", 0);
        textInfo = TTF_CreateText(textEngine, font, "Info", 0);
        text1 = TTF_CreateText(textEngine, font, texto1.c_str(), 0);
        text2 = TTF_CreateText(textEngine, font, texto2.c_str(), 0);
        text3 = TTF_CreateText(textEngine, font, texto3.c_str(), 0);
        text4 = TTF_CreateText(textEngine, font, texto4.c_str(), 0);
        text5 = TTF_CreateText(textEngine, font, texto5.c_str(), 0);
        text6 = TTF_CreateText(textEngine, font, texto6.c_str(), 0);
        text7 = TTF_CreateText(textEngine, font, texto7.c_str(), 0);
        text8 = TTF_CreateText(textEngine, font, texto8.c_str(), 0);

        // Define a cor dos textos
        if (textArquivo) TTF_SetTextColor(textArquivo, 0, 0, 0, 255);
        if (textEmulacao) TTF_SetTextColor(textEmulacao, 0, 0, 0, 255);
        if (textControles) TTF_SetTextColor(textControles, 0, 0, 0, 255);
        if (textInfo) TTF_SetTextColor(textInfo, 0, 0, 0, 255);
        if (text1) TTF_SetTextColor(text1, 0, 0, 0, 255);
        if (text2) TTF_SetTextColor(text2, 0, 0, 0, 255);
        if (text3) TTF_SetTextColor(text3, 0, 0, 0, 255);
        if (text4) TTF_SetTextColor(text4, 0, 0, 0, 255);
        if (text5) TTF_SetTextColor(text5, 0, 0, 0, 255);
        if (text6) TTF_SetTextColor(text6, 0, 0, 0, 255);
        if (text7) TTF_SetTextColor(text7, 0, 0, 0, 255);
        if (text8) TTF_SetTextColor(text8, 0, 0, 0, 255);

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
        if (textArquivo) TTF_DestroyText(textArquivo);
        if (textEmulacao) TTF_DestroyText(textEmulacao);
        if (textControles) TTF_DestroyText(textControles);
        if (textInfo) TTF_DestroyText(textInfo);
        if (text1) TTF_DestroyText(text1);
        if (text2) TTF_DestroyText(text2);
        if (text3) TTF_DestroyText(text3);
        if (text4) TTF_DestroyText(text4);
        if (text5) TTF_DestroyText(text5);
        if (text6) TTF_DestroyText(text6);
        if (text7) TTF_DestroyText(text7);
        if (text8) TTF_DestroyText(text8);
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

                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    float mouseX, mouseY;

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        mouseX = event.button.x;
                        mouseY = event.button.y;

                        bool insideMenuBar = !isFullscreen() && mouseY >= 0 && mouseY < menuBarHeight;
                        bool insideSubMenu = mouseX >= rectSubMenu.x && mouseX < rectSubMenu.x + rectSubMenu.w && mouseY >= rectSubMenu.y && mouseY < rectSubMenu.y + rectSubMenu.h;

                        if (insideMenuBar) { // se o clique for na barra de menu, faz uma ação
                            if (mouseX >= 12 && mouseX < 90) { // Seleciona arquivo
                                optionSelected = 1;
                                cleanTexts();
                                texto1 = "Carregar Arquivo";
                                texto2 = "Fechar Arquivo";
                                texto3 = "Tela cheia";
                                texto4 = "Jogos Recentes";
                                texto5 = "Sair";

                                updateTexts();
                                
                                rectSelected = SDL_FRect{
                                    10.0f,
                                    45.0f,
                                    90.0f,
                                    5.0f
                                };
                                rectSubMenu = SDL_FRect{                                
                                    0.0f,
                                    static_cast<float>(menuBarHeight),
                                    250.0f,
                                    250.0f
                                };

                                
                            }
                            else if (mouseX >= 100 && mouseX < 200) { // Seleciona emulação
                                optionSelected = 2;
                                cleanTexts();
                                texto1 = "Pausar/Retomar";
                                texto2 = "Reiniciar";
                                texto3 = "Alterar Velocidade";
                                texto4 = "Salvar estado";
                                texto5 = "Carregar estado";
                                texto6 = "Apagar estado";

                                updateTexts();

                                rectSelected = SDL_FRect{
                                    108.0f,
                                    45.0f,
                                    110.0f,
                                    5.0f
                                };
                                rectSubMenu = SDL_FRect{                                
                                    108.0f,
                                    static_cast<float>(menuBarHeight),
                                    250.0f,
                                    250.0f
                                };
                            }
                            else if (mouseX >= 220 && mouseX < 330) { // Seleciona controles
                                optionSelected = 3;
                                cleanTexts();
                                
                                rectSelected = SDL_FRect{
                                    228.0f,
                                    45.0f,
                                    110.0f,
                                    5.0f
                                };
                                rectSubMenu = SDL_FRect{                                
                                    228.0f,
                                    static_cast<float>(menuBarHeight),
                                    250.0f,
                                    250.0f
                                };
                            }
                            else if (mouseX >= 350 && mouseX < 410) { // Seleciona info
                                optionSelected = 4;
                                cleanTexts();

                                rectSelected = SDL_FRect{
                                    348.0f,
                                    45.0f,
                                    45.0f,
                                    5.0f
                                };
                                rectSubMenu = SDL_FRect{                                
                                    348.0f,
                                    static_cast<float>(menuBarHeight),
                                    250.0f,
                                    250.0f
                                };
                            }
                        }

                        else if (!insideSubMenu) { // se o clique não for no Menu nem no Submenu, limpa as seleções
                            rectSelected = SDL_FRect{};
                            rectSubMenu = SDL_FRect{};
                        }
                    }
                }
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
    TTF_Text* textArquivo{};
    TTF_Text* textEmulacao{};
    TTF_Text* textControles{};
    TTF_Text* textInfo{};
    // Textos genéricos pro submenu
    TTF_Text* text1{};
    TTF_Text* text2{};
    TTF_Text* text3{};
    TTF_Text* text4{};
    TTF_Text* text5{};
    TTF_Text* text6{};
    TTF_Text* text7{};
    TTF_Text* text8{};
    
    SDL_FRect rectMenu{
        0.0f,
        0.0f,
        static_cast<float>(windowWidth),
        static_cast<float>(menuBarHeight)
    };
    SDL_FRect rectSelected{};
    SDL_FRect rectSubMenu{};

    int windowWidth{};
    int windowHeight{};
    int menuBarHeight{};
    int optionSelected = 0;

    std::string texto1;
    std::string texto2;
    std::string texto3;
    std::string texto4;
    std::string texto5;
    std::string texto6;
    std::string texto7;
    std::string texto8;

    // Verifica se tá em tela cheia
    bool isFullscreen() {
        return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0;
    }

    void updateTexts() {
        if (text1) TTF_SetTextString(text1, texto1.c_str(), texto1.length());
        if (text2) TTF_SetTextString(text2, texto2.c_str(), texto2.length());
        if (text3) TTF_SetTextString(text3, texto3.c_str(), texto3.length());
        if (text4) TTF_SetTextString(text4, texto4.c_str(), texto4.length());
        if (text5) TTF_SetTextString(text5, texto5.c_str(), texto5.length());
        if (text6) TTF_SetTextString(text6, texto6.c_str(), texto6.length());
        if (text7) TTF_SetTextString(text7, texto7.c_str(), texto7.length());
        if (text8) TTF_SetTextString(text8, texto8.c_str(), texto8.length());
    }

    void cleanTexts() {
        texto1.clear();
        texto2.clear();
        texto3.clear();
        texto4.clear();
        texto5.clear();
        texto6.clear();
        texto7.clear();
        texto8.clear();

        updateTexts();
    }

    // Desenha a barra de menu
    void drawMenuBar() {
        // Atualiza posição porque windowWidht e Height mudam conforme o programa
        rectMenu.w = static_cast<float>(windowWidth);
        rectMenu.h = static_cast<float>(menuBarHeight);

        // Desenha a barra de menu
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rectMenu);
        // Desenha a seleção do menu
        SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255);
        SDL_RenderFillRect(renderer, &rectSelected);
        // Desenha a seleção do submenu
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rectSubMenu);

        // Escreve os textos do menu
        if (textArquivo) TTF_DrawRendererText(textArquivo, 12.0f, 10.0f);
        if (textEmulacao) TTF_DrawRendererText(textEmulacao, 110.0f, 10.0f);
        if (textControles) TTF_DrawRendererText(textControles, 230.0f, 10.0f);
        if (textInfo) TTF_DrawRendererText(textInfo, 350.0f, 10.0f);
        if (optionSelected) {
            switch (optionSelected) {
                case 1:
                    TTF_DrawRendererText(text1, 12.0f, static_cast<float>(menuBarHeight) + 10.0f);
                    TTF_DrawRendererText(text2, 12.0f, static_cast<float>(menuBarHeight) + 40.0f);
                    TTF_DrawRendererText(text3, 12.0f, static_cast<float>(menuBarHeight) + 70.0f);
                    TTF_DrawRendererText(text4, 12.0f, static_cast<float>(menuBarHeight) + 100.0f);
                    TTF_DrawRendererText(text5, 12.0f, static_cast<float>(menuBarHeight) + 130.0f);
                break;

                case 2:
                    TTF_DrawRendererText(text1, 108.0f, static_cast<float>(menuBarHeight) + 10.0f);
                    TTF_DrawRendererText(text2, 108.0f, static_cast<float>(menuBarHeight) + 40.0f);
                    TTF_DrawRendererText(text3, 108.0f, static_cast<float>(menuBarHeight) + 70.0f);
                    TTF_DrawRendererText(text4, 108.0f, static_cast<float>(menuBarHeight) + 100.0f);
                    TTF_DrawRendererText(text5, 108.0f, static_cast<float>(menuBarHeight) + 130.0f);
                    TTF_DrawRendererText(text6, 108.0f, static_cast<float>(menuBarHeight) + 160.0f);
                break;

                case 3:

                break;

                case 4:

                break;
            }

        }
    }
};