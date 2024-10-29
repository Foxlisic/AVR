#include <app.h>
#include <stdio.h>

// Обработчик кадра
uint DisplayTimer(uint interval, void *param) {

    SDL_Event     event;
    SDL_UserEvent userevent;

    /* Создать новый Event */
    userevent.type  = SDL_USEREVENT;
    userevent.code  = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return (interval);
}

// Инициализация и загрузка кода в память
void App::init(int argc, char** argv) {

    char ts[256];

    if (argc < 2) {

        printf("\nНеобходим параметр, для указания BIN-файла:\n");
        printf("./avr.out program/hello/main.bin\n\n");
        exit(0);
    }
    // Инициализировать нулями
    else {

        unsigned char* prog = avrcpu->getprog();
        for (int i = 0; i < 128*1024; i++) prog[i] = 0;
        return;
    }

    FILE* fp;

    // Попытка взять файл main.bin
    sprintf(ts, "%s/main.bin", argv[1]);
    fp = fopen(ts, "rb");

    if (!fp) {
        fp = fopen(argv[1], "rb");
    }

    if (fp) {

        unsigned char* prog = avrcpu->getprog();

        fseek(fp, 0, SEEK_END);
        int size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        fread(prog, 1, size, fp);
        fclose(fp);

    } else {

        printf("Указанный файл не был найден\n");
        exit(1);
    }
}

App::App(int w, int h, const char* caption) {

    width  = w;
    height = h;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_EnableUNICODE(1);

    sdl_screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption(caption, 0);
    SDL_AddTimer(16, DisplayTimer, NULL);

    avrcpu = new AVRCPU();
    cpu_start = 0;
}

// Получение XT-кода
int App::get_key(SDL_Event event) {

    /* Получение ссылки на структуру с данными о нажатой клавише */
    SDL_KeyboardEvent * eventkey = & event.key;

    int xt = 0;
    int k = eventkey->keysym.scancode;

    switch (k) {

        /* A */ case 0x26: xt = 0x1E; break;
        /* B */ case 0x38: xt = 0x30; break;
        /* C */ case 0x36: xt = 0x2E; break;
        /* D */ case 0x28: xt = 0x20; break;
        /* E */ case 0x1a: xt = 0x12; break;
        /* F */ case 0x29: xt = 0x21; break;
        /* G */ case 0x2a: xt = 0x22; break;
        /* H */ case 0x2b: xt = 0x23; break;
        /* I */ case 0x1f: xt = 0x17; break;
        /* J */ case 0x2c: xt = 0x24; break;
        /* K */ case 0x2d: xt = 0x25; break;
        /* L */ case 0x2e: xt = 0x26; break;
        /* M */ case 0x3a: xt = 0x32; break;
        /* N */ case 0x39: xt = 0x31; break;
        /* O */ case 0x20: xt = 0x18; break;
        /* P */ case 0x21: xt = 0x19; break;
        /* Q */ case 0x18: xt = 0x10; break;
        /* R */ case 0x1b: xt = 0x13; break;
        /* S */ case 0x27: xt = 0x1F; break;
        /* T */ case 0x1c: xt = 0x14; break;
        /* U */ case 0x1e: xt = 0x16; break;
        /* V */ case 0x37: xt = 0x2F; break;
        /* W */ case 0x19: xt = 0x11; break;
        /* X */ case 0x35: xt = 0x2D; break;
        /* Y */ case 0x1d: xt = 0x15; break;
        /* Z */ case 0x34: xt = 0x2C; break;

        /* 0 */ case 0x13: xt = 0x0B; break;
        /* 1 */ case 0x0A: xt = 0x02; break;
        /* 2 */ case 0x0B: xt = 0x03; break;
        /* 3 */ case 0x0C: xt = 0x04; break;
        /* 4 */ case 0x0D: xt = 0x05; break;
        /* 5 */ case 0x0E: xt = 0x06; break;
        /* 6 */ case 0x0F: xt = 0x07; break;
        /* 7 */ case 0x10: xt = 0x08; break;
        /* 8 */ case 0x11: xt = 0x09; break;
        /* 9 */ case 0x12: xt = 0x0A; break;

        /* ~ */ case 0x31: xt = 0x29; break;
        /* - */ case 0x14: xt = 0x0C; break;
        /* = */ case 0x15: xt = 0x0D; break;
        /* \ */ case 0x33: xt = 0x2B; break;
        /* [ */ case 0x22: xt = 0x1A; break;
        /* ] */ case 0x23: xt = 0x1B; break;
        /* ; */ case 0x2f: xt = 0x27; break;
        /* ' */ case 0x30: xt = 0x28; break;
        /* , */ case 0x3b: xt = 0x33; break;
        /* . */ case 0x3c: xt = 0x34; break;
        /* / */ case 0x3d: xt = 0x35; break;

        /* bs */ case 0x16: xt = 0x0E; break;
        /* sp */ case 0x41: xt = 0x39; break;
        /* tb */ case 0x17: xt = 0x0F; break;
        /* ls */ case 0x32: xt = 0x2A; break;
        /* lc */ case 0x25: xt = 0x1D; break;
        /* la */ case 0x40: xt = 0x38; break;
        /* en */ case 0x24: xt = 0x1C; break;
        /* es */ case 0x09: xt = 0x01; break;
    }

    /* Получить скан-код клавиш */
    return xt;
}

// Бесконечный цикл
void App::start() {

    int k;

    while (1) {

        while (SDL_PollEvent(& event)) {

            switch (event.type) {

                // Если нажато на крестик, то приложение будет закрыто
                case SDL_QUIT:
                    return;

                // Нажата какая-то клавиша
                case SDL_KEYDOWN:

                    avrcpu->key_press(get_key(event));
                    break;

                // Отпущена клавиша
                case SDL_KEYUP:

                    avrcpu->key_up(get_key(event));
                    break;

                // Вызывается по таймеру
                case SDL_USEREVENT:

                    // Выполнять что-то, если только запущен CPU
                    if (cpu_start) {

                        avrcpu->timer_tick();

                        if (avrcpu->need_update()) {
                            update();
                            flip();
                        }
                    }

                    break;
            }
        }

        avrcpu->frame();
        SDL_Delay(1);
    }
}

// Обновить весь экран
void App::update() {

    unsigned char* sram = avrcpu->getsram();

    int k = 0;
    for (int y = 0; y < 720; y++) {
        for (int x = 0; x < 768; x++) {

            int mx = x / 3, my = y / 3;
            int kx = mx & 3;
            int cb = sram[0xC000 + ((my*256 + mx) >> 2)];
            int cl;

            // 11.00.00.00 mx=0 >> 6 ==> 6 - 2*0 = 6
            // 00.11.00.00 mx=1 >> 4 ==> 6 - 2*1 = 4
            // 00.00.11.00 mx=2 >> 2 ==> 6 - 2*2 = 2
            // 00.00.00.11 mx=3 >> 0 ==> 6 - 2*3 = 0

            switch ((cb >> (6 - 2*kx)) & 3) {

                case 0: cl = 0x000000; break;
                case 1: cl = 0xC00000; break;
                case 2: cl = 0x00C000; break;
                case 3: cl = 0x0060FF; break;
            }

            ( (Uint32*)sdl_screen->pixels )[ width*y + x ] = cl;
        }
    }
}

// Нарисовать точку
void App::pset(int x, int y, uint color) {

    if (x >= 0 && y >= 0 && x < width && y < height) {
        ( (Uint32*)sdl_screen->pixels )[ x + width*y ] = color;
    }
}

// Обменять буфер
void App::flip() {
    SDL_Flip(sdl_screen);
}
