#include <avrio.h>
#include <display.h>

Display D;

int main() {

    D.cls(0x07);
    D.print("Hello World");
    for(;;);
}
