byte kb_shift = 0;

const static char kb_att[] PROGMEM = {
    //  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
        0,0x18,   0 ,0x14,0x12,0x10,0x11,0x1E,  0 ,0x19,0x17,0x15,0x13,  9,  '`',  0, // 0
        0 ,  0 ,  1 ,  0 ,  0 , 'Q', '1',  0 ,  0 ,  0 , 'Z', 'S', 'A', 'W', '2',  0, // 1
        0 , 'C', 'X', 'D', 'E', '4', '3',  0 ,  0 , ' ', 'V', 'F', 'T', 'R', '5',  0, // 2
        0 , 'N', 'B', 'H', 'G', 'Y', '6',  0 ,  0 ,  0 , 'M', 'J', 'U', '7', '8',  0, // 3
        0 , ',', 'K', 'I', 'O', '0', '9',  0 ,  0 , '.', '/', 'L', ';', 'P', '-',  0, // 4
        0 ,  0 ,'\'',  0 , '[', '=',  0 ,  0 ,  0 ,  1 ,  10, ']',  0 ,'\\',  0 ,  0, // 5
        0 ,  0 ,  0 ,  0 ,  0 ,  0 ,   8,  0 ,  0 ,0x0C,  0 ,0x06,0x0B,  0 ,  0 ,  0, // 6
        0 ,0x0F,0x05,  0 ,0x07,0x04,  27,  0 ,0x1A,  0 ,0x0E,  0 ,  0 ,0x0D,  0 ,  0, // 7
        0 ,  0 ,  0 ,0x16,  0 ,  0 ,   0,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  // 8
};

// Ожидание нажатия клавиши
byte kb_wait() {

    while ((inp(STATUS) & 1) == 0);
    outp(STATUS, inp(STATUS) & ~1);
    return inp(KEYB);
}

// Получение нажатой клавиши
byte kb_getch() {

    for (;;) {

        byte kb = kb_wait();

        // Клавиша отжата: прочитать и пропустить ее
        if (kb == 0xF0) {

            kb = kb_wait();

            // Клавиша LSHIFT / RSHIFT
            if (kb == 0x12 || kb == 0x59) kb_shift = 0;

        } else {

            // Клавиша LSHIFT / RSHIFT
            if (kb == 0x12 || kb == 0x59) {
                kb_shift = 1;

            } else if ((kb = LPM(kb_att[kb]))) {

                if (kb_shift == 0 && kb >= 'A' && kb <= 'Z') {
                    kb += 'a' - 'A';

                } else if (kb_shift) {

                    // При нажатой клавише SHIFT
                    switch (kb) {

                        case '1': kb = '!'; break;
                        case '2': kb = '@'; break;
                        case '3': kb = '#'; break;
                        case '4': kb = '$'; break;
                        case '5': kb = '%'; break;
                        case '6': kb = '^'; break;
                        case '7': kb = '&'; break;
                        case '8': kb = '*'; break;
                        case '9': kb = '('; break;
                        case '0': kb = ')'; break;
                        case '-': kb = '_'; break;
                        case '=': kb = '+'; break;
                        case '[': kb = '{'; break;
                        case ']': kb = '}'; break;
                        case ';': kb = ':'; break;
                        case '\'': kb = '"'; break;
                        case '\\': kb = '|'; break;
                        case ',': kb = '<'; break;
                        case '.': kb = '>'; break;
                        case '/': kb = '?'; break;
                        case '`': kb = '~'; break;
                    }
                }

                return kb;
            }

        }
    }
}
