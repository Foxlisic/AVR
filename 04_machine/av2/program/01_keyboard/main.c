#define BREAK asm volatile("break")

#include <avrio.c>
#include <fonts.h>
#include <print.c>

int main() {    

    outp(0x01, 2);

    while (1) {

        while (!(inp(3) & 1));
        int xt = inp(2);
        if (!(xt & 0x80)) {
            
        }
    }

    for(;;);
}
