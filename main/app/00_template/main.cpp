#include <avrio.h>
#include <text.cc>

int main()
{
    cls(0x17);
    color(0x70);
    cursor(2, 1);
    print(" Hello world! ");
    return 0;
}
