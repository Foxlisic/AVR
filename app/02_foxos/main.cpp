#include <avrio.h>
#include <gui.h>

STRING(s_hello, "Hello, World!");
STRING(s_iface, "Windows 98 meets you! Welcome to AVR OS");

int main()
{
    gui.cls(3);

    gui.block(32,32,320,100,7);
    gui.block(33,33,319,46,1);
    gui.lineb(31,31,321,101,0);
    gui.block(33,48,319,99,15);
    gui.block(33,48,33,99,8);
    gui.block(33,48,319,48,8);

    gui.print(36,35,s_hello,15);
    gui.print(36,52,s_iface,0);

    return 0;
}
