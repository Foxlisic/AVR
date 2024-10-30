#include <avrio.h>
#include <text.cc>

int main()
{
    cls(0x17);
    color(0x70);
    setxy(4, 1);
    print(" Hello world! ");
}
