#ifndef __AVRIO_HEADER_FILE
#define __AVRIO_HEADER_FILE

#include <avr/pgmspace.h>
#define LPM(x) pgm_read_byte(&x)

// Ссылка на пустой адрес
#define brk asm volatile("sleep")

// Базовые типы данных
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long dword;

enum KEY_ASCII {

    key_LSHIFT      = 0x01,
    key_LALT        = 0x02,
    key_LCTRL       = 0x03,
    key_UP          = 0x04,
    key_DN          = 0x05,
    key_LF          = 0x06,
    key_RT          = 0x07,
    key_BS          = 0x08,
    key_TAB         = 0x09,
    key_ENTER       = 0x0A,
    key_HOME        = 0x0B,
    key_END         = 0x0C,
    key_PGUP        = 0x0D,
    key_PGDN        = 0x0E,
    key_DEL         = 0x0F,
    key_F1          = 0x10,
    key_F2          = 0x11,
    key_F3          = 0x12,
    key_F4          = 0x13,
    key_F5          = 0x14,
    key_F6          = 0x15,
    key_F7          = 0x16,
    key_F8          = 0x17,
    key_F9          = 0x18,
    key_F10         = 0x19,
    key_F11         = 0x1A,
    key_ESC         = 0x1B,
    key_INS         = 0x1C,
    key_NL          = 0x1D,
    key_F12         = 0x1E,
    key_SPECIAL     = 0x1F          // Особая клавиша
};

// Чтение|запись в порты
inline byte inp (int port)           { return ((volatile byte*)0x20)[port]; }
inline void outp(int port, byte val) {        ((volatile byte*)0x20)[port] = val; }

// Объявление указателя на память (имя x, адрес a)
#define heap(x, a)  byte* x  = (byte*) a
#define heapvm      byte* vm = (byte*) 0xF000

#endif
