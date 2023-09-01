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

    BANK    = 0x00, // RW Банк памяти F000-FFFF
    VMODE   = 0x01, // RW Видеорежим
    KEYB    = 0x02, // R  Символ с клавиатуры
    SDCMD   = 0x0C, // RW Чтение или прием байта SD
    SDCTL   = 0x0D, // RW Команда 1=init, 2=ce0, 3-ce1
    CURX    = 0x0E, // RW Курсор по X
    CURY    = 0x0F, // RW Курсор по Y
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
