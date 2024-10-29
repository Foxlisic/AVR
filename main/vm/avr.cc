#include "avr.h"
#include "avr.cpp"

int main(int argc, char** argv) {

    AVR* avr = new AVR(argc, argv);
    while (avr->main()) { };
    return avr->destroy();
}
