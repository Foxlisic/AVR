#include "avr.h"
#include "avr.cpp"
#include "avr_font.h"
#include "avr_assign.h"
#include "avr_disasm.cc"
#include "avr_assign.cc"
#include "avr_cpu.cc"

/**
 * Параметры командой строки:
 * -4 Использовать конфигурацию C4 Omdazz
 * -d Вывод отладочной информации
 */

int main(int argc, char** argv)
{
    AVR* avr = new AVR(argc, argv);
    return avr->main();
}
