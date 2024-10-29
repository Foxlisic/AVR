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
        while (cc < target) { ips++; cc += step(); }

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
