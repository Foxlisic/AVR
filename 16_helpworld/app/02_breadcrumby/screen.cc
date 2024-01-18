byte hmode = 0;

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

    // Выдача стартового экрана
    block(12, 5, 55, 10, ' ', 0x60);
    block(12, 5, 55, 1,  0xB2, 0x40);
    block(12, 5, 1,  10, 0xB1, 0x0E);
    block(12, 5, 1,  1,  ' ', 0);
    block(66, 5, 1,  1,  ' ', 0);
    block(66, 6, 1,  9,  0xB2, 0x60);
    for (int i = 12; i < 70; i += 6) block(i, 7, 1, 7, 0xB0, 0x60);
    locate(32, 10); attr(0x0E); print(" ИГРА ПРО ХЛЕБ ");

    // Переключение меню
    do {

        locate(30, 17); attr(sel == 0 ? 0x0E : 0x07); print("  Начать игру");
        locate(30, 18); attr(sel == 1 ? 0x0E : 0x07); print("  Неешь, подумой");
        locate(30, 19); attr(sel == 2 ? 0x0E : 0x07); print("  Выход");
        locate(30, 17 + sel); attr(0x0E); prn(0x1A);
        locate(0, 25);

        ch = getch();
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
        locate(5, 10);  print("A fatal exception 0E has occurred at 0028:C0025606 in VXD VFAT(01) +");
        locate(5, 11);  print("0000798E. The current application will be terminated.");
        locate(5, 13);  print("* Press any key to terminate the current application.");
        locate(5, 14);  print("* Press CTRL + ALT + DEL again to restart your computer. You will");
        locate(5, 15);  print("  lose and unsaved information in all applications.");
        locate(26, 17); print("Press any key to continue");
        hide(); stop;
    }

    return sel;
}

// Вывод PGM-сообщения на экран t-title, m-message
void message(int x, int y, const char* m, const char* t) {

    byte wx = 0, hx = 5, lx = 0, ch;
    int i = 0;

    while ((ch = pgm_read_byte(&m[i++]))) {

        // Новая строка
        if (ch == 0x0A) { lx = 0; hx++; }
        else { if (ch == 0xD0 || ch == 0xD1) { i++; } lx++; if (wx < lx) wx = lx; }
    }

    outline(x, y, wx + 4, hx, 0x07);
    caret_at = x + 2;
    locate(caret_at, y + 2); print(m, 1);
    caret_at = 0;

    locate(x + 2, y);
    attr(0x70); print(t ,1);
}

void hero(byte x, byte y, byte pos = 0) {

    locate(caret_at = x, y);

    attr(0x07);
    switch (hmode) {

        case 0: print(" \x01\n/\xB3\\\n/ \\"); break;
        case 1: print("{\x02}\n/\xB3\\\n/ \\"); break;
        case 2: print(" \x1B\x0C\x03\x1B\x07\n/\xB3\\\n/ \\"); break;
    }

    caret_at = 0;
}

// Экран 1: звездное небо
void splash_1() {

    heapvm;
    byte H[3][4] = {
        {0, 18, 14, 8},
        {12, 16, 20, 10},
        {25, 19, 15, 12},
    };

    cls(0x07);
    for (int i = 0; i < 80; i++) {

        word k = rand();
        vm[(k & 0xFFE) | 0] = 0xF9 + (k & 1);
        vm[(k & 0xFFE) | 1] = 0x07 + (k&2?1:0);
    }

    // Нарисовать дома
    for (int i = 0; i < 3; i++) {

        // Случайные окна
        for (int y = H[i][1] + 1; y < H[i][1] + H[i][3] - 1; y++)
        for (int x = H[i][0] + 1; x < H[i][0] + H[i][2] - 1; x++) {

            byte w = ((rand() % 7) == 0);
            putx(x, y, w? 0xFE : ' ', 0x07);
        }

        // Окантовка
        outline(H[i][0], H[i][1], H[i][2], H[i][3]);
    }

    message(2, 1, s1e1, t1);
    getch();
}

//  Экран 2: Выбор персонажа
void splash_2() {

    cls();
    byte ch;
    locate(30,3); attr(0x70); print(" CHOOSE YOU DESTINY ");

    // Рисовать персонажей
    hmode = 0; hero(16, 8);
    hmode = 1; hero(38, 8);
    hmode = 2; hero(61, 8);
    hmode = 0;

    do {

        attr(hmode == 0 ? 0x0A : 0x07); outline(10, 5, 15, 9); locate(16, 14); print("Мале");
        attr(hmode == 1 ? 0x0A : 0x07); outline(32, 5, 15, 9); locate(37, 14); print("Фемале");
        attr(hmode == 2 ? 0x0A : 0x07); outline(54, 5, 17, 9); locate(58, 14); print("Андертале");
        hide();

        ch = getch();
        if (ch == 0x06) { hmode = hmode == 0 ? 2 : hmode - 1; }
        if (ch == 0x04) { hmode = hmode == 2 ? 0 : hmode + 1; }
    }
    while (ch != 0x0A);

}
