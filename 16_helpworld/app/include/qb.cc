/**
 * Некоторый набор классов для процедурной работы
 **/

#define CURSOR_X 1
#define CURSOR_Y 2

byte cursor_x, cursor_y, cursor_attr, caret_at = 0;
byte kb_data[8];
byte buffer[32];
volatile byte kb_top = 0;

// Псевдослучайное число через https://ru.wikipedia.org/wiki/Линейный_конгруэнтный_метод
unsigned long krandom = 0;
unsigned int rand() {

    krandom = ((krandom * 1664525) + 1013904223);
    return krandom >> 16;
}

// Прием следуюшего символа из IRQ
void kb_irq_handler() {

    if (kb_top >= 7) return;
    kb_data[kb_top++] = inp(0);
}

// Прием данных с клавиатуры
byte inkey() {

    byte ch = 0;
    if (kb_top) {
        ch = kb_data[0];
        kb_top--;
        for (int i = 0; i < kb_top; i++)
            kb_data[i] = kb_data[i+1];
    }

    return ch;
}

// Ждать нажатия
byte getch() {

    byte ch;
    do { ch = inkey(); } while (ch == 0);
    return ch;
}

// Установка курсорума
void locate(byte x, byte y) {

    out(CURSOR_X, x);
    out(CURSOR_Y, y);
    cursor_x = x;
    cursor_y = y;
}

// Скрыть курсор
void hide() { locate(0, 25); }

// Цвет
void attr(byte x) { cursor_attr = x; }

// Очистить весь экран в определенный цвет
void cls(byte color = 0x07) {

    heapvm;
    for (int i = 0; i < 4000; i += 2) {
        vm[i  ] = 0x00;
        vm[i+1] = color;
    }
    locate(0, 0);
    cursor_attr = color;
    kb_top = 0;
    caret_at = 0;
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
        cursor_x = caret_at;
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

// Уложить окрашенный символ
void putx(byte x, byte y, byte ch, byte at = 255) {

    if (x >= 80 || y >= 25) return;

    heapvm;
    vm[2*x + y*160 + 0] = ch;

    if (at < 255)
    vm[2*x + y*160 + 1] = at;
}

// Нарисовать блок
void block(byte x, byte y, byte w, byte h, byte ch, byte at) {

    for (int i = y; i < y + h; i++)
    for (int j = x; j < x + w; j++)
        putx(j, i, ch, at);
}

// Нарисовать outlined блок
void outline(byte x, byte y, byte w, byte h, byte fill = 255) {

    if (fill < 255) block(x, y, w, h, ' ', fill);

    w--; h--;

    for (int j = x+1; j < x + w; j++) {
        putx(j, y,   0xC4, cursor_attr);
        putx(j, y+h, 0xC4, cursor_attr);
    }

    for (int i = y+1; i < y + h; i++) {
        putx(x,   i, 0xB3, cursor_attr);
        putx(x+w, i, 0xB3, cursor_attr);
    }

    putx(x,   y,   0xDA, cursor_attr);
    putx(x+w, y,   0xBF, cursor_attr);
    putx(x,   y+h, 0xC0, cursor_attr);
    putx(x+w, y+h, 0xD9, cursor_attr);
}

// Печать числа -2147483647 .. 2147483647
byte i2a(long v) {

    char s[24];
    int  q, i = 0, cnt = 0, cs = 0;

    // Печать символа минус перед числом
    if (v < 0) { v = -v; buffer[cs++] = '-'; cnt = 1; }

    // Вычисление смещения
    do { q = v % 10; v /= 10; s[i++] = '0' + q; } while (v); i--;

    // Вывести число
    for (char k = 0; k <= i; k++) { buffer[cs++] = s[i-k]; cnt++; }

    // В конце ZTerm
    buffer[cs] = 0;

    // Занимаемый размер символов
    return cnt;
}

// Отпечатать строку UTF8 на экране
void print(const char* s, byte pgm = 0) {

    byte ch, nx, ct;
    int  i = 0, n = 0;
    while ((ch = (pgm ? pgm_read_byte(&s[i++]) : s[i++]))) {

        ct = (ch == 0x1B);

        // Считывание дополнительного байта UTF8 или CTL
        if (ch == 0xD0 || ch == 0xD1 || ch == 0x1B) {

            // Прочитать следующий символ
            nx = (pgm ? pgm_read_byte(&s[i++]) : s[i++]);

            // Разбор дополнительного байта
            if      (ch == 0xD0) { ch = (nx == 0x81 ? 0xF0 : nx - 0x10); }
            else if (ch == 0xD1) { ch = (nx == 0x91 ? 0xF1 : nx + (nx < 0xB0 ? 0x60 : 0x10)); }
            else if (ct) { cursor_attr = nx; continue; }
        }

        prn(ch);
        n++;
    }
}

void printi(word x) {

    i2a(x);
    print((const char*)buffer);
}
