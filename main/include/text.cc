byte _current_attr;

// Очистка текстового экрана от ненужного
void cls(byte a)
{
    _current_attr = a;

    outp(0, 0);
    setxy(0, 0);

    for (int i = 0; i < 2000; i++) { putv(0); putv(a); }
}

// Установка текущего цвета
void color(byte x)
{
    _current_attr = x;
}

// Пропечать строки
void print(const char* s)
{
    int i = 0;
    while (s[i]) { putv(s[i++]); putv(_current_attr); }
}
