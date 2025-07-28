#include <avrio.h>
#include <c4.h>

int main()
{
    cls();

    read(0);

    heapfs;
    loc(0,0);
    for (int i = 0; i < 512-384; i++)
    {
        color(i & 1 ? 0x07 : 0x02);
        print(fs[i+384], 16, 2);
    }

    stop;
}
