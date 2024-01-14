#include <stdlib.h>

#include "obj_dir/Vga.h"
#include "obj_dir/Vavr.h"
#include "avr.h"

int main(int argc, char **argv) {

    AVR* avr = new AVR(argc, argv);
    avr->fps();
    delete avr;
    return 0;
}
