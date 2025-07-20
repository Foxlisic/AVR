#include <avrio.h>
#include <c4.h>

int main()
{
    cls();

    char b[16]; input(b,15);
    int i = parseInt(b);
    tchar(10);
    print(i);
    stop;
}
