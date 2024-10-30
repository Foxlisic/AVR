void cls(byte a)
{
    outp(0,  2); // 320x200 Bank 0
    setxy(0, 0);

    for (int y = 0; y < 200; y++)
    for (int x = 0; x < 320; x++) {
        putv(a);
    }
}
