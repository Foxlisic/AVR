// Очистить весь экран в определенный цвет
void cls(byte color) {

    heapvm;
    for (int i = 0; i < 4000; i += 2) {
        vm[i  ] = 0x00;
        vm[i+1] = color;
    }
}
