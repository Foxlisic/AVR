#include <avrio.h>
#include <c4io.h>

STRING(YouWin,    " Congrats! You - winner! ");
STRING(YouLose,   " You lose this challenge ");
STRING(NoWinner,  " This is nothing, draw... ");

STRING(Selector, "\x00\x01\x10\x0E\x0F\x1F\xE0\xF0\xF1\xFF\xEF\xFE");

byte mate[9];
byte assoc[9];

void load()
{
    io.cls(0);

    // Нарисовать линии
    io.line(32,0,32,49,7);  io.line(49,0,49,49,7);
    io.line(16,16,65,16,7); io.line(16,33,65,33,7);

    for (int i = 0; i < 9; i++) mate[i] = 0;
}

// Проверка на то, кто выиграл по итогу
byte whowin()
{
    for (byte i = 1; i <= 2; i++)
    {
        // Горизонталь
        if (mate[0] == i && mate[1] == i && mate[2] == i) return i;
        if (mate[3] == i && mate[4] == i && mate[5] == i) return i;
        if (mate[6] == i && mate[7] == i && mate[8] == i) return i;

        // Вертикаль
        if (mate[0] == i && mate[3] == i && mate[6] == i) return i;
        if (mate[1] == i && mate[4] == i && mate[7] == i) return i;
        if (mate[2] == i && mate[5] == i && mate[8] == i) return i;

        // Диагональ
        if (mate[0] == i && mate[4] == i && mate[8] == i) return i;
        if (mate[2] == i && mate[4] == i && mate[6] == i) return i;
    }

    return 0;
}

// Нарисовать крестик или нолик
void draw(byte x, byte y, byte t)
{
    mate[x + 3*y] = t ? 1 : 2;

    x *= 17;
    y *= 17;

    if (t) {
        io.line(17+x,1+y,30+x,14+y,2);
        io.line(30+x,1+y,17+x,14+y,2);
    } else {
        io.circle(24+x,7+y,7,4);
    }
}

// Установка выбранной позици
void set(byte x, byte y, byte ch = 5)
{
    x *= 17;
    y *= 17;

    for (byte i = 0; i < sizeof(Selector); i++) {
        byte c = LPM(Selector[i]);
        io.pset(16 + x + (c & 15), y + (c >> 4), ch);
    }
}

// =1 X; =2 O; =3 Ничья
byte game()
{
    byte x = 0, y = 0;

    while (1) {

        byte accept = 0;

        // Выбрать следующее нажатие
        do {

            set(x, y);
            byte c = io.getch();
            set(x, y, 0);

            switch (c)
            {
                case key_RIGHT: x = (x == 2) ? 0 : x + 1; break;
                case key_DOWN:  y = (y == 2) ? 0 : y + 1; break;
                case key_LEFT:  x = (x == 0) ? 2 : x - 1; break;
                case key_UP:    y = (y == 0) ? 2 : y - 1; break;
                case key_ENTER: accept = mate[x+y*3] == 0; break;
            }
        }
        while (accept == 0);

        // Нарисовать
        draw(x, y, 1);

        // Проверить победителя
        if (whowin() == 1) { return 1; }

        // Сколько осталось пустых полей
        byte zerocount = 0;
        byte flag = 0;

        // Просмотреть возможность куда-то поставить нолик
        for (int i = 0; i < 9; i++)
        {
            byte cx = i % 3,
                 cy = i / 3;

            if (mate[i] == 0) {

                // Попробовать поставить нолик. Если выигрышно, установить этот выигрыш
                mate[i] = 2; if (whowin() == 2) { draw(cx, cy, 0); return 2; }

                // Попробовать поставить крестик. Если он выиграет, то поставить туда нолик
                mate[i] = 1; if (whowin() == 1) { draw(cx, cy, 0); flag = 1; break; }

                // Вернуть назад, если нет выигрыша
                mate[i] = 0;
            }
        }

        // Пересчитать оставшиеся поля
        for (int i = 0; i < 9; i++) {
            if (mate[i] == 0) {
                assoc[zerocount++] = i;
            }
        }

        // Ничья
        if (zerocount == 0) {
            return 3;
        }

        // Нет явно выигрышных ситуации, поставить куда угодно
        if (flag == 0) {

            byte rand = assoc[millis() % zerocount];
            byte cx   = rand % 3,
                 cy   = rand / 3;

            mate[rand] = 2;
            draw(cx, cy, 0);
        }
    }

    return 0;
}

int main()
{
    for (;;) {

        load();
        byte fin = game();

        io.at(30, 12);

        if      (fin == 1) { io.color(0x17); io.print(YouWin);   }
        else if (fin == 2) { io.color(0x47); io.print(YouLose);  }
        else if (fin == 3) { io.color(0x70); io.print(NoWinner); }

        io.getch();
    }
    return 0;
}
