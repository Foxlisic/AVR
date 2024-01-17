#include <avrio.h>
#include <qb.cc>

ISR(INT0_vect) { kb_irq_handler(); }

// Нарисовать блок
void block(int x1, int y1, int w, int h, byte ch, byte at) {

    heapvm;
    for (int i = y1; i < y1 + h; i++)
    for (int j = x1; j < x1 + w; j++) {
        vm[2*j + i*160 + 0] = ch;
        vm[2*j + i*160 + 1] = at;
    }
}

// Выдача
void exclamation(const char* t, byte h) {

    block(16, 8, 50, 3+h, 0xB2, 0x70);
    block(15, 7, 50, 1, ' ', 0x17);
    block(15, 8, 50, 2+h, ' ', 0x70);

    locate(16, 7); attr(0x17); print(t);
    attr(0x70);
}

// Логитип хлеба
int breadlogo() {

    byte ch, sel = 0;

    cls(0x07);

    block(12, 5, 55, 10, ' ', 0x60);
    block(12, 5, 55, 1,  0xB2, 0x40);
    block(12, 5, 1,  10, 0xB1, 0x0E);
    block(12, 5, 1,  1,  ' ', 0);
    block(66, 5, 1,  1,  ' ', 0);
    block(66, 6, 1,  9,  0xB2, 0x60);
    for (int i = 12; i < 70; i += 6) block(i, 7, 1, 7, 0xB0, 0x60);
    locate(32, 10); attr(0x0E); print(" ИГРА ПРО ХЛЕБ ");

    do {

        locate(30, 17); attr(sel == 0 ? 0x0E : 0x07); print("  Начать игру");
        locate(30, 18); attr(sel == 1 ? 0x0E : 0x07); print("  Неешь, подумой");
        locate(30, 19); attr(sel == 2 ? 0x0E : 0x07); print("  Выход");
        locate(30, 17 + sel); attr(0x0E); prn(0x1A);
        locate(0, 25);

        ch = inkey(1);
        if (ch == 0x03) { if (sel == 0) sel = 2; else sel--; }
        if (ch == 0x05) { if (sel == 2) sel = 0; else sel++; }
    }
    while (ch != 0x0A);

    // Неправильные выходы
    if (sel == 1) {

        exclamation("Игра окончена", 1);
        locate(16, 9); print("Вы съели хлеб слишком быстро, а он был в плесени");
        hide(); stop;
    }
    // Выход из игры
    else if (sel == 2) {

        cls(0x17);
        locate(35, 8); attr(0x71); print(" Windows "); attr(0x17);
        locate(5, 10); print("A fatal exception 0E has occurred at 0028:C0025606 in VXD VFAT(01) +");
        locate(5, 11); print("0000798E. The current application will be terminated.");
        locate(5, 13); print("* Press any key to terminate the current application.");
        locate(5, 14); print("* Press CTRL + ALT + DEL again to restart your computer. You will");
        locate(5, 15); print("  lose and unsaved information in all applications.");
        locate(26, 17); print("Press any key to continue");
        hide(); stop;
    }

    return sel;
}

int main() {

    sei();

    breadlogo();



    stop;
}
