class Display
{
protected:

    int     cursor;
    byte    dcolor;

public:

    // Очистить монитор
    void cls(byte attr = 0x07) {

        heapvm;

        cursor = 0;
        dcolor = attr;

        for (int i = 0; i < 4000; i += 2) {

            vm[i]   = 0x00;
            vm[i+1] = attr;
        }
    }

    // Печать строки
    void print(const char* s) {

        heapvm;
        int i = 0;
        while (s[i]) {

            vm[cursor]   = s[i];
            vm[cursor+1] = dcolor;

            cursor += 2; i++;
        }
    }
};
