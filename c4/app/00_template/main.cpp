#include <avrio.h>
#include <c4.h>

STRING(s1," Minus Odin ");

int main()
{
    cls(0x07);
    loc(11,12);
    color(0x17);
    pstr(s1);
    stop;
}
