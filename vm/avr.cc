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

 // Чтение из памяти
uint8_t AVR::get(uint16_t addr)
{
    uint8_t dv = sram[addr];

    switch (addr)
    {
        case 0x20: return key_code;
        case 0x21: return millis;
        case 0x22: dv = key_press;  key_press = 0; return dv;
        case 0x23: dv = vblank;     vblank = 0; return dv;
        case 0x24: dv = sdstat;     sdstat &= 0x7F; return dv;
        case 0x25: return mouse_x;
        case 0x26: return mouse_y;
        case 0x27: dv = mouse_btn; mouse_btn &= 0x7F; return dv;
    }

    return dv;
}

// Сохранение в память
void AVR::put(uint16_t addr, uint8_t value)
{
    sram[addr] = value;

    switch (addr)
    {
        case 0x20: update_border(value); break;
        case 0x21: vpage = value & 1; break;
        case 0x22: lba = (lba & 0xFFFFFF00) | (value); break;
        case 0x23: lba = (lba & 0xFFFF00FF) | (value << 8); break;
        case 0x24: lba = (lba & 0xFF00FFFF) | (value << 16); break;
        case 0x25: lba = (lba & 0x00FFFFFF) | (value << 24); break;
        case 0x26: sdstat = sd_request(value); break; // CARD=3, BSY=0, READY=1, ERR=0 :: 0xB0
        case 0x5F: byte_to_flag(value);
    }
}

int main(int argc, char** argv)
{
    AVR* avr = new AVR(argc, argv);
    return avr->main();
}
