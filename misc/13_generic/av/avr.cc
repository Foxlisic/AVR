#include "avr.h"

AVR::AVR(int argc, char** argv) {

    FILE* fp;

    // Удвоение пикселей
    width        = 2*640;
    height       = 2*400;
    frame_length = 1000/25; // 25 fps
    pticks       = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        exit(1);
    }

    SDL_ClearError();
    screen_buffer       = (Uint32*) malloc(width * height * sizeof(Uint32));
    sdl_window          = SDL_CreateWindow("AVR Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    sdl_renderer        = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
    sdl_pixel_format    = SDL_AllocFormat(SDL_PIXELFORMAT_BGRA32);
    sdl_screen_texture  = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);

    assign();

    // Загрузка шрифтов
    for (int i = 0; i < 4096; i++) sram[0x10000 + i] = ansi16[i>>4][i & 15];
    for (int i = 0; i < 4000; i+=2) sram[0xF001 + i] = 0x07;
    for (int i = 0; i < 96; i++)   pvsram[i] = i < 32 ? 0xFF : 0x00;

    int i = 1;

    // Загрузка данных
    while (i < argc) {

        if (argv[i][0] == '-') {

            if (argv[i][1] == 'd') {
                is_debuglog = 1;
                fp_debuglog = fopen("debug.log", "w");
            }

        } else if (fp = fopen(argv[i], "rb")) {
            fseek(fp, 0, SEEK_END);
            int size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            fread(program, 1, size, fp);
            fclose(fp);
        } else {
            printf("File not exists: %s\n", argv[i]);
            exit(1);
        }

        i++;
    }

    show_fps = 0;
    in_debugger = 1;
    ds_update();
}

// Основной обработчик
int AVR::main() {

    for (;;) {

        Uint32 ticks = SDL_GetTicks();

        while (SDL_PollEvent(& evt)) {

            // Прием событий
            switch (evt.type) {

                case SDL_QUIT:

                    return 0;

                case SDL_KEYDOWN:

                    keybcode = kbd_scancode(evt.key.keysym.scancode, 0);
                    if (in_debugger) return 2;
                    kbhit = 1;
                    break;

                case SDL_KEYUP:

                    keybcode = kbd_scancode(evt.key.keysym.scancode, 1);
                    if (in_debugger) return 2;
                    kbhit = 1;
                    break;
            }
        }

        // Обновление экрана
        if (ticks - pticks >= frame_length) {

            pticks = ticks;
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

    SDL_UpdateTexture       (sdl_screen_texture, NULL, screen_buffer, width * sizeof(Uint32));
    SDL_SetRenderDrawColor  (sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear         (sdl_renderer);
    SDL_RenderCopy          (sdl_renderer, sdl_screen_texture, NULL, &dstRect);
    SDL_RenderPresent       (sdl_renderer);
}

// Уничтожение окна
int AVR::destroy() {

    free(screen_buffer);
    if (is_debuglog) fclose(fp_debuglog);

    SDL_DestroyTexture(sdl_screen_texture);
    SDL_FreeFormat(sdl_pixel_format);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    return 0;
}

// Установка точки
void AVR::pset(int x, int y, Uint32 cl) {

    if (x < 0 || y < 0 || x >= 640 || y >= 400)
        return;

    for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
        screen_buffer[width*(2*y+i) + (2*x+j)] = cl;
}

// Сканирование нажатой XT клавиши
// https://ru.wikipedia.org/wiki/Скан-код
int AVR::kbd_scancode(int scancode, int release) {

    release = (release ? 0x80 : 0x00);

    switch (scancode) {

        // Коды клавиш A-Z
        case SDL_SCANCODE_A:            return 'A' | release;
        case SDL_SCANCODE_B:            return 'B' | release;
        case SDL_SCANCODE_C:            return 'C' | release;
        case SDL_SCANCODE_D:            return 'D' | release;
        case SDL_SCANCODE_E:            return 'E' | release;
        case SDL_SCANCODE_F:            return 'F' | release;
        case SDL_SCANCODE_G:            return 'G' | release;
        case SDL_SCANCODE_H:            return 'H' | release;
        case SDL_SCANCODE_I:            return 'I' | release;
        case SDL_SCANCODE_J:            return 'J' | release;
        case SDL_SCANCODE_K:            return 'K' | release;
        case SDL_SCANCODE_L:            return 'L' | release;
        case SDL_SCANCODE_M:            return 'M' | release;
        case SDL_SCANCODE_N:            return 'N' | release;
        case SDL_SCANCODE_O:            return 'O' | release;
        case SDL_SCANCODE_P:            return 'P' | release;
        case SDL_SCANCODE_Q:            return 'Q' | release;
        case SDL_SCANCODE_R:            return 'E' | release;
        case SDL_SCANCODE_S:            return 'S' | release;
        case SDL_SCANCODE_T:            return 'T' | release;
        case SDL_SCANCODE_U:            return 'U' | release;
        case SDL_SCANCODE_V:            return 'V' | release;
        case SDL_SCANCODE_W:            return 'W' | release;
        case SDL_SCANCODE_X:            return 'X' | release;
        case SDL_SCANCODE_Y:            return 'Y' | release;
        case SDL_SCANCODE_Z:            return 'Z' | release;

        // Цифры
        case SDL_SCANCODE_0:            return '0' | release;
        case SDL_SCANCODE_1:            return '1' | release;
        case SDL_SCANCODE_2:            return '2' | release;
        case SDL_SCANCODE_3:            return '3' | release;
        case SDL_SCANCODE_4:            return '4' | release;
        case SDL_SCANCODE_5:            return '5' | release;
        case SDL_SCANCODE_6:            return '6' | release;
        case SDL_SCANCODE_7:            return '7' | release;
        case SDL_SCANCODE_8:            return '8' | release;
        case SDL_SCANCODE_9:            return '9' | release;

        // Keypad
        case SDL_SCANCODE_KP_0:         return '0' | release;
        case SDL_SCANCODE_KP_1:         return '1' | release;
        case SDL_SCANCODE_KP_2:         return '2' | release;
        case SDL_SCANCODE_KP_3:         return '3' | release;
        case SDL_SCANCODE_KP_4:         return '4' | release;
        case SDL_SCANCODE_KP_5:         return '5' | release;
        case SDL_SCANCODE_KP_6:         return '6' | release;
        case SDL_SCANCODE_KP_7:         return '7' | release;
        case SDL_SCANCODE_KP_8:         return '8' | release;
        case SDL_SCANCODE_KP_9:         return '9' | release;
        // Специальные символы
        case SDL_SCANCODE_GRAVE:        return '`' | release;
        case SDL_SCANCODE_MINUS:        return '-' | release;
        case SDL_SCANCODE_EQUALS:       return '=' | release;
        case SDL_SCANCODE_BACKSLASH:    return '\\'| release;
        case SDL_SCANCODE_LEFTBRACKET:  return '[' | release;
        case SDL_SCANCODE_RIGHTBRACKET: return ']' | release;
        case SDL_SCANCODE_SEMICOLON:    return ';' | release;
        case SDL_SCANCODE_APOSTROPHE:   return '\'' | release;
        case SDL_SCANCODE_COMMA:        return ',' | release;
        case SDL_SCANCODE_PERIOD:       return '.' | release;
        case SDL_SCANCODE_KP_DIVIDE:
        case SDL_SCANCODE_SLASH:        return '/' | release;
        case SDL_SCANCODE_BACKSPACE:    return key_BS | release;
        case SDL_SCANCODE_SPACE:        return ' ' | release;
        case SDL_SCANCODE_TAB:          return key_TAB | release;
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:       return key_LSHIFT | release;
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_RCTRL:        return key_LCTRL   | release;
        case SDL_SCANCODE_LALT:
        case SDL_SCANCODE_RALT:         return key_LALT   | release;
        case SDL_SCANCODE_KP_ENTER:
        case SDL_SCANCODE_KP_MULTIPLY:  return '*' | release;
        case SDL_SCANCODE_KP_MINUS:     return '-' | release;
        case SDL_SCANCODE_KP_PLUS:      return '+' | release;
        case SDL_SCANCODE_KP_PERIOD:    return '.' | release;
        case SDL_SCANCODE_RETURN:       return key_ENTER  | release;
        case SDL_SCANCODE_ESCAPE:       return key_ESC    | release;
        case SDL_SCANCODE_NUMLOCKCLEAR: return key_NL     | release;
        case SDL_SCANCODE_APPLICATION:  return key_APP | release;

        // F1-F12 Клавиши
        case SDL_SCANCODE_F1:           return key_F1 | release;
        case SDL_SCANCODE_F2:           return key_F2 | release;
        case SDL_SCANCODE_F3:           return key_F3 | release;
        case SDL_SCANCODE_F4:           return key_F4 | release;
        case SDL_SCANCODE_F5:           return key_F5 | release;
        case SDL_SCANCODE_F6:           return key_F6 | release;
        case SDL_SCANCODE_F7:           return key_F7 | release;
        case SDL_SCANCODE_F8:           return key_F8 | release;
        case SDL_SCANCODE_F9:           return key_F9 | release;
        case SDL_SCANCODE_F10:          return key_F10 | release;
        case SDL_SCANCODE_F11:          return key_F11 | release;
        case SDL_SCANCODE_F12:          return key_F12 | release;

        // Расширенные клавиши
        case SDL_SCANCODE_UP:           return key_UP   | release;
        case SDL_SCANCODE_DOWN:         return key_DN   | release;
        case SDL_SCANCODE_LEFT:         return key_LF   | release;
        case SDL_SCANCODE_RIGHT:        return key_RT   | release;
        case SDL_SCANCODE_INSERT:       return key_INS  | release;
        case SDL_SCANCODE_HOME:         return key_HOME | release;
        case SDL_SCANCODE_END:          return key_END  | release;
        case SDL_SCANCODE_PAGEUP:       return key_PGUP | release;
        case SDL_SCANCODE_PAGEDOWN:     return key_PGDN | release;
        case SDL_SCANCODE_DELETE:       return key_DEL  | release;
    }

    return 0;
}

// Главный обработчик
void AVR::handle_event(int signal) {

    // В данный момент процессор в режиме отладки и работает
    if (in_debugger) {

        // Нажатие на клавишу
        if (signal == 2) {

            switch (keybcode) {

                // Выполнить одну инструкцию
                case key_F7:

                    step();
                    ds_cursor = pc;
                    ds_update();
                    break;

                // Запуск процессора
                case key_F9:

                    in_debugger = 0;
                    need_repaint = 1;
                    cpu_halt = 0;
                    break;

            }
        }

    } else {

        int     show_debug = 0;
        char    fpsb[64];

        fps++;

        if (keybcode == key_F10) show_fps = !show_fps;
        if (keybcode == key_F12) show_debug = 1;

        // Профилирование
        ticks_frame = SDL_GetTicks();

        // Сколько исполнять циклов на фрейм
        uint per_frame = (25000000 / 1000) * frame_length;

        // Выполнить N инструкции на кадр
        do {

            if (is_debuglog) {

                ds_info(pc);
                fprintf(fp_debuglog, "[%08x] %05X %s\n", ds_counter++, pc, ds_line);
            }

            instr_cycles += step();
        }
        while (instr_cycles < per_frame && cpu_halt == 0);
        instr_cycles %= per_frame;

        // --------------------------------------------------
        Uint32 ticks_1 = SDL_GetTicks();

        // Мигающий курсор (и просто обновление)
        if (flash_cnt++ > 12) {
            flash = !flash;
            flash_cnt = 0;
            need_repaint =  1;
        }

        if (need_repaint) update_screen();
        Uint32 ticks_2 = SDL_GetTicks();
        // --------------------------------------------------

        if (show_fps) {

            // Отображать FPS и другую отладку
            if (ticks_2 - fps_tick >= 1000) {

                fps_tick = ticks_2;
                fps_latch = fps;
                fps = 0;
            }

            for (int y = 386; y < 400; y++)
            for (int x = 0; x < 192; x++)
                pset(x, y, 0);

            sprintf(fpsb, "FPS %d (%d+%d=%d)", fps_latch, ticks_1-ticks_frame, ticks_2-ticks_1, ticks_2-ticks_frame);
            print(0, 24, fpsb, 0xFFFF00);
        }

        // Отладочная остановка
        if (cpu_halt) show_debug = 1;

        // Был запрошен показ отладчика
        if (show_debug) {

            cpu_halt = 0;
            in_debugger = 1;
            ds_cursor = pc;
            ds_update();
        }

        need_repaint = 0;
    }

    keybcode = 0;
}

// Обновление экрана целиком
void AVR::update_screen() {

    switch (videomode) {

        // Текстовый режим [F000-FFFF]
        case 0: {

            for (int addr = 0xF000; addr < 0x10000; addr += 2) {
                print_text80(addr);
            }

            break;
        }

        // Графический 320x200, BANK=8..23
        case 1: {

            int A = 0xF000 + 8*4096;
            for (int y = 0; y < 400; y += 2)
            for (int x = 0; x < 640; x += 2) {

                uint cl = doscolor[ sram[A++] ];

                // DosColor
                pset(x,   y,   cl);
                pset(x+1, y,   cl);
                pset(x,   y+1, cl);
                pset(x+1, y+1, cl);
            }

            break;
        }
    }
}

// ---------------------------------------------------------------------
// Процедуры для работы с выводом текста
// ---------------------------------------------------------------------

// Очистить экран в цвет
void AVR::cls(uint color) {

    for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
        pset(j, i, color);
}

// Печать на экране Char
void AVR::print16char(int col, int row, unsigned char ch, uint cl) {

    col *= 8;
    row *= 16;

    for (int i = 0; i < 16; i++) {

        unsigned char hl = ansi16[ch][i];
        for (int j = 0; j < 8; j++) {
            if (hl & (1 << (7-j)))
                pset(j + col, i + row, cl);
        }
    }
}

// Печать строки
void AVR::print(int col, int row, const char* s, uint cl) {

    int i = 0;
    while (s[i]) { print16char(col, row, s[i], cl); col++; i++; }
}

// Печать на экране Char из [F000-FFFF]
void AVR::print_text80(int addr) {

    addr &= 0x0FFE;

    int col = (addr >> 1) % 80;
    int row = (addr >> 1) / 80;

    col *= 8;
    row *= 16;

    int ch   = sram[ 0xF000 + (addr & 0x0FFE) ];
    int attr = sram[ 0xF001 + (addr & 0x0FFE) ];

    for (int i = 0; i < 16; i++) {

        int hl = sram[ 0x10000 + ch*16 + i ];
        for (int j = 0; j < 8; j++) {

            uint cl = (hl & (1 << (7 - j))) ? 1 : 0;
            if (addr == text_cursor && flash && i >= 14 && i <= 15) cl = 1;

            pset(j + col, i + row, doscolor[(cl ? attr : attr>>4) & 7]);
        }
    }
}

