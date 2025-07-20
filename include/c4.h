#include "zxfont.h"

// Хранение данных в памяти
// =============================================================================

class C4
{
public:

    byte _color, _locx, _locy;

} c4;

// Реализация методов
// =============================================================================

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

// Вывести символ на экране
void pchar(byte x, byte y, char a)
{
    heapvm;
    int hl = (y << 8) + x;
    int de = 8*(a - 0x20);
    for (int i = 0; i < 8; i++) {
        vm[hl] = LPM(zxfont[de + i]);
        hl += 0x20;
    }
    vm[0x1800 + y*32 + x] = c4._color;
}

// Вывод символа в режиме телетайпа
void tchar(char a)
{
    pchar(c4._locx, c4._locy, a);
    c4._locx++;
    if (c4._locx >= 32) { c4._locx = 0; c4._locy++; }
}

// Пропечать строки
void pstr(const unsigned char* s)
{
    char m;
    int  i = 0;
    while ((m = LPM(*s + i))) { tchar(m); i++; }
}

// Ждать нажатия кнопки и читать ASCII
byte getch()
{
    while((inp(2) & 1) == 0);
    return inp(0);
}

// Очистить экран и установить цвет борделя
void cls(byte cl = 0x07)
{
    heapvm;
    border(cl >> 4);
    loc(0, 0);
    color(cl);

    for (int i = 0; i < 6144; i++) vm[i] = 0;
    for (int i = 0; i < 768;  i++) vm[i+6144] = cl;
}
