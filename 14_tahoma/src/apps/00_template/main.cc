#include <avrio.h>
#include <print.h>

ISR(INT0_vect) { }

int main() {

    sei();
    cls(7);
    print("Hello, world");
    for(;;);
}
