#ifndef __AVRIO_HEADER
#define __AVRIO_HEADER

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

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
inline void outp(int port, unsigned char val) { ((volatile unsigned char*) 0x20)[port] = val; }

// Видеопамять
#define heapvm byte* vm = (byte*) 0xF000

#endif
