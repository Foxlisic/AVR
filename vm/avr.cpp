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
    border_color = 0;
    cursor_x     = 0;
    cursor_y     = 0;
    vconfig      = 1;
    device       = CYCLONE_V;
    video        = (uint8_t*) malloc(640*400);

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

                // Циклон-4: Только 80 на 25 строк
                case '4':

                    device = CYCLONE_IV;
                    width  = 640;
                    height = 400;
                    scale  = 2;

                    for (int i = 0; i < 4096; i++) sram[0xC000 + i] = charmap[i];
                    break;
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

    for (;;)
    {
        int ts, ms = 0, cc = 0, ips = 0, target = (25000000 / 60);
        Uint32 ticks = SDL_GetTicks();

        // Прием событий
        while (SDL_PollEvent(& evt)) {

            // Событие выхода
            switch (evt.type) {

                case SDL_QUIT:      destroy(); return 0;
                case SDL_KEYDOWN:   key_code = keyboard_ascii(evt, 1); key_press = !!key_code; break;
                case SDL_KEYUP:     keyboard_ascii(evt, 0); break;
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

    switch (addr)
    {
        case 0x20: return 0;
        case 0x21: return vconfig;
        case 0x22: return border_color;
        case 0x2C: return cursor_x;
        case 0x2D: return cursor_x >> 8;
        case 0x2E: return cursor_y;
        case 0x2F: return cursor_y >> 8;
        case 0x30: return key_code;
        case 0x31: dv = key_press; key_press = 0; return dv;
        case 0x32: return millis & 255;
    }

    return dv;
}

// Сохранение в память
void AVR::put(uint16_t addr, uint8_t value)
{
    sram[addr] = value;

    switch (addr)
    {
        // Рисовать точку на экране
        case 0x20:

            // Количество цветов всего 16
            video[cursor_x + cursor_y*640] = value & 15;

            // Параметры инкремента
            cursor_x += (vconfig & 1);

            if (cursor_x >= 640) {
                cursor_x = 0;
                cursor_y++;
            }

            cursor_y = (cursor_y + (vconfig & 2 ? 1 : 0)) % 400;

            break;

        case 0x21: border_color = value & 15; break;
        case 0x22: vconfig      = value; break;

        // Указатель в память точки рисования
        case 0x2C: cursor_x = (cursor_x & 0xFF00) |  value; break;
        case 0x2D: cursor_x = (cursor_x & 0x00FF) | (value << 8); break;
        case 0x2E: cursor_y = (cursor_y & 0xFF00) |  value; break;
        case 0x2F: cursor_y = (cursor_y & 0x00FF) | (value << 8); break;

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
        case CYCLONE_IV: {

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
        }

        default: {

            for (int y = 0; y < 400; y++)
            for (int x = 0; x < 640; x++) {
                pset(x, y, dac[video[x + y*640]]);
            }

            break;
        }
    }
}

// Убрать окно из памяти
int AVR::destroy()
{
    free(screen_buffer);
    free(video);

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

int AVR::keyboard_ascii(SDL_Event event, int press)
{
    // Получение ссылки на структуру с данными о нажатой клавише */
    SDL_KeyboardEvent * eventkey = & event.key;

    int xt = 0;
    int sh = key_shift;
    int k  = eventkey->keysym.sym;

    switch (k)
    {
        /* A */   case SDLK_a: return sh ? 'A' : 'a';
        /* B */   case SDLK_b: return sh ? 'B' : 'b';
        /* C */   case SDLK_c: return sh ? 'C' : 'c';
        /* D */   case SDLK_d: return sh ? 'D' : 'd';
        /* E */   case SDLK_e: return sh ? 'E' : 'e';
        /* F */   case SDLK_f: return sh ? 'F' : 'f';
        /* G */   case SDLK_g: return sh ? 'G' : 'g';
        /* H */   case SDLK_h: return sh ? 'H' : 'h';
        /* I */   case SDLK_i: return sh ? 'I' : 'i';
        /* J */   case SDLK_j: return sh ? 'J' : 'j';
        /* K */   case SDLK_k: return sh ? 'K' : 'k';
        /* L */   case SDLK_l: return sh ? 'L' : 'l';
        /* M */   case SDLK_m: return sh ? 'M' : 'm';
        /* N */   case SDLK_n: return sh ? 'N' : 'n';
        /* O */   case SDLK_o: return sh ? 'O' : 'o';
        /* P */   case SDLK_p: return sh ? 'P' : 'p';
        /* Q */   case SDLK_q: return sh ? 'Q' : 'q';
        /* R */   case SDLK_r: return sh ? 'R' : 'r';
        /* S */   case SDLK_s: return sh ? 'S' : 's';
        /* T */   case SDLK_t: return sh ? 'T' : 't';
        /* U */   case SDLK_u: return sh ? 'U' : 'u';
        /* V */   case SDLK_v: return sh ? 'V' : 'v';
        /* W */   case SDLK_w: return sh ? 'W' : 'w';
        /* X */   case SDLK_x: return sh ? 'X' : 'x';
        /* Y */   case SDLK_y: return sh ? 'Y' : 'y';
        /* Z */   case SDLK_z: return sh ? 'Z' : 'z';
        /* 0 */   case SDLK_0: return sh ? ')' : '0';
        /* 1 */   case SDLK_1: return sh ? '!' : '1';
        /* 2 */   case SDLK_2: return sh ? '@' : '2';
        /* 3 */   case SDLK_3: return sh ? '#' : '3';
        /* 4 */   case SDLK_4: return sh ? '$' : '4';
        /* 5 */   case SDLK_5: return sh ? '%' : '5';
        /* 6 */   case SDLK_6: return sh ? '^' : '6';
        /* 7 */   case SDLK_7: return sh ? '&' : '7';
        /* 8 */   case SDLK_8: return sh ? '*' : '8';
        /* 9 */   case SDLK_9: return sh ? '(' : '9';

        // Клавиши отдельные
        /* ` */    case SDLK_BACKQUOTE:     return sh ? '~' : '`';
        /* - */    case SDLK_MINUS:         return sh ? '_' : '-';
        /* = */    case SDLK_EQUALS:        return sh ? '+' : '=';
        /* \ */    case SDLK_BACKSLASH:     return sh ? '|' : '\\';
        /* [ */    case SDLK_LEFTBRACKET:   return sh ? '{' : '[';
        /* ] */    case SDLK_RIGHTBRACKET:  return sh ? '}' : ']';
        /* ; */    case SDLK_SEMICOLON:     return sh ? ':' : ';';
        /* ' */    case SDLK_QUOTE:         return sh ? '|' : '\'';
        /* , */    case SDLK_COMMA:         return sh ? '<' : ',';
        /* . */    case SDLK_PERIOD:        return sh ? '>' : '.';
        /* / */    case SDLK_SLASH:         return sh ? '?' : '/';
        /* bs */   case SDLK_BACKSPACE:     return key_BS;
        /* sp */   case SDLK_SPACE:         return ' ';
        /* tb */   case SDLK_TAB:           return key_TAB;
        /* ls */   case SDLK_LSHIFT:        key_shift = press; return 0;
        /* la */   case SDLK_LALT:          return key_LALT;
        /* lc */   case SDLK_LCTRL:         return key_LCTRL;
        /* en */   case SDLK_RETURN:        return key_ENTER;
        /* es */   case SDLK_ESCAPE:        return key_ESC;
        /* UP  */  case SDLK_UP:            return key_UP;
        /* RT  */  case SDLK_RIGHT:         return key_RIGHT;
        /* DN  */  case SDLK_DOWN:          return key_DOWN;
        /* LF  */  case SDLK_LEFT:          return key_LEFT;
        /* Home */ case SDLK_HOME:          return key_HOME;
        /* End  */ case SDLK_END:           return key_END;
        /* PgUp */ case SDLK_PAGEUP:        return key_PGUP;
        /* PgDn */ case SDLK_PAGEDOWN:      return key_PGDN;
        /* Del  */ case SDLK_DELETE:        return key_DEL;
        /* Ins  */ case SDLK_INSERT:        return key_INS;
        /* F1  */  case SDLK_F1:            return key_F1;
        /* F2  */  case SDLK_F2:            return key_F2;
        /* F3  */  case SDLK_F3:            return key_F3;
        /* F4  */  case SDLK_F4:            return key_F4;
        /* F5  */  case SDLK_F5:            return key_F5;
        /* F6  */  case SDLK_F6:            return key_F6;
        /* F7  */  case SDLK_F7:            return key_F7;
        /* F8  */  case SDLK_F8:            return key_F8;
        /* F9  */  case SDLK_F9:            return key_F9;
        /* F10 */  case SDLK_F10:           return key_F10;
        /* F11 */  case SDLK_F11:           return key_F11;
        /* F12 */  case SDLK_F12:           return key_F12;
    }

    return 0;
}
