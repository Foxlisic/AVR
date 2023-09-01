#include "av.h"

// -----------------------------------------------------------------------------
// ОБЩИЕ МЕТОДЫ
// -----------------------------------------------------------------------------

AVR::AVR(int w, int h, int scale, int fps) {

    unsigned format = SDL_PIXELFORMAT_BGRA32;

    _scale  = scale;
    _width  = w;
    _height = h;

    width   = w * scale;
    height  = h * scale;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        exit(1);
    }

    SDL_ClearError();

    // Создать окно
    sdl_window          = SDL_CreateWindow("FOX AVR EMULATOR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    sdl_renderer        = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
    sdl_pixel_format    = SDL_AllocFormat(format);
    sdl_screen_texture  = SDL_CreateTexture(sdl_renderer, format, SDL_TEXTUREACCESS_STREAMING, _width, _height);
    SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);

    screen_buffer       = (Uint32*)malloc(width * height * sizeof(Uint32));
    frame_length        = 1000 / (fps ? fps : 1);
    frame_prev_ticks    = SDL_GetTicks();

    sram    = (uint8_t*)  malloc(1024*1024);
    program = (uint16_t*) malloc(64*1024*2);

    assign();
}

AVR::~AVR() {

    SDL_DestroyTexture(sdl_screen_texture);
    SDL_FreeFormat(sdl_pixel_format);
    SDL_DestroyRenderer(sdl_renderer);

    free(screen_buffer);
    free(program);
    free(sram);

    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}

void AVR::load(int argc, char** argv) {

    if (argc > 1) {

        FILE* fp = fopen(argv[1], "rb");
        if (fp) {

            fread(program, 2, 65536, fp);
            fclose(fp);

        } else {
            printf("File not found\n");
            exit(1);
        }
    }
}

// Ожидание событий
int AVR::main() {

    for (;;) {

        Uint32 ticks = SDL_GetTicks();

        // Ожидать наступления события
        while (SDL_PollEvent(& evt)) {

            switch (evt.type) {

                // Выход из программы по нажатии "крестика"
                case SDL_QUIT: {
                    return 0;
                }

                // https://wiki.machinesdl.org/SDL_Scancode
                case SDL_KEYDOWN:

                    kbd_scancode(evt.key.keysym.scancode, 0);
                    break;

                case SDL_KEYUP:

                    kbd_scancode(evt.key.keysym.scancode, 1);
                    break;

                // Движение мыши
                case SDL_MOUSEMOTION: {

                    mx = evt.motion.x;
                    my = evt.motion.y;
                    break;
                }

                // Движение мыши
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: {

                    // SDL_BUTTON_LEFT | SDL_BUTTON_MIDDLE | SDL_BUTTON_RIGHT
                    // SDL_PRESSED | SDL_RELEASED
                    mb = evt.button.button;
                    ms = evt.button.state;

                    break;
                }

                // Все другие события
                default: break;
            }
        }

        // Истечение таймаута: обновление экрана
        if (ticks - frame_prev_ticks >= frame_length) {

            frame_prev_ticks = ticks;
            update();
            return 1;
        }

        SDL_Delay(1);
    }
}

// Обновить окно
void AVR::update() {

    SDL_Rect dstRect;

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = width;
    dstRect.h = height;

    SDL_UpdateTexture       (sdl_screen_texture, NULL, screen_buffer, _width * sizeof(Uint32));
    SDL_SetRenderDrawColor  (sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear         (sdl_renderer);
    SDL_RenderCopy          (sdl_renderer, sdl_screen_texture, NULL, &dstRect);
    SDL_RenderPresent       (sdl_renderer);
}

// Один фрейм
void AVR::frame() {

    uint32_t instr = 0;

    Uint32 TM = SDL_GetTicks();
    while (instr < target) {

        // Отладка
        // printf("%04X: %04X\n", pc, program[pc]);

        // Вызов прерывания клавиатуры на протяжении всего фрейма
        if (flag.i && kb_id) {

            kb_id = (kb_id - 1) & 255;
            kb_code = kb[kb_id];
            interruptcall(2);
        }

        instr += step();
    }

    // TIMER IRQ
    if (flag.i) interruptcall(1);

    TM = SDL_GetTicks() - TM;

    // Корректировать максимальную скорость
    if (TM < frame_length) {
        target = (target * frame_length) / (TM ? TM : 1);
    }

    // Не более 25 Мгц
    if (target > 1000000) {
        target = 1000000;
    }
}

// -----------------------------------------------------------------------------
// ФУНКЦИИ РИСОВАНИЯ
// -----------------------------------------------------------------------------

// Установка точки
void AVR::pset(int x, int y, Uint32 cl) {

    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;

    screen_buffer[y*_width + x] = cl;
}

// Печать символа в указанном месте
void AVR::pchr(uint8_t ch) {

    int x = loc_x * 8;
    int y = loc_y * 16;

    for (int i = 0; i < 16; i++) {

        int m = ansi[ch][i];
        for (int j = 0; j < 8; j++) {

            int cl = m & (1 << (7-j)) ? fore : back;
            if (cl >= 0) pset(x + j, y + i, cl);
        }
    }

    loc_x++;
}

// Пропечатка
void AVR::print(const char* t) {

    int i = 0;
    uint8_t ch;

    while ((ch = t[i])) {

        // Прописные русские буквы
        if (ch == 0xD0) {

            ch = t[++i];

            if (ch == 0x01) ch = 0xA5;
            else if (ch >= 0x90 && ch < 0xC0) ch -= 0x10;
            else if (ch >= 0xB0 && ch < 0xC0) ch -= 0x10;
        }
        // Строчные русские буквы
        else if (ch == 0xD1) {

            ch = t[++i];

            if (ch == 0x91) ch = 0x85;
            else if (ch >= 0x80) ch += 0x60;
        }

        pchr(ch);
        i++;
    }

}

// Установить курсор
void AVR::locate(int x, int y) { loc_x = x; loc_y = y; }
