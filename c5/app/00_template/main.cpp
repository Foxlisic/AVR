#include <avrio.h>
#include <c4.h>

int main()
{
    cls(0x07);

    for (;;) {

        tchar(getch());
    }

    stop;
}
