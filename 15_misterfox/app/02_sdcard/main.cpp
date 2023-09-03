#include <avrio.h>
#include <display.h>
#include <sd.h>

Display D;
SD      sd;
byte    sector[512];

int main() {

    D.cls();

    sd.cmd(SPI_CMD_INIT);
    sd.cmd(SPI_CMD_CE0);

    D.printi(sd.get()); D.prn(' ');
    D.printi(sd.get()); D.prn(' ');

    sd.put(0x40);
    sd.put(0x00);
    sd.put(0x00);
    sd.put(0x00);
    sd.put(0x00);
    sd.put(0x95);

    for (int i = 0; i < 300; i++) { D.printi(sd.get()); D.prn(' '); }
    // D.printi(sd.get()); D.prn(' ');

    //D.printi(sd.SD_command(SD_CMD0, 0));

    // heapvm;
    // sd.read (0, vm);

    for(;;);
}
