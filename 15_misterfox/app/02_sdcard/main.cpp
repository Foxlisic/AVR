#include <avrio.h>
#include <sd.h>

SD   sd;
byte sector[512];

int main() {

    sd.read (0, sector);
    sd.write(0, sector);

    for(;;);
}
