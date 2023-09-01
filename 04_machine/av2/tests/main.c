#include <avrio.c>
#include <fonts/ansi3.h>
#include <graphics.c>
#include <line.c>

int main() {

    screen3();
    
    cls(3);
    block(10, 10, 300, 100, 7);    
    lineb(10, 10, 300, 100, 0);
    block(12, 12, 298, 22, 1);

    for(;;);
}

