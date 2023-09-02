#include "av.h"

// Чтение из памяти
uint8_t AVR::get(uint16_t addr) {

    uint8_t D = 0;
    uint32_t A = addr;

    if (A >= 0xF000) A += bank * 4096;

    switch (A) {

        case 0x20: return bank;
        case 0x21: return videomode;
        case 0x22: return kb_code;
        case 0x2C: return spi_data;
        case 0x2D: return spi_st;
        case 0x2E: return cur_x;
        case 0x2F: return cur_y;
        case 0x30: return intr_mask;

        // Остальная память
        default: return sram[A];
    }

    return 0;
}

// Сохранение в память
void AVR::put(uint16_t addr, uint8_t value) {

    uint32_t A = addr;

    if (A >= 0xF000) A += bank * 4096;

    switch (A) {

        case 0x20: bank = value; break;         // Указатель на банк памяти
        case 0x21: switch_vm(value); break;     // Видеорежим
        case 0x22: eoi = 0; break;              // Разрешение прерывания
        case 0x2C: spi_cmd(value); break;       // SD-карта
        case 0x2D: spi_st &= ~2; break;         // Сброс таймаута
        case 0x2E: cur_x = value; break;        // Курсор X
        case 0x2F: cur_y = value; break;        // Курсор Y
        case 0x30: intr_mask = value; break;    // Маски прерываний
    }

    // Запись в память
    sram[A] = value;

    update_vm_byte(A);

    // Запись во флаги
    if (A == 0x5F) byte_to_flag(value);
}

// Запись в видеопамять
void AVR::update_vm_byte(int A) {

    int t0, t1, t2;

    switch (videomode) {

        // 80x25 Текстовый режим
        case 0:

            if (A >= 0xF000 && A < 0x10000) {

                t0 = (A & 0x0FFF) >> 1;
                t1 = sram[A & ~1]; // Char
                t2 = sram[A |  1]; // Attr

                fore = DOS_13[ t2 & 15 ];
                back = DOS_13[ t2 >> 4 ];

                locate(t0 % 80, t0 / 80);
                pchr(t1);
            }
            break;

        // Видеорежим 320x200
        case 1:

            if (A >= 0xF000 && A < 0x1F000) {

                t0 = (A - 0xF000);
                t1 = t0 % 320;
                t2 = t0 / 320;
                t0 = DOS_13[sram[A]];

                t1 *= 2;
                t2 *= 2;

                pset(t1,   t2,   t0);
                pset(t1+1, t2,   t0);
                pset(t1,   t2+1, t0);
                pset(t1+1, t2+1, t0);
            }
            break;
    }
}

// Обновить или перерисовать экран заново
void AVR::switch_vm(uint8_t new_vm) {

    videomode = new_vm;

    switch (videomode) {

        // 80x25 Текстовый режим
        case 0:

            for (int i = 0xF000; i < 0x10000; i += 2)
                update_vm_byte(i);

            break;

        // 320x200x256
        case 1:

            for (int i = 0xF000; i < 0x1F000; i++)
                update_vm_byte(i);

            break;
    }
}

// Сканирование нажатой клавиши
// https://ru.wikipedia.org/wiki/Скан-код
void AVR::kbd_scancode(int scancode, int release) {

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

// Добавить вызов прерывания клавиатуры в очередь
void AVR::kbd_push(uint8_t code) {

    kb[kb_id] = code;
    kb_id = (kb_id + 1) & 255;
}
