#include <avrio.cc>
#include "format.cc"

class Graphics {

protected:

    int  region_x1, region_y1, region_x2, region_y2;
    int  cursor_x, cursor_y;
    byte cursor_cl, wipe_under;
    Format form;

    // Вычисление адреса и банка
    word bank8(word x, word y) {

        word z = 320*y + x;
        outp(BANK, 8 + ((z >> 12) & 15));
        return (z & 0xFFF);
    }

public:

    // Запустить видеорежим
    Graphics* start() {

        outp(BANK,      0);
        outp(VIDEOMODE, SCREEN_13);

        region_x1  = 0;
        region_y1  = 0;
        region_x2  = 319;
        region_y2  = 199;
        cursor_x   = 0;
        cursor_y   = 0;
        cursor_cl  = 15;
        wipe_under = 0;

        return this;
    }

    // Очистка экрана
    Graphics* cls(byte cl) {

        block(0, 0, 319, 199, cl);
        return this;
    }

    // Установить точку
    Graphics* pset(word x, word y, byte cl) {

        heapvm;
        if (x >= 320 || y >= 200)
            return this;

        word z = bank8(x, y);
        vm[z] = cl;

        return this;
    }

    // Вернуть точку
    byte point(int x, int y) {

        heapvm;
        return vm[ bank8(x, y) ];
    }

    // Рисование блока. Необходимо, чтобы x1 < x2, y1 < y2
    Graphics* block(word x1, word y1, word x2, word y2, byte cl) {

        heapvm;

        word A = y1*320 + x1;
        word B = 8 + (A >> 12);

        A &= 0xFFF;
        for (word y = y1; y <= y2; y++) {

            bank(B);
            word Ap = A;

            // X++
            for (word x = x1; x <= x2; x++) {

                vm[A++] = cl;
                if (A & 0x1000) { bank(inp(BANK) + 1); A &= 0x0FFF; }
            }

            // Y++
            A = (Ap + 320);
            if (A & 0x1000) { B++; A &= 0x0FFF; }
        }

        return this;
    }

    // Рисование линии
    Graphics* line(int x1, int y1, int x2, int y2, byte cl) {

        if (y2 < y1) {
            x1 ^= x2; x2 ^= x1; x1 ^= x2;
            y1 ^= y2; y2 ^= y1; y1 ^= y2;
        }

        int deltax = x2 > x1 ? x2 - x1 : x1 - x2;
        int deltay = y2 - y1;
        int signx  = x1 < x2 ? 1 : -1;

        int error2;
        int error = deltax - deltay;

        while (x1 != x2 || y1 != y2)
        {
            pset(x1, y1, cl);
            error2 = error * 2;

            if (error2 > -deltay) {
                error -= deltay;
                x1 += signx;
            }

            if (error2 < deltax) {
                error += deltax;
                y1 += 1;
            }
        }

        pset(x1, y1, cl);
        return this;
    }

    // Блок линии
    Graphics* lineb(int x1, int y1, int x2, int y2, byte cl) {

        line(x1, y1, x2, y1, cl);
        line(x2, y1, x2, y2, cl);
        line(x2, y2, x1, y2, cl);
        line(x1, y2, x1, y1, cl);
        return this;
    }

    // Рисование окружности
    Graphics* circle(int xc, int yc, int r, byte c) {

        int x = 0;
        int y = r;
        int d = 3 - 2*y;

        while (x <= y) {

            // --
            pset(xc - x, yc + y, c);
            pset(xc + x, yc + y, c);
            pset(xc - x, yc - y, c);
            pset(xc + x, yc - y, c);
            pset(xc + y, yc + x, c);
            pset(xc - y, yc + x, c);
            pset(xc + y, yc - x, c);
            pset(xc - y, yc - x, c);
            // ...

            d += 4*x + 6;
            if (d >= 0) {
                d += 4*(1 - y);
                y--;
            }

            x++;
        }

        return this;
    }

    // Рисование окружности
    Graphics* circle_fill(int xc, int yc, int r, byte c) {

        int x = 0;
        int y = r;
        int d = 3 - 2*y;

        while (x <= y) {

            block(xc-x, yc-y, xc+x, yc-y, c);
            block(xc-x, yc+y, xc+x, yc+y, c);
            block(xc-y, yc-x, xc+y, yc-x, c);
            block(xc-y, yc+x, xc+y, yc+x, c);

            d += 4*x + 6;
            if (d >= 0) {
                d += 4*(1-y);
                y--;
            }

            x++;
        }

        return this;
    }

    // Печать символа
    Graphics* printchar(int x, int y, byte ch, byte cl) {

        byte t[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

        heap(vm, 0xf000);
        for (int i = 0; i < 16; i++) {

            bank(1);
            byte fn = vm[16*ch + i];
            for (int j = 0; j < 8; j++) {
                if (fn & t[j & 7])
                    pset(x + j, y + i, cl);
                else if (wipe_under & 0x10) {
                    pset(x + j, y + i, wipe_under & 15);
                }
            }
        }

        return this;
    }

    // Установка курсора
    Graphics* cursor(int x, int y) {

        cursor_x = x;
        cursor_y = y;
        return this;
    }

    // Выбор цвета
    Graphics* color(byte cl) {

        cursor_cl = cl;
        return this;
    }

    // Очищать ли background и в какой цвет (младший ниббл)
    Graphics* wiped(byte w) {

        wipe_under = w;
        return this;
    }

    // Печать в режиме телетайпа
    Graphics* printch(byte cl) {

        printchar(cursor_x, cursor_y, cl, cursor_cl);

        cursor_x += 8;
        if (cursor_x > region_x2) {
            cursor_x = region_x1;
            cursor_y += 16;
            // @todo region move
        }

        return this;
    }

    // Печать строки (utf8)
    int print(const char* s) {

        form.utf8_to_cp866(s);
        byte* b = form.get_buffer();

        int i = 0; while (b[i]) printch(b[i++]); return i;
    }

    void printint(long v) {

        form.i2a(v);
        print((const char*) form.get_buffer());
    }

    // Формы и окна
    // -----------------------------------------------------------------

    // Рисование окна
    Graphics* window(int x, int y, int w, int h, const char* s) {

        int x2 = x + w, y2 = y + h;

        block(x, y, x2, y2, 7);
        line(x2, y, x2, y2, 8);
        line(x, y2, x2, y2, 8);
        line(x, y, x2, y, 15);
        line(x, y, x, y2, 15);
        block(x+2,y+2,x2-2,y+18,3);

        cursor(x+4,y+2)->color(15);
        print(s);

        return this;
    }
};
