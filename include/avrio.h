#ifndef __AVRIO
#define __AVRIO

// Доступ к PGM
#include <avr/pgmspace.h>
#define LPM(x) pgm_read_byte(&x)
#define LPW(x) pgm_read_word(&x)
#define STRING(x,y) extern const byte x[] PROGMEM; const byte x[] = y;

// Ссылка на пустой адрес
#define NULL    ((void*)0)
#define brk     asm volatile("sleep"); // break

// Базовые типы данных
#define byte        unsigned char
#define uint        unsigned int
#define word        unsigned int
#define ulong       unsigned long
#define dword       unsigned long

// Указатель на память
#define heap(x,y)     volatile byte* x = (byte*)y;
#define heapvm        volatile byte* vm = (byte*)0xB000;

// Чтение и запись в порт
inline byte inp(int port) { return ((volatile byte*)0x20)[port]; }
inline void outp(int port, unsigned char val) { ((volatile unsigned char*)0x20)[port] = val; }

// Графические манипуляции
inline void cx(int a)                       { outp(0x0C, a); outp(0x0D, a >> 8); }
inline void cy(int a)                       { outp(0x0E, a); outp(0x0F, a >> 8); }
inline void loc(int x, int y)               { cx(x); cy(y); }
inline void pset(int x, int y, byte v)      { loc(x,y); outp(0, v); }
inline void point(byte v)                   { outp(0x00, v); }
inline void border(byte v)                  { outp(0x01, v); }
inline void vconf(byte v)                   { outp(0x02, v); }
inline byte millis()                        { return inp(0); }

enum KBASCII
{
    key_UP          = 0x01,     // "Вверх"
    key_DOWN        = 0x02,     // "Вниз"
    key_LEFT        = 0x03,     // "Влево"
    key_RIGHT       = 0x04,     // "Вправо"
    key_HOME        = 0x05,
    key_END         = 0x06,
    key_PGUP        = 0x07,
    key_BS          = 0x08,     // Backspace
    key_TAB         = 0x09,     // [DOS]
    key_PGDN        = 0x0A,
    key_DEL         = 0x0B,
    key_INS         = 0x0C,
    key_ENTER       = 0x0D,     // [DOS]
    key_NL          = 0x0E,     // Num Lock
    key_CAP         = 0x0F,     // Caps Shift
    key_LSHIFT      = 0x10,
    key_LCTRL       = 0x11,
    key_LALT        = 0x12,
    key_LWIN        = 0x13,
    key_RSHIFT      = 0x14,
    key_RWIN        = 0x15,
    key_MENU        = 0x16,     // Кнопка Меню
    key_SCL         = 0x17,     // Scroll Lock
    key_NUM         = 0x18,     // Num Pad
    key_ESC         = 0x1B,     // [DOS]

    // Функциональные клавиши
    key_F1          = 0x80,
    key_F2          = 0x81,
    key_F3          = 0x82,
    key_F4          = 0x83,
    key_F5          = 0x84,
    key_F6          = 0x85,
    key_F7          = 0x86,
    key_F8          = 0x87,
    key_F9          = 0x88,
    key_F10         = 0x89,
    key_F11         = 0x8A,
    key_F12         = 0x8B,
};

#endif
