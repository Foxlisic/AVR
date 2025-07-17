#include <avrio.cc>

int main() {

    heapvm;

    int j = 0;

    for(;;) {

        j++;
        for (int i = 0; i < 2000; i++) vm[i] = (i * j) >> 8;
    }
}
