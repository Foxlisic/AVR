#include "zxfont.h"

// В зависимости от страницы будет писать в видео память
#define heapvp heap(vm, (c4._vpage ? 0xA000 : 0x8000))

// Хранение данных в памяти
// =============================================================================

class C4
{
public:

    byte _color, _locx, _locy;
    byte _vpage = 0;

} c4;

// Работа с IO
// =============================================================================

// Установка номера видеостраницы для чтения
inline void vpage(byte c)
{
    outp(0x01, c & 1);
}

// Установить текущую видеостраницу для записи
inline void cpage(byte c)
{
    c4._vpage = c & 1;
}

// Проверить сигнал VBlank
inline byte vblank()
{
    return (inp(3) & 1);
}

// Есть ли нажатие клавиши?
inline byte kbhit()
{
    return (inp(2) & 1);
}

// Цвет бордера
inline void border(byte c)
{
    outp(0x00, c);
}

// Цветовая гамма
inline void color(byte c)
{
    c4._color = c;
}

// Установка курсора для печати
inline void loc(byte x, byte y)
{
    c4._locx = x;
    c4._locy = y;
}

// ASCII-код
inline byte keycode()
{
    return inp(0);
}

// Реализация методов
// =============================================================================

// Установка атрибута
void pattr(byte x, byte y, byte a)
{
    heapvp;
    vm[0x1800 + y*32 + x] = a;
}

// Вывести символ на экране
void pchar(byte x, byte y, char a)
{
    heapvp;
    int hl = (y << 8) + x;
    int de = 8*(a - 0x20);
    for (int i = 0; i < 8; i++) {
        vm[hl] = LPM(zxfont[de + i]);
        hl += 0x20;
    }
    pattr(x,y,c4._color);
}

// Вывод символа в режиме телетайпа
void tchar(char a)
{
    if (a == 10) {
        c4._locx = 32;
    } else {
        pchar(c4._locx, c4._locy, a);
        c4._locx++;
    }

    if (c4._locx >= 32) {
        c4._locx = 0;
        c4._locy++;
        if (c4._locy >= 24) {
            c4._locy = 23;
            // Прокрутить
        }
    }
}

// Пропечать константной строки из PGM
int pstr(const unsigned char* s)
{
    char m;
    int  i = 0;
    while ((m = LPM(*s + i))) { tchar(m); i++; }
    return i;
}

// Печать строки из памяти RAM
int print(const char* s)
{
    int i = 0;
    while (s[i]) tchar(s[i++]);
    return i;
}

// Ждать нажатия кнопки и читать ASCII
byte getch()
{
    while(kbhit() == 0);
    return keycode();
}

// Ждать сигнала VBlank
inline void vsync()
{
    while (vblank() == 0);
}

// Очистить экран и установить цвет борделя
void cls(byte cl = 0x07)
{
    heapvp;

    border(cl >> 4);
    loc(0, 0);
    color(cl);

    for (int i = 0; i < 6144; i++) vm[i] = 0;
    for (int i = 0; i < 768;  i++) vm[i+6144] = cl;
}

// Ввод данных с клавиатуры для изучения приколюх
int input(char* str, int max = 16)
{
    int x = c4._locx,
        y = c4._locy;

    // Инициализация переменных
    byte i = 0, len = 0, blink = 0, bcnt = 255, color = c4._color;

    for (;;) {

        // Курсор меняется каждые 0.25 сек
        if (vblank()) {

            if (++bcnt >= 15) {

                bcnt  = 0;
                blink = 1 - blink;
                pattr(x, y, blink ? 0x70 : color);
            }
        }

        // Нажатие на клавиатуре
        if (kbhit()) {

            byte k = keycode();

            // На всякий случай стереть курсор
            if (blink) { pattr(x, y, color); }

            // Клавиша Enter
            if (k == 10) {
                return len;
            }
            // Нажат backspace
            else if (k == 8) {
                if (len > 0) { // Мы на начале строки
                    pchar(--x, y, 0); str[--i] = 0; len--;
                }
            }
            // Нажата ASCII клавиша
            else if (len < max) {
                pchar(x++, y, k); str[i++] = k; str[i] = 0; len++;
            }

            // Нарисовать курсор на новой позиции
            blink = 1;
            bcnt  = 0;
            pattr(x, y, 0x70);
        }
    }
}

// Распечатать на экране целочисленное значение
int print(int x)
{
    char b[16];

    int a, i = 16;

    if (x < 0) { x = -x; tchar('-'); }

    do {
        a  = x % 10;
        x /= 10;
        b[--i] = '0'+a;
    }
    while (x);

    // Печать числа в обратном порядке
    for (int j = i; j < 16; j++) tchar(b[j]);

    return 16 - i;
}

// Прочесть число из потока данных
int parseInt(char* s)
{
    int a = 0;
    int i = 0;
    int m = 1;
    if (s[i] == '-') { m = -1; i++; }
    while (s[i] >= '0' && s[i] <= '9' && s[i]) {
        a = (10*a) + (s[i++]-'0');
    }
    return a*m;
}

// Рисование линии на экране (256,192)
void line(int x1, int y1, int x2, int y2, byte c)
{
    heapvp;

    int signx  = x1 < x2 ? 1 : -1;
    int signy  = y1 < y2 ? 1 : -1;
    int deltax = x2 > x1 ? x2 - x1 : x1 - x2;
    int deltay = y2 > y1 ? y2 - y1 : y1 - y2;
    int error  = deltax - deltay;
    int error2;

    // Первичное позиционирование
    int ad = (x1 >> 3) + (32*y1);
    byte m = 0x80 >> (x1 & 7);

    // Перебирать до конца
    for (;;) {

        // PSET (x1,y1),c
        if (c) vm[ad] |= m; else vm[ad] &= ~m;

        error2 = 2 * error;

        // Выйти из цикла при достижении конца линии
        if (x1 == x2 && y1 == y2) break;

        // Перемещение точки по X++, X--
        if (error2 > -deltay) {

            x1    += signx;
            error -= deltay;

            if (signx > 0) { if (m == 0x01) { ad++; m = 0x80; } else { m >>= 1; } }
            else           { if (m == 0x80) { ad--; m = 0x01; } else { m <<= 1; } }
        }

        // Перемещение точки по Y++, Y--
        if (error2 < deltax) {

            y1    += signy;
            error += deltax;

            ad = signy > 0 ? ad + 32 : ad - 32;
        }
    }
}
