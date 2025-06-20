#include "tahoma.h"

class GUI
{
public:

    int mouse_x = 0,
        mouse_y = 0;

    // Обчистить экран
    void cls(byte color, byte back = 255)
    {
        back = back == 255 ? color : back;

        loc(0, 0);
        for (int i = 0; i < 400; i++)
        for (int j = 0; j < 640; j++) {
            point((i ^ j) & 1 ? color : back);
        }
    }

    // Нарисовать блок на экране
    void block(int x1, int y1, int x2, int y2, byte color)
    {
        if (x1 < 0) x1 = 0; else if (x1 >= 640) return;
        if (y1 < 0) y1 = 0; else if (y1 >= 480) return;

        if (x2 >= 640) x2 = 640;
        if (y2 >= 400) x2 = 400;

        if (x1 > x2 || y1 > y2) return;

        for (int y = y1; y <= y2; y++)
        {
            loc(x1, y);
            for (int x = x1; x <= x2; x++) { point(color); }
        }
    }

    // Печать символа
    byte tahoma(int x, int y, byte ch, byte color)
    {
        ch -= 32;

        word x0   = LPW(tahoma_offset[ch]);
        byte size = LPM(tahoma_size[ch]);

        word h = x0 >> 3;
        byte l = x0 & 7;

        // Высота символа: 11 пикселей
        for (int i = 0; i < 11; i++) {

            loc(x, y);

            word st = (256*LPM(tahoma_bitmap[h]) + 1*LPM(tahoma_bitmap[h+1])) << l;
            for (int j = 0; j < size; j++) {

                if ((st & 0x8000)) { cx(x); point(color); }

                st <<= 1;
                x++;
            }

            h += 112;
            x -= size;
            y += 1;
        }

        return size;
    }

    // Обрисовать квадрат
    void lineb(int x1, int y1, int x2, int y2, byte color)
    {
        block(x1, y1, x2, y1, color);
        block(x1, y2, x2, y2, color);
        block(x1, y1, x1, y2, color);
        block(x2, y1, x2, y2, color);
    }

    // Пропечать строки
    int print(int x, int y, const unsigned char* p, byte color, byte bold = 0)
    {
        byte m, s;
        int i = 0;
        int size = 0;

        // Ссылку придется разыменовать, чтобы напечатать ее тут
        while ((m = LPM(*p + i))) {

            s = tahoma(x, y, m, color);

            if (bold) {

                tahoma(x+1, y, m, color);
                s++;
            }

            size += s;
            x += s;

            i++;
        }

        return 0;
    }

    void button(int x1, int y1, int x2, int y2, byte press = 0)
    {
        block(x1+1, y1+1, x2-1, y2-1, 7);
        block(x1,   y1,   x1,   y2,   15);
        block(x1+1, y1,   x2,   y1,   15);
        block(x2,   y1,   x2,   y2,   0);
        block(x1,   y2,   x2-1, y2,   0);
        block(x1+1, y2-1, x2-1, y2-1, 8);
        block(x2-1, y1+1, x2-1, y2-1, 8);
    }

} gui;
