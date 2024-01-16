#include <avrio.h>
#include <qb.cc>

ISR(INT0_vect) {
    prn(inp(0));
}

int main() {

    cls(0x07);
    print("Декларе ундертале\nЛИЦЕНЗИЯ NO WARRANTY ИСПОЛЬЗОВАНИЕ\nСтив крутится вокруг своей оси, почитывая Котячью Свободную Лицензию.");
    sei();
    for(;;);
}
