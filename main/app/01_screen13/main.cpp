#include <avrio.h>
#include <screen13.cc>

int main()
{
    int c = 0;
    for (;;) {

        run();
        for (int y = 0; y < 200; y++)
        for (int x = 0; x < 320; x++) {
            putv(x + y + c);
        }
        c++;
    }

    return 0;
}
