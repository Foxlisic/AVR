#include <avrio.h>
#include <c4.h>

STRING(s1,"Press Any Key To Continue...");

int main()
{
    cls(); loc(0,23); pstr(s1); stop;
}
