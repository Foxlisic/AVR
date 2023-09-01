class format {
protected:
public:

    byte buffer[64];

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

    // Печать float, n=2
    byte f2a(float x, char n) {

        byte tmpbuf[32];
        byte cs = 0, t = 0;

        // Отрицательное число
        if (x < 0) { x = -x; tmpbuf[cs++] = '-'; }

        // Получить целочисленную и дробную части
        unsigned long i = (unsigned long) x;
        float f = x - i;

        // Печать целочисленного значения
        i2a(i);
        while (buffer[t]) tmpbuf[cs++] = buffer[t++];
        tmpbuf[cs++] = '.';

        // Печать остатка
        for (int k = 0; k < n; k++) {

            f *= 10;
            tmpbuf[cs++] = f + '0';
            f = f - (int)f;
        }

        // Скопировать во временный буфер
        for (t = 0; t < cs; t++) buffer[t] = tmpbuf[t];
        buffer[cs] = 0;

        return cs;
    }
};
