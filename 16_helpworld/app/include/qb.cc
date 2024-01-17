/**
 * Некоторый набор классов для процедурной работы
 **/

#define CURSOR_X 1
#define CURSOR_Y 2

byte cursor_x, cursor_y, cursor_attr;

// Установка курсорума
void locate(byte x, byte y) {

    out(CURSOR_X, x);
    out(CURSOR_Y, y);
    cursor_x = x;
    cursor_y = y;
}

// Цвет
void attr(byte x) { cursor_attr = x; }

// Очистить весь экран в определенный цвет
void cls(byte color) {

    heapvm;
    for (int i = 0; i < 4000; i += 2) {
        vm[i  ] = 0x00;
        vm[i+1] = color;
    }
    locate(0, 0);
    cursor_attr = color;
}

// Вывод символа на экране
void prn(char ch) {

    heapvm;
    int A = 2*cursor_x + 160*cursor_y;

    // Выводить на экран ВСЁ, кроме Enter
    if (ch != 0x0A) {

        vm[A]   = ch;
        vm[A+1] = cursor_attr;
    }

    cursor_x++;
    if (cursor_x == 80 || ch == 0x0A) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y == 25) {
            cursor_y = 24;
            for (int i = 0; i < 4000-160; i++) vm[i] = vm[i + 160];
            for (int i = 0; i < 160; i += 2) {
                vm[24*160+i]   = 0x00;
                vm[24*160+i+1] = cursor_attr;
            }
        }
    }

    locate(cursor_x, cursor_y);
}

// Отпечатать строку UTF8 на экране
void print(const char* s, byte pgm = 0) {

    byte ch, ct;
    int  i = 0, n = 0;
    while ((ch = (pgm ? pgm_read_byte(&s[i]) : s[i]))) {

        i++;
        ct = 0;

        // Считывание дополнительного байта UTF8 или CTL
        if (ch == 0xD0 || ch == 0xD1 || ch == 0x1B) {
            ch = pgm ? pgm_read_byte(&s[i]) : s[i];
            ct = 1;
            i++;
        }

        // Разбор дополнительного байта
        if      (ch == 0xD0) ch = (ch == 0x81 ? 0xF0 : ch - 0x10);
        else if (ch == 0xD1) ch = (ch == 0x91 ? 0xF1 : ch + (ch < 0xB0 ? 0x60 : 0x10));
        else if (ct) { cursor_attr = ch; continue; }

        prn(ch);
        n++;
    }
}
