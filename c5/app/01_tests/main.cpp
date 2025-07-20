#include <avrio.h>
#include <c4.h>

int main()
{
    char b[16];

    cls();

    input(b,16);
    loc(0,1); print(b); tchar('.');

    stop;
}
