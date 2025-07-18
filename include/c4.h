#include "zxfont.h"

class C4
{
public:

    byte _color, _locx, _locy;

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

    // Цвет бордера
    inline void border(byte c)
    {
        outp(0x00, c);
    }

    // Цветовая гамма
    inline void color(byte c)
    {
        _color = c;
    }

    // Установка курсора для печати
    inline void loc(byte x, byte y)
    {
        _locx = x;
        _locy = y;
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
        vm[0x1800 + y*32 + x] = _color;
    }

    // Вывод символа в режиме телетайпа
    void tchar(char a)
    {
        pchar(_locx, _locy, a);
        _locx++;
        if (_locx >= 32) { _locx = 0; _locy++; }
    }

    // Пропечать строки
    void pstr(const unsigned char* s)
    {
        char m;
        int  i = 0;
        while ((m = LPM(*s + i))) { tchar(m); i++; }
    }

} c4;
