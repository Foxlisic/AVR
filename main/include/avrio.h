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

// Чтение из порта
inline byte inp(int port) { return ((volatile byte*)0x20)[port]; }

// Запись в порт
inline void outp(int port, unsigned char val) { ((volatile unsigned char*)0x20)[port] = val; }

// Графические манипуляции
inline void cx(byte a)                      { outp(0, a); }
inline void cy(byte a)                      { outp(1, a); }
inline void loc(byte x, byte y)             { outp(0, x); outp(0, y); }
inline void pset(byte x, byte y, byte v)    { outp(0, x); outp(0, y); outp(2, v); }
inline void padd(byte v)                    { outp(2, v); }
inline void border(byte v)                  { outp(0x0D, v); }

#endif
