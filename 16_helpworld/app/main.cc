#include <avrio.h>

void test(const char* s) {

    heapvm;
    int i = 0;
    while (s[i]) {
        vm[2*(33+11*80)+2*i] = s[i];
        vm[2*(33+11*80)+2*i+1] = 0x70;
        i++;
    }
}

int main() {

    test(" Help, world! ");
    for(;;);
}
