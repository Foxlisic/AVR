#include <graphics.cc>

Graphics g;

int main() {

    g.start();

    g.line(0,0,319,199,2);
    g.circle(160,100,50,10);
    g.circle_fill(160,100,45,3);
    g.cursor(121,93)->print("Коты рулят");

    for(;;);
}
