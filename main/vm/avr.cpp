AVR::AVR(int argc, char** argv)
{
    x   = 0;
    y   = 0;
    _hs = 1;
    _vs = 0;

    scale        = 2;               // Удвоение пикселей
    width        = 640;             // Ширина экрана
    height       = 400;             // Высота экрана
    frame_length = (1000/60);       // 60 FPS

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
            }

        } else {

            fp = fopen(argv[i], "rb");
            if (fp) {
                fread(program, 2, 65536, fp); // Ты кот?
                fclose(fp); // А если найду?
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

        int cc = 0, ips = 0, target = (25000000/60);
        Uint32 ticks = SDL_GetTicks();

        // Прием событий
        while (SDL_PollEvent(& evt)) {

            // Событие выхода
            switch (evt.type) {

                case SDL_QUIT: destroy(); return 0;
            }
        }

        // Выполнение инструкции 25 Мгц / 60 кадров
        while (cc < target) {

            disassemble();
            cc += step(); ips++;
        }

        tstates += cc;

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
            printf("%05X %s\n", 2*pc, ds_line);
        }
    }
}

// Чтение из памяти
uint8_t AVR::get(uint16_t addr)
{
    uint8_t dv = sram[addr];

    switch (addr) {

        // Читать из памяти
        case 0x22: dv = video[video_addr++]; break;
    }

    return dv;
}

// Сохранение в память
void AVR::put(uint16_t addr, uint8_t value)
{
    sram[addr] = value;

    switch (addr) {

        case 0x20: video_mode = value; break;

        // 16 битное адрес, сначала пишется младший, потом старший байт
        case 0x21: cursor_y = cursor_x; cursor_x = value; break;

        // Запись байта в видеопамять
        case 0x22: video[video_addr++] = value; break;

        // Позиционирование
        case 0x2C: loc_x = (loc_x & 0xFF00) | value;     recalc_video_address(); break;
        case 0x2D: loc_x = (loc_x & 0x00FF) | value*256; recalc_video_address(); break;
        case 0x2E: loc_y = (loc_y & 0xFF00) | value;     recalc_video_address(); break;
        case 0x2F: loc_y = (loc_y & 0x00FF) | value*256; recalc_video_address(); break;
    }

    // Запись во флаги
    if (addr == 0x5F) byte_to_flag(value);
}

// На основе locx, locy рассчитать новый video_addr
void AVR::recalc_video_address()
{
    switch (video_mode)
    {
        case 0: video_addr = 160*loc_y + 2*loc_x; break;
        case 1:
        case 2: video_addr = 320*loc_y + loc_x; break;
        case 3:
        case 4: video_addr = 640*loc_y + loc_x; break;
    }
}

void AVR::update_screen()
{
    switch (video_mode)
    {
        // 80x25
        case 0:

            for (int y = 0; y < 25; y++)
            for (int x = 0; x < 80; x++) {

                int a = video[2*x + y*160    ];
                int b = video[2*x + y*160 + 1];

                for (int i = 0; i < 16; i++) {

                    int c = charmap[a*16 + i];
                    for (int j = 0; j < 8; j++) {
                        pset(x*8 + j, y*16 + i, c & (0x80 >> j) ? dac[b & 15] : dac[b >> 4]);
                    }
                }
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
