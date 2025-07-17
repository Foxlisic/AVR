#include <stdio.h>
#include <stdlib.h>

#include "glut.h"
#include "ansi16.h"
#include "avr.h"
#include "display.cc"
#include "disasm.cc"
#include "vm.cc"

int main(int argc, char* argv[]) { 

    screen();
    assign();
    parsearg(argc, argv);    
    glutMainLoop();
    return 0;
}

