#ifndef __AVRIO_HFILE
#define __AVRIO_HFILE

#include <avr/interrupt.h>

// Ссылка на пустой адрес
#define NULL    ((void*)0)
#define brk     asm volatile("sleep"); // break

// Базовые типы данных
#define byte        unsigned char
#define uint        unsigned int
#define word        unsigned int
#define ulong       unsigned long
#define dword       unsigned long

// Описания всех портов
enum PortsID {

    BANK    = 0x00, // RW
    VMODE   = 0x01, // RW
    KEYB    = 0x02, // R
    SDCMD   = 0x0C, // RW
    SDCTL   = 0x0D, // RW
    CURX    = 0x0E,
    CURY    = 0x0F,
};

// Список видеорежимов
enum VideoModes {

    VM_80x25        = 0,
    VM_320x200      = 1,
};

// Чтение из порта
inline byte inp(int port) {
    return ((volatile byte*)0x20)[port];
}

// Запись в порт
inline void outp(int port, unsigned char val) {
    ((volatile unsigned char*)0x20)[port] = val;
}

// Объявление указателя на память (имя x, адрес a)
#define heap(x, a)  byte* x = (byte*) a
#define heapvm      byte* vm = (byte*) 0xF000
#define bank(x)     outp(BANK, x)
#define vmode(x)    outp(VMODE, x)

#endif
