#include <avrio.h>
#include <print.h>
#include <keyb.h>
#include <sdcard.h>

SDCard sd;

int main() {

    cls(0x07);

    byte data[512];
    sd.read(0, data);

    for (int i = 0; i < 256; i++) {

        int h = data[i] >> 4;
        int l = data[i] & 15;

        cursor.attr = i & 1 ? 0x07 : 0x04;

        prn(h < 10 ? '0' + h : '7' + h);
        prn(l < 10 ? '0' + l : '7' + l);
    }

    prn('0' + sd.error());

    // data[0]++; sd.write(1, data);

    for(;;) { prn( kb_getch()); }
}
