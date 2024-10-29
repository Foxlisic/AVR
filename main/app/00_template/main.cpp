#include <avrio.h>

static const char HE[] = "Hello World!";

int main() {

    int i = 0;
    while (HE[i]) {
        outp(2, HE[i++]);
        outp(2, 0x17);
    }
}
