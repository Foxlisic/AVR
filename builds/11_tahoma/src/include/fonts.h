#include <avrio.h>

// Набор шрифтов 4x8, 8x8, 11pt

#include "zxfont4.h"
#include "zxfont8.h"
#include "tahomafont11.h"

class fonts {
protected:

    byte _size, _overlap;
    byte cursor_x,
         cursor_y;

public:

    fonts() { _size = 4; _overlap = 1; locate(0, 0); }

    void size(byte v) { _size = v; }
    void locate(byte x, byte y) { cursor_x = x; cursor_y = y; }
    void overlap(byte v) { _overlap = v; }

    // Печать символа 4x8
    void prn4(byte ch) {

        heapvm;

        ch -= 32;
        byte k = ch >> 1;
        byte h = cursor_x >> 3,
             l = cursor_x & 7;

        for (byte i = 0; i < 8; i++) {

            byte m = LPM(_zxfont4[k][i]);
            word a = LPW(height[cursor_y + i]) + h;

            m = (ch & 1) ? m << 4 : m & 0xF0;

            word m0 = (word) m << (8 - l);
            word m1 = 0xF000 >> l;
                 m1 = ~m1;

            // В данном случае всегда overlap=1
            vm[a] = (vm[a] & (byte)(m1 >> 8)) | (byte)(m0 >> 8);
            if (l >= 4) vm[a+1] = (vm[a+1] & (byte)(m1)) | (byte)(m0);
        }

        cursor_x += 4;
    }

    // Печать символа Tahoma
    void prn11(byte ch) {

        heapvm;

        ch -= 32;

        word x0 = LPW(_tahoma11_offset[ch]);
        byte sz = LPM(_tahoma11_size[ch]);

        // Позиция символа в битовой карте
        word h  = x0 >> 3;
        byte l  = x0 & 7;
        byte cx = cursor_x >> 3,
             cl = cursor_x & 7;

        // Высота символа: 11 пикселей
        for (int i = 0; i < 11; i++) {

            // Получение данных и сдвиг на правильную позицию
            word st = (256*LPM(_tahoma11_bitmap[h]) + 1*LPM(_tahoma11_bitmap[h+1])) << l;
            word m1 = (0xFFF0000 >> sz); // Выделить единичные биты слева
            int  at = LPW(height[cursor_y + i]) + cx;

            word  mk = st & m1;  // Удалить лишние справа от символа
            dword td =  ( (dword) mk << 8) >> cl;  // Передвинуть
            dword tm = ~(((dword) m1 << 8) >> cl); // Для удаления места под символом

            // Очистить фон картинки и наложить туда новый глиф
            if (_overlap) {

                vm[at]   &= (tm >> 16); // | (td >> 16);
                vm[at+1] &= (tm >>  8); // | (td >> 8);
                vm[at+2] &= (tm >>  0); // | (td >> 0);
            }

            vm[at]   |= (td >> 16);
            vm[at+1] |= (td >> 8);
            vm[at+2] |= (td >> 0);

            h += 112;
        }

        cursor_x += sz;
    }

    // Печать одного символа
    void prn(byte ch) {

        switch (_size) {

            case 4:  prn4 (ch); break;
            case 11: prn11(ch); break;
        }
    }

    // Прочесть байт utf8/rus
    byte fetch(const char* s, int& i) {

        byte ch = s[i++];

        // UTF
        if      (ch == 0xD0) { ch = s[i++] - 0x10; if (ch == 0x71) ch = 0xC0; }
        else if (ch == 0xD1) { ch = s[i++] + 0x30; }

        return ch;
    }

    // Печать строки, возможно в позиции XY, или размер шрифта sz
    void print(const char* s, int x = -1, int y = -1, int sz = -1) {

        int i = 0;

        // Позиция
        if (x >= 0 && y >= 0) locate(x, y);

        // Установить новый размер шрифта
        if (sz >= 0) _size = sz;

        while (s[i]) { byte ch = fetch(s, i); prn(ch); }
    }
};
