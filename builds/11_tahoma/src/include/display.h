#ifndef MODULE_DISPLAY
#define MODULE_DISPLAY

// ---------------------------------------------------------------------

struct cursor_type { byte x, y, attr, visible; };
struct cursor_type cursor;

// ---------------------------------------------------------------------

inline void border(byte);
void        cls(byte);
void        pnewline();

// print.h; print8.h
void        locate(byte, byte);
void        clrcursor();
void        pchar(byte x, byte y, byte ch);
void        prn(byte ch);

// ---------------------------------------------------------------------

// Адрес для ZX Spectrum линии
static const word height[192] PROGMEM = {

    0x0000, 0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700,
    0x0020, 0x0120, 0x0220, 0x0320, 0x0420, 0x0520, 0x0620, 0x0720,
    0x0040, 0x0140, 0x0240, 0x0340, 0x0440, 0x0540, 0x0640, 0x0740,
    0x0060, 0x0160, 0x0260, 0x0360, 0x0460, 0x0560, 0x0660, 0x0760,
    0x0080, 0x0180, 0x0280, 0x0380, 0x0480, 0x0580, 0x0680, 0x0780,
    0x00a0, 0x01a0, 0x02a0, 0x03a0, 0x04a0, 0x05a0, 0x06a0, 0x07a0,
    0x00c0, 0x01c0, 0x02c0, 0x03c0, 0x04c0, 0x05c0, 0x06c0, 0x07c0,
    0x00e0, 0x01e0, 0x02e0, 0x03e0, 0x04e0, 0x05e0, 0x06e0, 0x07e0,
    0x0800, 0x0900, 0x0a00, 0x0b00, 0x0c00, 0x0d00, 0x0e00, 0x0f00,
    0x0820, 0x0920, 0x0a20, 0x0b20, 0x0c20, 0x0d20, 0x0e20, 0x0f20,
    0x0840, 0x0940, 0x0a40, 0x0b40, 0x0c40, 0x0d40, 0x0e40, 0x0f40,
    0x0860, 0x0960, 0x0a60, 0x0b60, 0x0c60, 0x0d60, 0x0e60, 0x0f60,
    0x0880, 0x0980, 0x0a80, 0x0b80, 0x0c80, 0x0d80, 0x0e80, 0x0f80,
    0x08a0, 0x09a0, 0x0aa0, 0x0ba0, 0x0ca0, 0x0da0, 0x0ea0, 0x0fa0,
    0x08c0, 0x09c0, 0x0ac0, 0x0bc0, 0x0cc0, 0x0dc0, 0x0ec0, 0x0fc0,
    0x08e0, 0x09e0, 0x0ae0, 0x0be0, 0x0ce0, 0x0de0, 0x0ee0, 0x0fe0,
    0x1000, 0x1100, 0x1200, 0x1300, 0x1400, 0x1500, 0x1600, 0x1700,
    0x1020, 0x1120, 0x1220, 0x1320, 0x1420, 0x1520, 0x1620, 0x1720,
    0x1040, 0x1140, 0x1240, 0x1340, 0x1440, 0x1540, 0x1640, 0x1740,
    0x1060, 0x1160, 0x1260, 0x1360, 0x1460, 0x1560, 0x1660, 0x1760,
    0x1080, 0x1180, 0x1280, 0x1380, 0x1480, 0x1580, 0x1680, 0x1780,
    0x10a0, 0x11a0, 0x12a0, 0x13a0, 0x14a0, 0x15a0, 0x16a0, 0x17a0,
    0x10c0, 0x11c0, 0x12c0, 0x13c0, 0x14c0, 0x15c0, 0x16c0, 0x17c0,
    0x10e0, 0x11e0, 0x12e0, 0x13e0, 0x14e0, 0x15e0, 0x16e0, 0x17e0
};

// ---------------------------------------------------------------------

// Выдача бордера на экран
inline void border(byte attr) { outp(BORDER, attr); }

// Очистка экрана от скверны
void cls(byte attr = 0x07) {

    heapvm;

    for (int i = 0; i < 6144; i++) vm[i] = 0x00;
    for (int i = 0; i <  768; i++) vm[i + 0x1800] = attr;

    border(attr >> 3);

    cursor.x = 0;
    cursor.y = 0;
    cursor.attr = attr;
    cursor.visible = 0;
}

// Установка атрибутов
void setattr(byte x, byte y, byte attr) {

    heapvm;
    vm[y*32 + x + 0x1800] = attr;
}

// Рисовать блок атрибутов
void battr(byte x1, byte y1, byte x2, byte y2, byte attr) {

    heapvm;
    for (byte y = y1; y <= y2; y++) {

        word a = 0x1800 + 32*(word)y + (word)x1;
        for (byte x = x1; x <= x2; x++) vm[a++] = attr;
    }
}

// Скроллинг строки вверх
void pscrollup() {

    heapvm;

    // Сдвиг трех банков памяти вверх
    for (int m = 0; m < 6144; m += 2048)
    for (int i = 0; i < 256; i++) {

        int k1 = i + m;
        int k2 = k1 + 32;

        // Последняя строка переводится вперед
        if (i >= 224) k2 += (2048-224-32);

        for (int j = 0; j < 8; j++) {

            vm[k1] = (m == 4096 && i >= 224) ? 0 : vm[k2];
            k1 += 0x100;
            k2 += 0x100;
        }
    }

    // Скроллинг атрибутов
    for (int i = 0; i < 768; i++) {
        vm[0x1800 + i] = i < (768-32) ? vm[0x1820 + i] : cursor.attr;
    }
}

void fore(int x) { cursor.attr = (cursor.attr & 0xF8) | x; }
void back(int x) { cursor.attr = (cursor.attr & 0xC7) | (x << 3); }

// Переход на новую линию
void pnewline() {

    cursor.x = 0;
    cursor.y++;

    // Сдвиг вверх
    if (cursor.y == 24) {

        pscrollup();
        cursor.y = 23;
    }
}

// Позиция курсора
word gpos(byte x, byte y) { return LPW(height[y]) + (x >> 3); }

// Либо поставит точку, либо удалить
// Этот способ довольно медленный
void pset(byte x, byte y, byte cl) {

    heapvm;

    // Вычисление положения точки
    word A = gpos(x, y);
    byte M = 1 << ((7 ^ x) & 7);

    // Установка или очистка точки
    if      (cl == 0) vm[A] &= ~M;
    else if (cl == 2) vm[A] ^=  M;
    else              vm[A] |=  M;
}

// Рисовать линию на экране конечно же
void line(int x1, int y1, int x2, int y2, byte cl) {

    // Инициализация смещений
    char signx  = x1 < x2 ? 1 : -1;
    char signy  = y1 < y2 ? 1 : -1;
    int  deltax = x2 > x1 ? x2 - x1 : x1 - x2;
    int  deltay = y2 > y1 ? y2 - y1 : y1 - y2;
    int  error  = deltax - deltay;
    int  error2;

    // Если линия - это точка
    pset(x2, y2, cl);

    // Перебирать до конца
    while ((x1 != x2) || (y1 != y2)) {

        pset(x1, y1, cl);

        error2 = 2 * error;
        if (error2 > -deltay) { error -= deltay; x1 += signx; } // X
        if (error2 <  deltax) { error += deltax; y1 += signy; } // Y
    }
}

// Ускоренное рисование блока по сравнению с линией
void block(byte x1, byte y1, byte x2, byte y2, byte cl) {

    heapvm;

    word at;
    byte x1l = x1 & 7;
    byte x2l = x2 & 7;
    byte x1h = x1 >> 3;
    byte x2h = x2 >> 3;

    byte m0 = 0x00FF >> x1l;
    byte m1 = 0xFF80 >> x2l;

    // Находится на одной позиции
    if (x1h == x2h) {

        for (int y = y1; y <= y2; y++) {

            at = LPW(height[y]) + x1h;
            if (cl) vm[at] |= (m0 & m1); else vm[at] &= ~(m0 & m1);
        }
    }
    else {

        for (int y = y1; y <= y2; y++) {

            at = LPW(height[y]) + x1h;

            if (cl) vm[at++] |= (m0); else vm[at++] &= ~(m0);
            for (int x = x1h + 1; x < x2h; x++) vm[at++] = cl ? 255 : 0;
            if (cl) vm[at]   |= (m1); else vm[at++] &= ~(m1);
        }
    }
}

// Рисование контурной линии
void lineb(int x1, int y1, int x2, int y2, byte cl) {

    block(x1, y1, x2, y1, cl);
    block(x1, y1, x1, y2, cl);
    block(x2, y1, x2, y2, cl);
    block(x1, y2, x2, y2, cl);
}

#endif
