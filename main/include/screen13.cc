// 320x200 Bank x=0 или x=1
inline void run(byte x = 0)
{
    outp(0, 2 + x);
    setxy(0, 0);
}

// Очистка текущего экрана от всего [пока что таким Макаром]
void cls(byte a = 0)
{
    setxy(0, 0);
    for (int y = 0; y < 200; y++)
    for (int x = 0; x < 320; x++) {
        putv(a);
    }
}
