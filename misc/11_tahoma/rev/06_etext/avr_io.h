#ifndef __AVRIO_HEADER
#define __AVRIO_HEADER

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

enum PortsNum {

    PORT_INKEY  = 0,
    PORT_WRDATA = 0,        // W Данные для записи в VideoRAM/ROM
    PORT_FFCUR  = 1,        // W Адрес для записи
    SPI_DATA    = 2,
    SPI_STATUS  = 3,
    SPI_CMD     = 3,
    PORT_CURSOR = 4,
};

const static char kb_att[] PROGMEM = {
//  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0,0x18,   0 ,0x14,0x12,0x10,0x11,0x1E,  0 ,0x19,0x17,0x15,0x13,  9,  '`',  0, // 0
    0 ,  0 ,  1 ,  0 ,  0 , 'Q', '1',  0 ,  0 ,  0 , 'Z', 'S', 'A', 'W', '2',  0, // 1
    0 , 'C', 'X', 'D', 'E', '4', '3',  0 ,  0 , ' ', 'V', 'F', 'T', 'R', '5',  0, // 2
    0 , 'N', 'B', 'H', 'G', 'Y', '6',  0 ,  0 ,  0 , 'M', 'J', 'U', '7', '8',  0, // 3
    0 , ',', 'K', 'I', 'O', '0', '9',  0 ,  0 , '.', '/', 'L', ';', 'P', '-',  0, // 4
    0 ,  0 ,'\'',  0 , '[', '=',  0 ,  0 ,  0 ,  1 ,  10, ']',  0 ,'\\',  0 ,  0, // 5
    0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   8,  0 ,  0 ,0x0C,  0 ,0x06,0x0B,  0 ,  0 ,  0, // 6
    0 ,0x0F,0x05,  0 ,0x07,0x04,  27,  0 ,0x1A,  0 ,0x0E,  0 ,  0 ,0x0D,  0 ,  0, // 7
    0 ,  0 ,  0 ,0x16,  0 ,  0 ,   0,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  // 8
};


// Пример как пользоваться LPM(data[i])
#define LPM(t) pgm_read_byte(&t)
#define LPW(t) pgm_read_word(&t)
#define brk asm volatile("sleep")

// Базовые типы данных
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long dword;

// Чтение|запись в порты
inline byte inp(int port) { return ((volatile byte*) 0x20)[port]; }
inline void out(int port, unsigned char val) { ((volatile unsigned char*) 0x20)[port] = val; }

// Видеопамять
#define heapvm byte* vm = (byte*) 0xF000

// -----------------------------------------------------------------------------
// Общий класс для интерфейса взаимодействия
// -----------------------------------------------------------------------------

class IFace
{
protected:

    word    cursor_x, cursor_y;
    byte    term_attr;
    byte    _fore, _back;
    byte    kb_buf[8];
    volatile byte kb_shift, kb_up, kb_id, kb_cu;

public:

    // Конструктор
    IFace() { kb_shift = kb_up = kb_id = kb_cu = 0; _fore = 15; _back = -1; }

    // Очистить экран
    void cls(byte attr = 0x07) {

        heapvm;
        locate(0, 0);
        term_attr = attr;

        for (int i = 0; i < 4000; i += 2) {

            vm[i]   = 0x00;
            vm[i+1] = attr;
        }
    }

    // Обработка прерывания после INT0_vect
    void keyfetch() {

        byte kb = inp(PORT_INKEY);

        // Код отпущенной клавиши
        if (kb == 0xF0) {
            kb_up = 1;
        }
        // Код предыдущей клавиши был F0h
        else if (kb_up) {

            if (kb == 0x12 || kb == 0x59)
                kb_shift = 0;

            kb_up = 0;
        }
        // Нажатие на SHIFT
        else if (kb == 0x12 || kb == 0x59) {

            kb_shift = 1;
            kb_up = 0;
        }
        // Существующий скан-код?
        else if ((kb = LPM(kb_att[kb]))) {

            if (kb_shift == 0 && kb >= 'A' && kb <= 'Z') {
                kb += 'a' - 'A';

            } else if (kb_shift) {

                // При нажатой клавише SHIFT
                switch (kb) {

                    case '1': kb = '!'; break;
                    case '2': kb = '@'; break;
                    case '3': kb = '#'; break;
                    case '4': kb = '$'; break;
                    case '5': kb = '%'; break;
                    case '6': kb = '^'; break;
                    case '7': kb = '&'; break;
                    case '8': kb = '*'; break;
                    case '9': kb = '('; break;
                    case '0': kb = ')'; break;
                    case '-': kb = '_'; break;
                    case '=': kb = '+'; break;
                    case '[': kb = '{'; break;
                    case ']': kb = '}'; break;
                    case ';': kb = ':'; break;
                    case '\'': kb = '"'; break;
                    case '\\': kb = '|'; break;
                    case ',': kb = '<'; break;
                    case '.': kb = '>'; break;
                    case '/': kb = '?'; break;
                    case '`': kb = '~'; break;
                }
            }

            kb_buf[kb_id] = kb;
            kb_id = (kb_id + 1) & 7;
            kb_up = 0;
        }
    }

    // Считывание следующей клавиши из буфера
    byte inkey() {

        if (kb_id == kb_cu) return 0;

        byte kb = kb_buf[kb_cu];
        kb_cu = (kb_cu + 1) & 7;

        return kb;
    }

    // Установка цветов
    void color(byte f, byte b = 128) {

        if (f != 128) { _fore = f; term_attr = f; }
        if (b != 128) { _back = b; }
    }

    // Запись адреса видеопамяти
    void address(long k) {

        out(PORT_FFCUR, k >> 8);
        out(PORT_FFCUR, k);
    }

    // Установка позиции курсора
    void locate(int x, int y) {

        cursor_x = x;
        cursor_y = y;

        int cursor = y*80 + x;
        out(PORT_CURSOR, cursor >> 8);
        out(PORT_CURSOR, cursor);
    }

    // Печать символа в телетайп-режиме
    void term(byte ch) {

        heapvm;

        int cursor = 160*cursor_y + (cursor_x << 1);

        vm[cursor  ] = ch;
        vm[cursor+1] = term_attr;

        cursor_x++;
        if (cursor_x == 80) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y == 25) {
                cursor_y = 24;
            }
        }
    }

    // Печать строки
    void print(const char* s) {

        int i = 0;
        while (s[i]) {

            unsigned char ch = s[i++];

            // UTF
            if      (ch == 0xD0) { ch = s[i++] - 0x10; if (ch == 0x71) ch = 0xC0; }
            else if (ch == 0xD1) { ch = s[i++] + 0x60; }

            term(ch);
        }

        locate(cursor_x, cursor_y);
    }

    // Печать строки из PROGMEM
    void printpgm(const char* s) {

        int i = 0;
        while (LPM(s[i])) {

            unsigned char ch = LPM(s[i]); i++;

            // UTF
            if      (ch == 0xD0) { ch = LPM(s[i]) - 0x10; if (ch == 0x71) ch = 0xC0; i++; }
            else if (ch == 0xD1) { ch = LPM(s[i]) + 0x60; i++; }

            term(ch);
        }
    }

    // 16 bit число
    void printint(int v) {

        char bf[5];
        int  n = 0;

        // Негативное значение
        if (v < 0) { term('-'); v = -v; }

        // Пропечатка числа в буфере
        do { char mod = v % 10; v /= 10; bf[n++] = mod; } while (v);

        // Пропечатка в буфер
        for (int i = n-1; i >= 0; i--) term('0' + bf[i]);
    }

    // Печать 8 битного шестнадцатеричного числа
    void printhex(byte w) {

        byte a = w >> 4, b = w & 15;

        term('0' + a + (a > 9 ? 7 : 0));
        term('0' + b + (b > 9 ? 7 : 0));
    }
};

#endif
