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
#define stop    for (;;);

// Базовые типы данных
#define byte    unsigned char
#define uint    unsigned int
#define word    unsigned int
#define ulong   unsigned long
#define dword   unsigned long

// Указатель на память
#define heap(x,y)   volatile byte* x  = (byte*)y;
#define heapvm      volatile byte* vm = (byte*)0x8000;

// Чтение и запись в порт
inline byte inp(int port) { return ((volatile byte*)0x20)[port]; }
inline void outp(int port, unsigned char val) { ((volatile unsigned char*)0x20)[port] = val; }

#endif
