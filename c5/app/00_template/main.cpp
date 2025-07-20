#include <avrio.h>
#include <c4.h>

int main()
{
    cls();
    for (;;) { tchar(getch()); }
    stop;
}
