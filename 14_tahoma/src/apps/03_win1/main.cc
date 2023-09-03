#include <avrio.h>
#include <display.h>

#include "wallpaper.h"

// Копировать строку из одной в другую
char* strcpy(char* __dest, const char* __src) {

    int i = 0;
    while (__src[i]) { __dest[i] = __src[i]; i++; }
    __dest[i] = 0;
    return __dest;
}

int strlen(const char* s) {
    int i = 0; while (s[i]) i++; return i;
}

#include "gui.h"

GUI gui;

static const word ICON1[64] PROGMEM = {
    0x5550, 0x0055, 0xFFE0, 0x007F, 0xFFD0, 0x00FF, 0x00A0, 0x0180,
    0x00D0, 0x0080, 0xFFA0, 0x01BF, 0xFFD0, 0x00BF, 0x1FA0, 0x01B8,
    0xDFD0, 0x00BF, 0x07A0, 0x01BE, 0xF7D0, 0x00BF, 0xF7A0, 0x01B9,
    0xF7D0, 0x00BF, 0x07A0, 0x01BE, 0xFFD0, 0x00BF, 0xFFA0, 0x01BF,
    0x5550, 0x00D5, 0xAAA4, 0x01AA, 0x000A, 0x0000, 0x5554, 0x0D55,
    0xFFFA, 0x0FFF, 0xFFF4, 0x07FF, 0x557A, 0x0FFF, 0x0034, 0x073F,
    0xFDF8, 0x0F3F, 0x0000, 0x0000, 0x6DA8, 0x0ADB, 0xDB50, 0x1DB6,
    0xB6C0, 0x3B6D, 0xFFA0, 0x3FFF, 0xB6C0, 0x5B6D, 0x0000, 0x0000};

void test1() {

    heapvm;

    cls(8*5 + 1);
    border(0);

    for (int i = 0; i < 6144+768; i++) vm[i] = LPM(_wallpaper[i]);

    font.size(11);

    // Панель
    block(0, 192-16, 255, 191, 0);
    for (int i = 0; i < 32; i++) { setattr(i, 22, 0x38); setattr(i, 23, 0x38); }

    // Чисто поматросить и бросить
    line(0, 192-16, 255, 192-16, 1);

    // Выпуклая кнопка
    line(1,  192-2,  32, 192-2, 1);
    line(32, 192-14, 32, 192-2, 1);

    font.locate(4, 179); font.print("Start!");

    // Чисто просто
    block (0, 32, 64, 47, 0);
    font.locate(2, 18+16);
    font.print("My Comp");

    for (int i = 0; i < 6; i++) {

        setattr(i,4,0x07+8*1+0x40);
        setattr(i,5,0x07+8*1);
    }

    // Бордер
    block (64, 8, 239, 95, 0);
    lineb (64, 32, 239, 95, 1);

    // Раскрашивание
    for (int i = 8; i < 30; i++) {

        setattr(i,1,8*1+7+0x40);
        setattr(i,2,8*1+7);
        setattr(i,3,8*7+0);

        for (int k = 4; k < 12; k++) setattr(i,k,8*7+0x40);
    }

    // Закрыть
    setattr(28,1,8*2+7+0x40); setattr(29,1,8*2+7+0x40);
    setattr(28,2,8*2+7);      setattr(29,2,8*2+7);

    line(227,11,237,21,1); line(226,11,236,21,1);
    line(227,21,237,11,1); line(226,21,236,11,1);

    font.locate(68, 11);      font.print("Notepad--");
    font.locate(68, 37+0*11); font.print("Я сделал русский шрифт!");
    font.locate(68, 37+1*11); font.print("Он работает и вполне даже...");
    font.locate(68, 37+2*11); font.print("Этот экран от Спектрума");

    font.overlap(0);
    font.print("Полуболд шрифт", 68, 37+3*11);
    font.print("Полуболд", 69, 37+3*11);
    font.overlap(1);

    font.print("File  Edit  About",68,24,4);
    setattr(8,3,7);
    setattr(9,3,7);
    setattr(10,3,7);

    font.print("Notepad--", 40, 181);
    font.print("08:09", 234, 181);

    // Ползунок
    gui.hscroller(29,4,8);

    // Кнопка снизу
    block(36, 178, 80, 178, 1);
    block(36, 178, 36, 190, 1);

    for (int i = 37; i < 80; i+=2) pset(i, 179, 1);
    for (int i = 179; i < 190; i+=2) pset(37, i, 1);

    // Изображение компа
    for (int i = 0; i < 32; i++) {

        word at = LPW(height[i + 0]);
        word t0 = LPW(ICON1[2*i+1]);
        word t1 = LPW(ICON1[2*i]);
        vm[1+at] = (t0 >> 8);
        vm[2+at] = t0 & 255;
        vm[3+at] = (t1 >> 8);
        vm[4+at] = t1 & 255;
    }

    for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
        setattr(1+j,i,7);
}

// Просто тест
void test2() {

    cls(7+8*0);
    border(0);

    int x = gui.add_window("Norton Commander", 0, 0, 32, 21);

    gui.show(x);
    gui.redraw_panel();

    battr(0,1,31,19,8*0+7);

    block(130,13,130,160,1); block(132,13,132,160,1);
    block(0,12,130,12,1); block(132,12,255,12,1);
    block(0,10,255,10,1);

    gui.button(3,163,32,10,"F3 View",0);
    gui.button(3+36,163,32,10,"F4 Edit",0);
    gui.button(3+2*36,163,32,10,"F5 Copy",0);
    gui.button(3+3*36,163,32,10,"F6 Ren",0);
    gui.button(3+4*36,163,32,10,"F7 MkDr",1);
    gui.button(3+5*36,163,32,10,"F8 Del",0);
    gui.button(3+6*36,163,32,10,"F9 Exit",0);

    block(0,160,255,160,1);

    font.print("Hell Play",1,16,4);

    font.size(11);
}

int main() {

    test1();

    for(;;);
}
