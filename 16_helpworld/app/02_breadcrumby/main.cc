#include <avrio.h>
#include <qb.cc>
#include "strings.h"
#include "screen.cc"

ISR(INT0_vect) { kb_irq_handler(); }

int main() {

    sei();

    breadlogo();
    splash_1();
    splash_2();

    stop;
}
