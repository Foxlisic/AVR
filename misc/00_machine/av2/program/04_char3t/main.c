#include <avrio.c>
#include <print3.c>
#include <kbd.c>

int main() {

    outp(0x01, 2);
    print(0,0,"Starting OS...", 2);

    for (;;) {
        print_char(getchar(), 1);
    }
}
