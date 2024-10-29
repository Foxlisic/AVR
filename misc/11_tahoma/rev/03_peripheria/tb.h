#include <SDL2/SDL.h>

#include "obj_dir/Vgpu.h"
#include "obj_dir/Vcpu.h"
#include "obj_dir/Vps2.h"

#include <stdlib.h>
#include <stdio.h>

#include "font.h"

class App {
protected:

    SDL_Window*         sdl_window;
    SDL_Renderer*       sdl_renderer;
    SDL_Texture*        sdl_screen_texture;
    Uint32*             screen_buffer;

    // SYSTEM
    int     pticks = 0, width, height, scale;

    // VGA
    int      x = 0, y = 0, _hs = 1, _vs = 0;
    int      font_addr_cu = 0, vidmod = 0;
    uint8_t* himem;
    uint8_t  fontmem[4096];
    uint8_t  memory [65536];
    uint16_t program[65536];

    // PS2-KEYBOARD
    int     ps_clock = 0, ps_data = 0, kbd_phase  = 0, kbd_ticker = 0;
    uint8_t kbd[256],     kbd_top = 0, kb_hit_cnt = 0, kb_latch   = 0, kb_data = 0;

    // SD-CARD
    int     sd_data_o, sd_data_i, sd_timeout = 1;
    int     sd_state,  sd_arg,    sd_next_cmd, sd_count;
    uint8_t sd_sector[512];

    // MODULES
    Vcpu*       mod_cpu;
    Vgpu*       mod_gpu;
    Vps2*       mod_ps2;

public:

    App(int argc, char** argv, const char* name = "AVR CPU CORE") {

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            exit(1);
        }

        FILE* fp;

        width  = 640;
        height = 400;
        scale  = 2;
        himem  = (uint8_t*) malloc(256*256*4); // 256k
        vidmod = 0;

        // Заполнение памяти дефолтными значениями
        for (int i = 0; i < 4096; i++) { memory[0xF000+i] = 0x07; fontmem[i] = font[i]; }
        for (int i = 0; i < 65536; i++) program[i] = 0x0000;

        // Разбор аргументов
        for (int i = 1; i < argc; i++) {

            // Параметры
            if (argv[i][0] == '-') {

            }
            // Загрузить программу, если она задана
            else {

                if (fp = fopen(argv[i], "rb")) {
                    fread(program, 2, 65536, fp);
                    fclose(fp);
                }
            }
        }

        // Инициализация окна
        screen_buffer = (Uint32*) malloc(width * height * sizeof(Uint32));
        sdl_window    = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * width, scale * height, SDL_WINDOW_SHOWN);
        sdl_renderer  = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
        sdl_screen_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);

        // Создание объектов
        mod_cpu     = new Vcpu;
        mod_gpu     = new Vgpu;
        mod_ps2     = new Vps2;

        // Сброс процессора
        mod_cpu->reset_n = 0;
        mod_cpu->clock   = 0; mod_cpu->eval();
        mod_cpu->clock   = 1; mod_cpu->eval();
        mod_cpu->reset_n = 1;

        sd_timeout = 1;
    }

    // Обработка одного такта
    void tick() {

        // Обработка клавиатуры
        kbd_pop(ps_clock, ps_data);

        // Прием символа и вызов прерывания
        if (mod_ps2->done) {

            kb_data       = mod_ps2->data;
            kb_latch      = ~kb_latch;
            mod_cpu->intr = kb_latch;
            mod_cpu->vect = 1;
        }

        // Реализация видеоадаптера
        mod_gpu->char_data = memory [mod_gpu->char_address | 0xF000];
        mod_gpu->font_data = fontmem[mod_gpu->font_address];
        mod_gpu->himm_data = himem  [mod_gpu->himm_address];

        // Реализация процессора
        if (mod_cpu->we) {

            memory[mod_cpu->address] = mod_cpu->data_o;

            // Запись в порты
            switch (mod_cpu->address) {

                case 0x20:

                    if (vidmod == 0)
                        fontmem[font_addr_cu] = mod_cpu->data_o;

                    if (vidmod == 1 && mod_cpu->data_o < 0x10)
                        himem  [font_addr_cu] = mod_cpu->data_o;

                    font_addr_cu = (font_addr_cu + 1) & 0x3FFFF;
                    break;

                case 0x21: font_addr_cu = 256*(font_addr_cu & 0x3FF) + mod_cpu->data_o; break;
                case 0x22: sd_data_o = mod_cpu->data_o; break;
                case 0x23: spi_process(mod_cpu->data_o & 3); break;
                case 0x24: mod_gpu->cursor = 256*(mod_gpu->cursor & 7) + mod_cpu->data_o; break;
                case 0x25: vidmod = mod_cpu->data_o & 1; mod_gpu->mode = vidmod; break;
            }
        }

        // Чтение из памяти
        mod_cpu->ir = program [mod_cpu->pc];

        switch (mod_cpu->address) {

            case 0x20: mod_cpu->data_i = kb_data; break;
            case 0x21: mod_cpu->data_i = vidmod ? himem [font_addr_cu & 0x3FFF] : fontmem[font_addr_cu & 0xFFF]; break;
            case 0x22: mod_cpu->data_i = sd_data_i; break;
            case 0x23: mod_cpu->data_i = sd_timeout*0x40; break;
            default:   mod_cpu->data_i = memory  [mod_cpu->address];
        }

        // Отладка
        // printf("%04x = %04x\n", mod_cpu->pc, mod_cpu->ir);

        // Такт на процессор
        mod_cpu->clock = 0; mod_cpu->eval();
        mod_cpu->clock = 1; mod_cpu->eval();

        // Такт на видеопроцессор
        mod_gpu->clock = 0; mod_gpu->eval();
        mod_gpu->clock = 1; mod_gpu->eval();

        // Такт на PS2
        mod_ps2->ps_clock = ps_clock;
        mod_ps2->ps_data  = ps_data;
        mod_ps2->clock    = 0; mod_ps2->eval();
        mod_ps2->clock    = 1; mod_ps2->eval();

        // Вывод на экран
        vga(mod_gpu->hs, mod_gpu->vs, (mod_gpu->r*16)*65536 + (mod_gpu->g*16)*256 + (mod_gpu->b*16));
    }

    // Ожидание событий
    int main() {

        SDL_Event evt;

        for (;;) {

            Uint32 ticks = SDL_GetTicks();

            // Обработать все новые события
            while (SDL_PollEvent(& evt)) {

                switch (evt.type) {

                    case SDL_QUIT:

                        return 0;

                    case SDL_KEYDOWN:

                        kbd_scancode(evt.key.keysym.scancode, 0);
                        break;

                    case SDL_KEYUP:

                        kbd_scancode(evt.key.keysym.scancode, 1);
                        break;
                }
            }

            // Истечение таймаута: обновление экрана
            if (ticks - pticks >= 40) {

                pticks = ticks;
                update();
                return 1;
            }

            SDL_Delay(1);
        }
    }

    // Сканирование нажатой клавиши
    // https://ru.wikipedia.org/wiki/Скан-код
    void kbd_scancode(int scancode, int release) {

        switch (scancode) {

            // Коды клавиш A-Z
            case SDL_SCANCODE_A: if (release) kbd_push(0xF0); kbd_push(0x1C); break;
            case SDL_SCANCODE_B: if (release) kbd_push(0xF0); kbd_push(0x32); break;
            case SDL_SCANCODE_C: if (release) kbd_push(0xF0); kbd_push(0x21); break;
            case SDL_SCANCODE_D: if (release) kbd_push(0xF0); kbd_push(0x23); break;
            case SDL_SCANCODE_E: if (release) kbd_push(0xF0); kbd_push(0x24); break;
            case SDL_SCANCODE_F: if (release) kbd_push(0xF0); kbd_push(0x2B); break;
            case SDL_SCANCODE_G: if (release) kbd_push(0xF0); kbd_push(0x34); break;
            case SDL_SCANCODE_H: if (release) kbd_push(0xF0); kbd_push(0x33); break;
            case SDL_SCANCODE_I: if (release) kbd_push(0xF0); kbd_push(0x43); break;
            case SDL_SCANCODE_J: if (release) kbd_push(0xF0); kbd_push(0x3B); break;
            case SDL_SCANCODE_K: if (release) kbd_push(0xF0); kbd_push(0x42); break;
            case SDL_SCANCODE_L: if (release) kbd_push(0xF0); kbd_push(0x4B); break;
            case SDL_SCANCODE_M: if (release) kbd_push(0xF0); kbd_push(0x3A); break;
            case SDL_SCANCODE_N: if (release) kbd_push(0xF0); kbd_push(0x31); break;
            case SDL_SCANCODE_O: if (release) kbd_push(0xF0); kbd_push(0x44); break;
            case SDL_SCANCODE_P: if (release) kbd_push(0xF0); kbd_push(0x4D); break;
            case SDL_SCANCODE_Q: if (release) kbd_push(0xF0); kbd_push(0x15); break;
            case SDL_SCANCODE_R: if (release) kbd_push(0xF0); kbd_push(0x2D); break;
            case SDL_SCANCODE_S: if (release) kbd_push(0xF0); kbd_push(0x1B); break;
            case SDL_SCANCODE_T: if (release) kbd_push(0xF0); kbd_push(0x2C); break;
            case SDL_SCANCODE_U: if (release) kbd_push(0xF0); kbd_push(0x3C); break;
            case SDL_SCANCODE_V: if (release) kbd_push(0xF0); kbd_push(0x2A); break;
            case SDL_SCANCODE_W: if (release) kbd_push(0xF0); kbd_push(0x1D); break;
            case SDL_SCANCODE_X: if (release) kbd_push(0xF0); kbd_push(0x22); break;
            case SDL_SCANCODE_Y: if (release) kbd_push(0xF0); kbd_push(0x35); break;
            case SDL_SCANCODE_Z: if (release) kbd_push(0xF0); kbd_push(0x1A); break;

            // Цифры
            case SDL_SCANCODE_0: if (release) kbd_push(0xF0); kbd_push(0x45); break;
            case SDL_SCANCODE_1: if (release) kbd_push(0xF0); kbd_push(0x16); break;
            case SDL_SCANCODE_2: if (release) kbd_push(0xF0); kbd_push(0x1E); break;
            case SDL_SCANCODE_3: if (release) kbd_push(0xF0); kbd_push(0x26); break;
            case SDL_SCANCODE_4: if (release) kbd_push(0xF0); kbd_push(0x25); break;
            case SDL_SCANCODE_5: if (release) kbd_push(0xF0); kbd_push(0x2E); break;
            case SDL_SCANCODE_6: if (release) kbd_push(0xF0); kbd_push(0x36); break;
            case SDL_SCANCODE_7: if (release) kbd_push(0xF0); kbd_push(0x3D); break;
            case SDL_SCANCODE_8: if (release) kbd_push(0xF0); kbd_push(0x3E); break;
            case SDL_SCANCODE_9: if (release) kbd_push(0xF0); kbd_push(0x46); break;

            // Keypad
            case SDL_SCANCODE_KP_0: if (release) kbd_push(0xF0); kbd_push(0x70); break;
            case SDL_SCANCODE_KP_1: if (release) kbd_push(0xF0); kbd_push(0x69); break;
            case SDL_SCANCODE_KP_2: if (release) kbd_push(0xF0); kbd_push(0x72); break;
            case SDL_SCANCODE_KP_3: if (release) kbd_push(0xF0); kbd_push(0x7A); break;
            case SDL_SCANCODE_KP_4: if (release) kbd_push(0xF0); kbd_push(0x6B); break;
            case SDL_SCANCODE_KP_5: if (release) kbd_push(0xF0); kbd_push(0x73); break;
            case SDL_SCANCODE_KP_6: if (release) kbd_push(0xF0); kbd_push(0x74); break;
            case SDL_SCANCODE_KP_7: if (release) kbd_push(0xF0); kbd_push(0x6C); break;
            case SDL_SCANCODE_KP_8: if (release) kbd_push(0xF0); kbd_push(0x75); break;
            case SDL_SCANCODE_KP_9: if (release) kbd_push(0xF0); kbd_push(0x7D); break;

            // Специальные символы
            case SDL_SCANCODE_GRAVE:        if (release) kbd_push(0xF0); kbd_push(0x0E); break;
            case SDL_SCANCODE_MINUS:        if (release) kbd_push(0xF0); kbd_push(0x4E); break;
            case SDL_SCANCODE_EQUALS:       if (release) kbd_push(0xF0); kbd_push(0x55); break;
            case SDL_SCANCODE_BACKSLASH:    if (release) kbd_push(0xF0); kbd_push(0x5D); break;
            case SDL_SCANCODE_LEFTBRACKET:  if (release) kbd_push(0xF0); kbd_push(0x54); break;
            case SDL_SCANCODE_RIGHTBRACKET: if (release) kbd_push(0xF0); kbd_push(0x5B); break;
            case SDL_SCANCODE_SEMICOLON:    if (release) kbd_push(0xF0); kbd_push(0x4C); break;
            case SDL_SCANCODE_APOSTROPHE:   if (release) kbd_push(0xF0); kbd_push(0x52); break;
            case SDL_SCANCODE_COMMA:        if (release) kbd_push(0xF0); kbd_push(0x41); break;
            case SDL_SCANCODE_PERIOD:       if (release) kbd_push(0xF0); kbd_push(0x49); break;
            case SDL_SCANCODE_SLASH:        if (release) kbd_push(0xF0); kbd_push(0x4A); break;
            case SDL_SCANCODE_BACKSPACE:    if (release) kbd_push(0xF0); kbd_push(0x66); break;
            case SDL_SCANCODE_SPACE:        if (release) kbd_push(0xF0); kbd_push(0x29); break;
            case SDL_SCANCODE_TAB:          if (release) kbd_push(0xF0); kbd_push(0x0D); break;
            case SDL_SCANCODE_CAPSLOCK:     if (release) kbd_push(0xF0); kbd_push(0x58); break;
            case SDL_SCANCODE_LSHIFT:       if (release) kbd_push(0xF0); kbd_push(0x12); break;
            case SDL_SCANCODE_LCTRL:        if (release) kbd_push(0xF0); kbd_push(0x14); break;
            case SDL_SCANCODE_LALT:         if (release) kbd_push(0xF0); kbd_push(0x11); break;
            case SDL_SCANCODE_RSHIFT:       if (release) kbd_push(0xF0); kbd_push(0x59); break;
            case SDL_SCANCODE_RETURN:       if (release) kbd_push(0xF0); kbd_push(0x5A); break;
            case SDL_SCANCODE_ESCAPE:       if (release) kbd_push(0xF0); kbd_push(0x76); break;
            case SDL_SCANCODE_NUMLOCKCLEAR: if (release) kbd_push(0xF0); kbd_push(0x77); break;
            case SDL_SCANCODE_KP_MULTIPLY:  if (release) kbd_push(0xF0); kbd_push(0x7C); break;
            case SDL_SCANCODE_KP_MINUS:     if (release) kbd_push(0xF0); kbd_push(0x7B); break;
            case SDL_SCANCODE_KP_PLUS:      if (release) kbd_push(0xF0); kbd_push(0x79); break;
            case SDL_SCANCODE_KP_PERIOD:    if (release) kbd_push(0xF0); kbd_push(0x71); break;
            case SDL_SCANCODE_SCROLLLOCK:   if (release) kbd_push(0xF0); kbd_push(0x7E); break;

            // F1-F12 Клавиши
            case SDL_SCANCODE_F1:   if (release) kbd_push(0xF0); kbd_push(0x05); break;
            case SDL_SCANCODE_F2:   if (release) kbd_push(0xF0); kbd_push(0x06); break;
            case SDL_SCANCODE_F3:   if (release) kbd_push(0xF0); kbd_push(0x04); break;
            case SDL_SCANCODE_F4:   if (release) kbd_push(0xF0); kbd_push(0x0C); break;
            case SDL_SCANCODE_F5:   if (release) kbd_push(0xF0); kbd_push(0x03); break;
            case SDL_SCANCODE_F6:   if (release) kbd_push(0xF0); kbd_push(0x0B); break;
            case SDL_SCANCODE_F7:   if (release) kbd_push(0xF0); kbd_push(0x83); break;
            case SDL_SCANCODE_F8:   if (release) kbd_push(0xF0); kbd_push(0x0A); break;
            case SDL_SCANCODE_F9:   if (release) kbd_push(0xF0); kbd_push(0x01); break;
            case SDL_SCANCODE_F10:  if (release) kbd_push(0xF0); kbd_push(0x09); break;
            case SDL_SCANCODE_F11:  if (release) kbd_push(0xF0); kbd_push(0x78); break;
            case SDL_SCANCODE_F12:  if (release) kbd_push(0xF0); kbd_push(0x07); break;

            // Расширенные клавиши
            case SDL_SCANCODE_LGUI:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x1F); break;
            case SDL_SCANCODE_RGUI:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x27); break;
            case SDL_SCANCODE_APPLICATION:  kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x2F); break;
            case SDL_SCANCODE_RCTRL:        kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x14); break;
            case SDL_SCANCODE_RALT:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x11); break;
            case SDL_SCANCODE_KP_DIVIDE:    kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x4A); break;
            case SDL_SCANCODE_KP_ENTER:     kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x5A); break;

            case SDL_SCANCODE_INSERT:       kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x70); break;
            case SDL_SCANCODE_HOME:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x6C); break;
            case SDL_SCANCODE_END:          kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x69); break;
            case SDL_SCANCODE_PAGEUP:       kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x7D); break;
            case SDL_SCANCODE_PAGEDOWN:     kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x7A); break;
            case SDL_SCANCODE_DELETE:       kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x71); break;

            case SDL_SCANCODE_UP:           kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x75); break;
            case SDL_SCANCODE_DOWN:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x72); break;
            case SDL_SCANCODE_LEFT:         kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x6B); break;
            case SDL_SCANCODE_RIGHT:        kbd_push(0xE0); if (release) kbd_push(0xF0); kbd_push(0x74); break;

            // Клавиша PrnScr
            case SDL_SCANCODE_PRINTSCREEN: {

                if (release == 0) {

                    kbd_push(0xE0); kbd_push(0x12);
                    kbd_push(0xE0); kbd_push(0x7C);

                } else {

                    kbd_push(0xE0); kbd_push(0xF0); kbd_push(0x7C);
                    kbd_push(0xE0); kbd_push(0xF0); kbd_push(0x12);
                }

                break;
            }

            // Клавиша Pause
            case SDL_SCANCODE_PAUSE: {

                kbd_push(0xE1);
                kbd_push(0x14); if (release) kbd_push(0xF0); kbd_push(0x77);
                kbd_push(0x14); if (release) kbd_push(0xF0); kbd_push(0x77);
                break;
            }
        }
    }

    // Нажатие на клавишу
    void kbd_push(int data) {

        if (kbd_top >= 255) return;
        kbd[kbd_top] = data;
        kbd_top++;
    }

    // Извлечение PS/2
    void kbd_pop(int& ps_clock, int& ps_data) {

        // В очереди нет клавиш для нажатия
        if (kbd_top == 0) return;

        // 25000000/2000 = 12.5 kHz Очередной полутакт для PS/2
        if (++kbd_ticker >= 2000) {

            ps_clock = kbd_phase & 1;

            switch (kbd_phase) {

                // Старт-бит [=0]
                case 0: case 1: ps_data = 0; break;

                // Бит четности
                case 18: case 19:

                    ps_data = 1;
                    for (int i = 0; i < 8; i++)
                        ps_data ^= !!(kbd[0] & (1 << i));

                    break;

                // Стоп-бит [=1]
                case 20: case 21: ps_data = 1; break;

                // Небольшая задержка между нажатиями клавиш
                case 22: case 23:
                case 24: case 25:

                    ps_clock = 1;
                    ps_data  = 1;
                    break;

                // Завершение
                case 26:

                    // Удалить символ из буфера
                    for (int i = 0; i < kbd_top - 1; i++)
                        kbd[i] = kbd[i+1];

                    kbd_top--;
                    kbd_phase = -1;
                    ps_clock  = 1;
                    break;

                // Отсчет битов от 0 до 7
                // 0=2,3   | 1=4,5   | 2=6,7   | 3=8,9
                // 4=10,11 | 5=12,13 | 6=14,15 | 7=16,17
                default:

                    ps_data = !!(kbd[0] & (1 << ((kbd_phase >> 1) - 1)));
                    break;
            }

            kbd_ticker = 0;
            kbd_phase++;
        }
    }

    // Обновить экран
    void update() {

        SDL_Rect dstRect;

        dstRect.x = 0;
        dstRect.y = 0;
        dstRect.w = scale * width;
        dstRect.h = scale * height;

        SDL_UpdateTexture       (sdl_screen_texture, NULL, screen_buffer, width * sizeof(Uint32));
        SDL_SetRenderDrawColor  (sdl_renderer, 0, 0, 0, 0);
        SDL_RenderClear         (sdl_renderer);
        SDL_RenderCopy          (sdl_renderer, sdl_screen_texture, NULL, &dstRect);
        SDL_RenderPresent       (sdl_renderer);
    }

    // Очистка экрана в определенный цвет
    void cls(uint32_t color = 0) {

        for (int x = 0; x < height * width; x++)
            screen_buffer[x] = color;
    }

    // Установка точки
    void pset(int x, int y, Uint32 color) {

        if (x < 0 || y < 0 || x > width || y >= height)
            return;

        screen_buffer[y*width + x] = color;
    }

    // Чтение точки
    Uint32 point(int x, int y) {

        if (x < 0 || y < 0 || x > width || y >= height)
            return 0;

        return screen_buffer[y*width + x];
    }

    // Удалить окно
    int destroy() {

        free(screen_buffer);
        SDL_DestroyTexture  (sdl_screen_texture);
        SDL_DestroyRenderer (sdl_renderer);
        SDL_DestroyWindow   (sdl_window);
        SDL_Quit();
        return 0;
    }

    // Сохранение фрейма
    void saveframe() {

        FILE* fp = fopen("out/record.ppm", "ab");
        if (fp) {

            fprintf(fp, "P6\n# Verilator\n%d %d\n255\n", width, height);
            for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++) {

                int cl = screen_buffer[y*width + x];
                int vl = ((cl >> 16) & 255) + (cl & 0xFF00) + ((cl&255)<<16);
                fwrite(&vl, 1, 3, fp);
            }

            fclose(fp);
        }
    }

    // 640 x 400 x 70
    void vga(int hs, int vs, int color) {

        if (hs) x++;

        // Отслеживание изменений HS/VS
        if (_hs == 0 && hs == 1) { x = 0; y++; }
        if (_vs == 1 && vs == 0) { x = 0; y = 0; saveframe(); }

        // Сохранить предыдущее значение
        _hs = hs;
        _vs = vs;

        // Вывод на экран
        pset(x-49, y-35, color);
    }

    // Процессинг SPI
    // 1 READ|WRITE
    // 2 CE=0 Активация чипа
    // 3 CE=1 Деактивация
    void spi_process(int sd_cmd) {

        FILE* fp;

        // Значение по умолчанию
        sd_data_i  = 0xFF;
        sd_timeout = 0;

        // Чтение или запись
        if ((sd_cmd & 3) == 1) {

            switch (sd_state) {

                // IDLE
                case 0: {

                    // Получена команда
                    if ((sd_data_o & 0xC0) == 0x40) {

                        sd_arg   = 0;
                        sd_state = 1;

                        // Инициализация карты
                        if      (sd_data_o == 0x40) sd_next_cmd = 6;
                        else if (sd_data_o == 0x48) sd_next_cmd = 7;
                        else if (sd_data_o == 0x77) sd_next_cmd = 12;
                        else if (sd_data_o == 0x69) sd_next_cmd = 12;
                        else if (sd_data_o == 0x7A) sd_next_cmd = 13;
                        // Чтение с диска
                        else if (sd_data_o == 0x51) sd_next_cmd = 18;
                        else if (sd_data_o == 0x58) sd_next_cmd = 21;
                        else if (sd_data_o == 0x4D) sd_next_cmd = 28;
                    }

                    break;
                }

                // Прием аргумента
                case 1: case 2: case 3: case 4: {

                    sd_arg  = (sd_arg << 8) | sd_data_o;
                    sd_arg &= 0xFFFFFFFF;

                    sd_state++;
                    break;
                }

                // CRC
                case 5: sd_state = sd_next_cmd; break;

                // --------- ИСПОЛНЕНИЕ КОМАНД ---------

                // CMD0
                case 6: sd_data_i = 0x01; sd_state = 0; break;   // Ответ 01

                // CMD48
                case 7:  sd_data_i = 0x00; sd_state = 8;  break; // Ответ 00
                case 8:  sd_data_i = 0x00; sd_state = 9;  break;
                case 9:  sd_data_i = 0x00; sd_state = 10; break;
                case 10: sd_data_i = 0x01; sd_state = 11; break;
                case 11: sd_data_i = 0xAA; sd_state = 0; break;

                // ACMD55, CMD41
                case 12: sd_data_i = 0x00; sd_state = 0; break;  // Ответ 00

                // CMD58
                case 13: sd_data_i = 0x00; sd_state = 14; break; // Ответ 00
                case 14: sd_data_i = 0xC0; sd_state = 15; break;
                case 15: sd_data_i = 0x00; sd_state = 16; break;
                case 16: sd_data_i = 0x00; sd_state = 17; break;
                case 17: sd_data_i = 0x00; sd_state = 0;  break;

                // CMD17: READ
                case 18: sd_data_i = 0x00; sd_state = 19; break; // Ответ 00
                case 19: {

                    // Ответ FE
                    sd_data_i = 0xFE;
                    sd_state  = 20;
                    sd_count  = 0;

                    fp = fopen("sd.img", "rb+");
                    if (fp) {

                        fseek(fp, 512*sd_arg, SEEK_SET);
                        fread(sd_sector, 1, 512, fp);
                        fclose(fp);
                    }

                    break;
                }

                // Чтение с карты
                case 20: {

                    sd_data_i = sd_sector[sd_count];
                    sd_count++;

                    if (sd_count == 512) sd_state = 0;
                    break;
                }

                // CMD24: WRITE
                case 21: sd_data_i = 0x00; sd_state = 22; break; // Ответ 00
                case 22: sd_data_i = 0xFE; sd_state = 23; sd_count = 0; break;
                case 23: {

                    sd_sector[sd_count] = sd_data_o;
                    sd_count++;

                    if (sd_count == 512) {

                        fp = fopen("sd.img", "rb+");

                        if (fp) {

                            fseek(fp, 512*sd_arg, SEEK_SET);
                            fwrite(sd_sector, 1, 512, fp);
                            fclose(fp);
                        }

                        sd_state = 24;
                    }

                    break;
                }

                // CRC
                case 24: sd_data_i = 0x00; sd_state = 25; break; // CRCL
                case 25: sd_data_i = 0x00; sd_state = 26; break; // CRCH
                case 26: sd_data_i = 0x05; sd_state = 27; break; // 05
                case 27: sd_data_i = 0xFF; sd_state = 0; break;  // Status

                // CMD13: Должен быть ответ 2 нуля
                case 28: sd_data_i = 0x00; sd_state = 29; break; // Ответ 00
                case 29: sd_data_i = 0x00; sd_state = 0;  break; // Ответ 00
            }
        }
        // CE=1
        else if ((sd_cmd & 3) == 3)  {
            sd_state = 0;
        }
    }
};
