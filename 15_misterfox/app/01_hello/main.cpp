#include <avrio.h>
#include <display3.h>
#include <display13.h>

void hello_3() {

    Display3 D;

    D.cls(0x07);
    for (int i = 0; i < 1000; i++) D.print("/\\");

    D.attr = 0x17;
    D.locate(34, 12);
    D.print("HELLO WORLD!");
}

void hello_13() {

    Display13 D;
    D.cls(4);

    for (int y = 0; y < 200; y++)
        D.pset(0, y, 3);
}

int main() {

    hello_13();
}
