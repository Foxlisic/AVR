
// ---------------------------------------------------------------------
// ЧТЕНИЕ В ПАМЯТЬ И ПОРТЫ
// ---------------------------------------------------------------------

// Чтение из памяти
uint8_t AVR::get(int addr) {

    uint8_t dv = 0;

    addr &= 0xFFFF;

    // Банкинг памяти
    if (addr >= 0xF000) addr += 4096 * membank;

    switch (addr) {

        case 0x20: return membank;
        case 0x21: kbhit = 0; return keybcode;
        case 0x22: return kbhit;
        case 0x23: return (ticks_frame      ) & 255;
        case 0x24: return (ticks_frame >> 8 ) & 255;
        case 0x25: return (ticks_frame >> 16) & 255;
        case 0x26: return (ticks_frame >> 24) & 255;
        case 0x2C: return cursor_x;
        case 0x2D: return cursor_y;
        case 0x38: return videomode;

        // Остальная память
        default: dv = sram[addr]; break;
    }

    return dv & 0xFF;
}

// ---------------------------------------------------------------------

// Сохранение в память
void AVR::put(int addr, unsigned char value) {

    addr &= 0xFFFF;

    // Учитывать банкинг
    if (addr >= 0xF000) addr += 4096 * membank;

    sram[addr] = value;

    switch (addr) {

        // Системные
        case 0x20: membank = value; break;

        // Установка видеорежима
        case 0x38: videomode = value; update_screen(); break;

        // Положение курсора
        case 0x2C: cursor_x = value; text_cursor = 80*cursor_y + cursor_x; break;
        case 0x2D: cursor_y = value; text_cursor = 80*cursor_y + cursor_x; break;

        // Специальный регистр
        case 0x5F: byte_to_flag(value); break;
    }

    // Запросить перерисовку всего холста при доступе в старшую память
    if (addr >= 0xF000 && addr < 0x11000) {
        need_repaint = 1;
    }
    // Перерисовка графической области
    else if (videomode == 1 && addr >= (0xF000 + 8*4096) && membank < (0xF000 + 24*4096)) {
        need_repaint = 1;
    }
}
