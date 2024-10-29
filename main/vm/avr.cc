#include "avr.h"
#include "avr.cpp"

int main(int argc, char** argv) {

    AVR* avr = new AVR(argc, argv);
    return avr->main();
}
