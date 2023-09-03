#include <avrio.h>
#include <display.h>
#include <sd.h>

Display D;
SD      sd;
byte    sector[512];

int main() {

    D.cls();

    heapvm;
    sd.read (0, vm);

    for(;;);
}
