#include <avrio.h>
#include <display.h>

Display D;

int main() {

    D.cls(0x17);
    for(;;);
}
