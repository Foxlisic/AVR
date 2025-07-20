#include <avrio.h>
#include <c4.h>

STRING(s1,"Ugadai Chislo!\nZagadal ot 1 do 1000? (Y/n): ");
STRING(s2,"I'm Winner!");

int main()
{
    char u[4];

    cls();
    pstr(s1);
    input(u,1);
    tchar(10);

    int a = 1;
    int b = 1000;

    for(;;) {

        int c = (a + b) / 2;

        // Выдача числа на экране и ожидание
        print(c); print("? "); input(u,1); tchar(10);

        if (u[0] == 'n') {
            a = c;
        } else if (u[0] == 'p') {
            b = c;
        } else {
            break; // Wakh!
        }
    }

    pstr(s2);
    stop;
}
