#include <avrio.h>
#include <print.h>
#include <keyb.h>

#include "hello.h"

// Тестовая память
unsigned char mem[4096];

#include "cpu86.cc"

// Реализация интерефейса
// -----------------------------------------
class K86_inst : public K86 {

public:

    // Чтение байта
    u8 read(u32 address) {
        return mem[address & 0xFFF];
    }

    // Запись байта
    void write(u32 address, u8 data) {

        if (address >= 0xB800 && address < 0xBB00) {
            address -= 0xB800;
            locate(address & 31, address >> 5);
            prn(data);
        } else {
            mem[address & 0xFFF] = data;
        }
    }

    u8 pin(u16 port) {
        return 0xFF;
    }

    void pout(u16 port, u8 data) { }

    // Показать регистры (временный код)
    void debug() {

        // Основной набор регистров
        locate(0, 0); fore(3); print("AX"); fore(7); printhex(regs[_AX], 16); fore(3); print(" SP"); fore(7); printhex(regs[_SP], 16);
        locate(0, 1); fore(3); print("BX"); fore(7); printhex(regs[_BX], 16); fore(3); print(" BP"); fore(7); printhex(regs[_BP], 16);
        locate(0, 2); fore(3); print("CX"); fore(7); printhex(regs[_CX], 16); fore(3); print(" SI"); fore(7); printhex(regs[_SI], 16);
        locate(0, 3); fore(3); print("DX"); fore(7); printhex(regs[_DX], 16); fore(3); print(" DI"); fore(7); printhex(regs[_DI], 16);
        locate(0, 4); fore(3); print("ES"); fore(7); printhex(segs[_ES], 16); fore(3); print(" DS"); fore(7); printhex(segs[_DS], 16);
        locate(0, 5); fore(3); print("CS"); fore(7); printhex(segs[_CS], 16); fore(3); print(" SS"); fore(7); printhex(segs[_SS], 16);
        locate(0, 6); fore(3); print("IP"); fore(7); printhex(ip, 16);

        // Вывести список флагов
        locate(0, 7); fore(3); print("FL"); fore(7); printhex(flags, 12); fore(4);
        prn(flags & 0x800 ? 'O' : '-');
        // prn(flags & 0x400 ? 'D' : '-'); prn(flags & 0x200 ? 'I' : '-'); prn(flags & 0x100 ? 'T' : '-');
        prn(flags & 0x080 ? 'S' : '-');
        prn(flags & 0x040 ? 'Z' : '-');
        prn(flags & 0x010 ? 'A' : '-');
        prn(flags & 0x004 ? 'P' : '-');
        prn(flags & 0x001 ? 'C' : '-');
    }
};

// -----------------------------------------
K86_inst cpu;

int main() {

    cls();

    // Скопировать программу в память
    for (int i = 0; i < 4096; i++) mem[i] = LPM(_data[i]);

    // Запуск процессора
    while (cpu.halted() == 0) {
        cpu.step();
    }

    cpu.debug();

    for(;;);
}
