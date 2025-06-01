class c4io
{
protected:

    byte attr;
    byte locx, locy;

public:

    // Очистить экран от ненужного мусора
    void cls(byte _attr = 0x07)
    {
        heapvm;
        attr = _attr;
        for (int i = 0; i < 4000; i += 2)
        {
            vm[i]   = 0x00;
            vm[i+1] = attr;
        }
    }

    // Задать дальнейший цвет атрибута
    void color(byte c)
    {
        attr = c;
    }

    // Установка точки где будет печататься
    void at(byte x, byte y)
    {
        locx = x;
        locy = y;
    }

    // Установка курсора куда нужно
    void loc(byte x, byte y)
    {
        outp(0, x);
        outp(1, y);
        at(x, y);
    }

    void pset(int x, int y, byte ch = 7)
    {
        heapvm;

        ch &= 15;
        if (x >= 0 && x < 80 && y >= 0 && y < 50)
        {
            int a = 2*x + 160*(y >> 1);

            vm[a]   = 0xDC;
            vm[a+1] = y & 1 ? (vm[a + 1] & 0xF0) | ch : (vm[a + 1] & 0x0F) | (ch*16);
        }
    }

    void line(int x1, int y1, int x2, int y2, int color)
    {
        // Инициализация смещений
        int signx  = x1 < x2 ? 1 : -1;
        int signy  = y1 < y2 ? 1 : -1;
        int deltax = x2 > x1 ? x2 - x1 : x1 - x2;
        int deltay = y2 > y1 ? y2 - y1 : y1 - y2;
        int error  = deltax - deltay;
        int error2;

        // Если линия - это точка
        pset(x2, y2, color);

        // Перебирать до конца
        while ((x1 != x2) || (y1 != y2)) {

            pset(x1, y1, color);
            error2 = 2 * error;

            if (error2 > -deltay) { error -= deltay; x1 += signx; }   // Коррекция по X
            if (error2 <  deltax) { error += deltax; y1 += signy; }   // Коррекция по Y
        }
    }

    void circle(int xc, int yc, int radius, int color)
    {
        int x = 0,
            y = radius,
            d = 3 - 2*y;

        while (x <= y) {

            // Верхний и нижний сектор
            pset(xc - x, yc + y, color);
            pset(xc + x, yc + y, color);
            pset(xc - x, yc - y, color);
            pset(xc + x, yc - y, color);

            // Левый и правый сектор
            pset(xc - y, yc + x, color);
            pset(xc + y, yc + x, color);
            pset(xc - y, yc - x, color);
            pset(xc + y, yc - x, color);

            d += (4*x + 6);
            if (d >= 0) {
                d += 4*(1 - y);
                y--;
            }

            x++;
        }
    }

    // Пропечать символа
    void prn(byte ch)
    {
        heapvm;

        if (ch == 10 || locx >= 80) {

            locx = 0;
            locy++;

            if (locy >= 25) {

                // @TODO поднять скрин
                locy = 25;
            }
        }

        int xy = 2*locx + 160*locy;

        vm[xy]   = ch;
        vm[xy+1] = attr;

        locx++;
    }

    byte print(const char* s)
    {
        int i = 0;
        byte b;
        while ((b = LPM(s[i]))) {

            prn(b);
            i++;
        }

        return i;
    }

    // Чтение нажатия на клавишу
    byte inkey()
    {
        if (inp(0x11) & 1) {
            return inp(0x10);
        } else {
            return 0;
        }
    }

    byte getch()
    {
        int b = 0;
        while (0 == (b = inkey()));
        return b;
    }

} io;
