/*
 * Этот файл покрыт различными некультурными лицензиями и не собирается
 * на этом останавливаться! LICENSE NO WARRANTY COPYRIGHT TRADEMARK RIGHTS
 * звучит очень круто, как-то не по-нашему, ну вы понимаете, да!
 */

#ifndef __AVRIO_GENERIC_FILE
#define __AVRIO_GENERIC_FILE

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// Пример как пользоваться: LPM(my_best_array[i])
#define LPM(t)  pgm_read_byte(&t)
#define LPW(t)  pgm_read_word(&t)
#define stop    for(;;)

// Базовые типы данных
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long dword;

// Чтение|запись в порты
inline byte inp(byte port)           { return ((volatile byte*) 0x20)[port]; }
inline void out(byte port, byte val) { ((volatile byte*) 0x20)[port] = val; }

// Объявление указателя на память (имя x, адрес a)
#define heap(x, a) byte* x = (byte*) a
#define heapvm byte* vm = (byte*) 0xF000

#endif
