AVR::AVR(int argc, char** argv)
{
    x   = 0;
    y   = 0;
    _hs = 1;
    _vs = 0;

    scale        = 4;               // Удвоение пикселей
    width        = 320;             // Ширина экрана
    height       = 200;             // Высота экрана
    frame_length = (1000/60);       // 60 FPS
    border_color = 0;
    cursor       = 0;
    vconfig      = 2;
    device       = CYCLONE_V;

    int i = 1;
    FILE* fp;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        exit(1);
    }

    // Назначить инструкциям коды -> assign();
    reset();

    // Разбор параметров
    while (i < argc) {

        // Опции
        if (argv[i][0] == '-') {

            switch (argv[i][1]) {

                // Активация дебаггера
                case 'd': ds_enable = 1; break;

                // Циклон-4
                case '4': device = CYCLONE_IV; break;
            }

        } else {

            fp = fopen(argv[i], "rb");
            if (fp) {
                fread(program, 2, 65536, fp);   // Ты кот?
                fclose(fp);                     // А если найду?
            } else {
                printf("Program has not been are symbol table, please encode sync tables and back again in this place of MZ DOS application\n"); exit(1);
            }
        }

        i++;
    }

    // Создание окна
    SDL_ClearError();
    sdl_window          = SDL_CreateWindow("FoxAVR Machine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * width, scale * height, SDL_WINDOW_SHOWN);
    sdl_renderer        = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_PRESENTVSYNC);
    screen_buffer       = (Uint32*) malloc(width * height * sizeof(Uint32));
    sdl_screen_texture  = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_SetTextureBlendMode(sdl_screen_texture, SDL_BLENDMODE_NONE);
}

// Основной цикл работы
int AVR::main()
{
    SDL_Rect dstRect;

    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = scale * width;
    dstRect.h = scale * height;

    for (;;) {

        int ts, ms = 0, cc = 0, ips = 0, target = (25000000 / 60);
        Uint32 ticks = SDL_GetTicks();

        // Прием событий
        while (SDL_PollEvent(& evt)) {

            // Событие выхода
            switch (evt.type) {

                case SDL_QUIT: destroy(); return 0;
            }
        }

        // Выполнение инструкции 25 Мгц / 60 кадров
        while (cc < target)
        {
            disassemble();

            ts  = step();
            cc += ts;
            ms += ts;

            tstates += ts;
            ips++;

            if (ms > 25000) { ms = 0; millis = SDL_GetTicks() & 255; }
        }

        // Обновить экран
        update_screen();

        // Подсчет сколько необходимо миллисекунд ожидания после выполнения кадра
        ticks += frame_length - SDL_GetTicks();
        ticks = (ticks < 1 ? 1 : (ticks > 16 ? 16 : ticks));

        SDL_UpdateTexture       (sdl_screen_texture, NULL, screen_buffer, width * sizeof(Uint32));
        SDL_SetRenderDrawColor  (sdl_renderer, 0, 0, 0, 0);
        SDL_RenderClear         (sdl_renderer);
        SDL_RenderCopy          (sdl_renderer, sdl_screen_texture, NULL, & dstRect);
        SDL_RenderPresent       (sdl_renderer);

        // Ожидать нерастраченное время
        SDL_Delay(ticks);
    }
}

void AVR::disassemble()
{
    if (ds_enable) {

        // RJMP -1 === HALT
        if (program[pc] != 0xCFFF) {

            ds_info(pc);
            printf("[%05X] %04X %s\n", 2*pc, tstates, ds_line);
        }
    }
}

// Чтение из памяти
uint8_t AVR::get(uint16_t addr)
{
    uint8_t dv = sram[addr];

    switch (addr) {

        case 0x20: return cursor & 0xFF;
        case 0x21: return cursor >> 8;
        case 0x22: return millis & 255;
        case 0x2C: return border_color;
        case 0x2E: return vconfig;
        case 0x2F: return 1;
    }

    return dv;
}

// Сохранение в память
void AVR::put(uint16_t addr, uint8_t value)
{
    sram[addr] = value;

    switch (addr)
    {
        case 0x20: cursor = (cursor & 0xFF00) | (value); break;
        case 0x21: cursor = (cursor & 0x00FF) | (value << 8); break;
        case 0x22:

            // Количество цветов всего 16
            video[cursor] = value & 15;

            // Параметры инркемента
            if (vconfig & 4) cursor += 0x0001;
            if (vconfig & 8) cursor += 0x0100;

            break;

        case 0x2D: border_color = value & 15; break;
        case 0x2E: vconfig      = value; break;

        // Запись во флаги
        case 0x5F: byte_to_flag(value);
    }
}

void AVR::update_screen()
{
    switch (device)
    {
        // Текстовый видеорежим
        // Расположение видеопамяти B000 (4kb VRAM) C000 (4k FONT)
        case CYCLONE_IV:

            flash_cnt = (flash_cnt + 1) % 15;
            if (flash_cnt == 0) flash = !flash;

            for (int i = 0; i < 25; i++)
            for (int j = 0; j < 80; j++) {

                int     a = 0xB000 + 2*j + 160*i;
                uint8_t b = sram[a],
                        c = sram[a + 1];

                for (int y = 0; y < 16; y++) {

                    int d = sram[0xC000 + 16*b + y];
                    for (int x = 0; x < 8; x++) {

                        int mask = d & (0x80 >> x);
                        int cl = mask || (cursor_x == j && cursor_y == i && flash && y >= 14) ? c & 15 : c >> 4;

                        pset(j*8 + x, i*16 + y, dac[cl]);
                    }
                }
            }

            break;

        default:

            for (int y = 0; y < height; y++)
            for (int x = 0; x < 320; x++) {

                if (x >= 32 && x < 32+256 && y >= 4 && y < 196)
                     pset(x, y, dac[ video[(x-32) + (y-4)*256] ]);
                else pset(x, y, dac[ border_color ]);
            }

            break;
    }
}

// Убрать окно из памяти
int AVR::destroy()
{
    free(screen_buffer);
    SDL_DestroyTexture(sdl_screen_texture);
    SDL_FreeFormat(sdl_pixel_format);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
    return 0;
}

// Установка точки
void AVR::pset(int x, int y, Uint32 cl)
{
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    screen_buffer[width*y + x] = cl;
}
