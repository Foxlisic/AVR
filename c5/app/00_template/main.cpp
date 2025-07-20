#include <avrio.h>
#include <c4.h>

STRING(s1,"Press Any Key To Continue...");

int main()
{
    cls();
    loc(0,23); pstr(s1);
    loc(0, 0); for (;;) { tchar(getch()); }
    stop;
}
