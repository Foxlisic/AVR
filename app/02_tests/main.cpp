#include <avrio.h>
#include <print.h>

int main()
{
    locate(0, 0); color(15, 0);
    print("Start Windows XP Professional...");
    printh(0xF00D, 4);
    return 0;
}
