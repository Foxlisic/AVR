#include <avrio.h>
#include <c4.h>

int main()
{
    cls();

    for (;;) {
        if (btn(1)) pset(mx(),my(),1);
    }

    stop;
}
