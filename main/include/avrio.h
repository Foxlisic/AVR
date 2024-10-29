#ifndef __AVRIO
#define __AVRIO

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
enum PortsID 
{
    VIDEOMODE       = 0x00
};

// Список видеорежимов
enum VideoModes 
{
    VM_80x25        = 0,
    VM_320x200      = 2,
    VM_640x400      = 4
};

// Чтение из порта
inline byte inp(int port) { return ((volatile byte*)0x20)[port]; }

// Запись в порт
inline void outp(int port, unsigned char val) { ((volatile unsigned char*)0x20)[port] = val; }

#endif
