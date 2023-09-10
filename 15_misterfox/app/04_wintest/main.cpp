#include <avrio.h>
#include <displayzx.h>

DisplayZX zx;

int main() {

    zx.cls(0x07);

    //zx.attrb(1,1,18,1,1*8+7+0x40);
    zx.attrb(1,1,18,1,1*8+7);
    zx.attrb(1,2,18,10,0x38);
    zx.loc(8,10);

    zx.print4("# Googlenote");


    for(;;);
}
