#include <avrio.h>
#include <c4.h>

STRING(s1," Minus Odin ");

int main()
{
    c4.cls(0x07);
    c4.loc(10,12);
    c4.color(0x17);
    c4.pstr(s1);
    stop;
}
