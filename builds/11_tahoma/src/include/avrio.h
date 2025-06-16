#ifndef __AVRIO_HEADER_FILE
#define __AVRIO_HEADER_FILE

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// Пример как пользоваться LPM(data[i])
#define LPM(t) pgm_read_byte(&t)
#define LPW(t) pgm_read_word(&t)

// Ссылка на пустой адрес
#define brk asm volatile("sleep")

// Карта портов
enum IOMap {

    BANK        = 0x00,
    KEYB        = 0x01, // R
    BORDER      = 0x01, // W

    STATUS      = 0x02, // R
    SPI_STATUS  = 0x02, // R
    SPI_DATA    = 0x03, // RW
    SPI_CMD     = 0x04, // W

    IRQ1        = 0x0E,
    TIMERL      = 0x0E,
    TIMERH      = 0x0F,
};

enum KEYBOARD_KEY {

    // Управление
    kb_LSHIFT      = 0x01,
    kb_LALT        = 0x02,
    kb_LCTRL       = 0x03,

    // Стрелки
    kb_UP          = 0x04,
    kb_DN          = 0x05,
    kb_LF          = 0x06,
    kb_RT          = 0x07,

    kb_BS          = 0x08,
    kb_TAB         = 0x09,
    kb_ENTER       = 0x0A,

    kb_HOME        = 0x0B,
    kb_END         = 0x0C,
    kb_PGUP        = 0x0D,
    kb_PGDN        = 0x0E,
    kb_DEL         = 0x0F,

    kb_F1          = 0x10,
    kb_F2          = 0x11,
    kb_F3          = 0x12,
    kb_F4          = 0x13,
    kb_F5          = 0x14,
    kb_F6          = 0x15,
    kb_F7          = 0x16,
    kb_F8          = 0x17,
    kb_F9          = 0x18,
    kb_F10         = 0x19,
    kb_F11         = 0x1A,
    kb_ESC         = 0x1B,
    kb_INS         = 0x1C,
    kb_NL          = 0x1D,
    kb_F12         = 0x1E,
    kb_SPECIAL     = 0x1F          // Особая клавиша
};

// Базовые типы данных
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long dword;

// Чтение|запись в порты
inline byte inp(int port) { return ((volatile byte*) 0x20)[port]; }
inline void outp(int port, unsigned char val) { ((volatile unsigned char*) 0x20)[port] = val; }

// Объявление указателя на память (имя x, адрес a)
#define heap(x, a)  byte* x = (byte*) a

// Видеопамять спектрума
#define heapvm      byte* vm   = (byte*) 0xE000
#define heapbank    byte* bank = (byte*) 0xFF00

#endif
