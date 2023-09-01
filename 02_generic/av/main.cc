#include "avr.cc"
#include "core.cc"
#include "io.cc"
#include "disasm.cc"

int main(int argc, char** argv) {

    AVR* avr = new AVR(argc, argv);

    while (int signal = avr->main()) {
        avr->handle_event(signal);
    }

    return avr->destroy();
}
