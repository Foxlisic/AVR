#include <avrio.h>
#include <gui.h>

STRING(s_hello, "Hello, World!");
STRING(s_iface, "Windows 98 meets you! Welcome to AVR OS");
STRING(s_start, "\x8F\xB3\xB1\xAA");

/*
- Календарь
- Змейка, сокобан, сапер
- Редактор шрифтов
- Калькулятор
- Фейловый манагер
- Будильник
- Блокнот
*/

int main()
{
    gui.cls(1,3);

    gui.block(32,32,320,100,7);
    gui.block(33,33,319,46,1);
    gui.lineb(31,31,321,101,0);
    gui.block(33,48,319,99,15);
    gui.block(33,48,33,99,8);
    gui.block(33,48,319,48,8);

    // Пуск
    gui.block(0,375,639,399,7);
    gui.block(0,376,639,376,15);
    gui.block(64,380,64,396,15);
    gui.block(63,380,63,396,8);

    gui.button(3,379,58,397,0);
    gui.print(24,383,s_start,0,1);

    gui.print(36,35,s_hello,15);
    gui.print(36,52,s_iface,0);

    for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
        gui.block(6+7*j,382+7*i,6+7*j+5,382+7*i+5,i+j*2+1);

    return 0;
}
