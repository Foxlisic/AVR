#include <avrio.h>

static const char HE[] = " Hello World! ";

int main() {

    setxy(4, 1);
    int i = 0; while (HE[i]) { putv(HE[i++]); putv(0x70); }
}
