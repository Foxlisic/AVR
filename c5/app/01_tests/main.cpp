#include <avrio.h>
#include <c4.h>

int main()
{
    cls();
    for (int y = 0; y < 192; y++)
    for (int x = 0; x < 256; x++)
        pset(x,y,x&y);

    line(0,0,255,191,1);

    stop;
}
