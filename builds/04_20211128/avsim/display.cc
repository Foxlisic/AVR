// ---------------------------------------------------------------------
// Процедуры для работы с выводом текста
// ---------------------------------------------------------------------

// Обработчик окна при вызове таймера
void display()  {
    
    if (cpu_halt) {
    
        disassembly();   
        
    } else {       
        
        // 20 x N IPS
        for (int i = 0; i < 5000; i++) {            
            step();
            if (cpu_halt) { pc--; ds_cursor = pc; break; }
        }
    }
    
    displayout();
    update();
}

// Запустить экран
int screen() {

    int   argc = 0;
    char* argv[1];

    // Инициализация переменных
    app_width       = 900; 
    app_height      = 400; 
    app_factor      = 1;        
    app_interval    = 50;

    // Запустить окно
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(app_width, app_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("AVR Emulator Win32");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    // Выделить необходимую область памяти
    app_membuf = (unsigned char*) malloc(app_width * app_height * 4);
    
    glutDisplayFunc(display);
    glutSpecialFunc(ds_keydown_spec);
    glutKeyboardFunc(ds_keydown);
 
    timer_function(0);   
    
    return 0;
}

void ds_keydown_spec(int key, int x, int y) {
    
    switch (key) {
        
        // Один шаг отладчика
        case GLUT_KEY_F7: 
        
            step();
            ds_cursor = pc;
            break;     

        // Запуск
        case GLUT_KEY_F9:
        
            cpu_halt = !cpu_halt;
            ds_cursor = pc;
            
            // Притушить отладчик
            if (cpu_halt == 0) {
                for (int i = 0; i < 800; i++)
                for (int j = 0; j < 800; j += 2)
                    pset(j + (i%2), i, 0);
            }
            
            break;
    }
    
    //printf("=%d ", key);
}

void ds_keydown(unsigned char key, int x, int y) {
    printf(">%d ", key);
}

// Обработчик таймера
void timer_function(int value) {

    glutPostRedisplay();
    glutTimerFunc(app_interval, timer_function, 0);
}

// Преобразовать цвет в DOS
int dac(int c) {
    return doscolortable[c & 255];
}

// Преобразовать в RGB
int rgb(unsigned char r, unsigned char g, unsigned char b) {
    return r*65536 + g*256 + b;
}

// Нарисовать точку
void pset(int x, int y, unsigned int cl) {

    x *= app_factor;
    y *= app_factor;

    if (x < 0 || y < 0 || x >= app_width || y >= app_height)
        return;

    for (int i = 0; i < app_factor; i++)
    for (int j = 0; j < app_factor; j++) {

        int cursor = 4*(j+x + (app_height-1-i-y)*app_width);
        app_membuf[cursor++] = cl>>16;
        app_membuf[cursor++] = cl>>8;
        app_membuf[cursor++] = cl;
        app_membuf[cursor++] = 255;
    }
}

// Получение точки с учетом factor
int point(int x, int y) {

    x *= app_factor;
    y *= app_factor;

    int cursor = 4*(x + (app_height-1-y)*app_width);

    unsigned char r = app_membuf[cursor  ];
    unsigned char g = app_membuf[cursor+1];
    unsigned char b = app_membuf[cursor+2];

    return (r*65536 + g*256 + b);
}

// Обновить экран
void update() {

    glRasterPos2i(-1, -1);
    glDrawPixels(app_width, app_height, GL_RGBA, GL_UNSIGNED_BYTE, app_membuf);
    glutSwapBuffers();
}

// Сохранение изображения в PPM
void saveppm(const char* filename) {

    char temp[256];

    int w = app_width  / app_factor;
    int h = app_height / app_factor;

    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) exit(2);

    sprintf(temp, "P6\n%d %d\n255\n", w, h);
    fputs(temp, fp);

    for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {

        int p = point(x, y);

        temp[0] = p>>16;
        temp[1] = p>>8;
        temp[2] = p;

        fwrite(temp, 1, 3, fp);
    }

    fclose(fp);
}

// Очистить экран в цвет
void cls(long color) {

    for (int i = 0; i < app_height; i++)
    for (int j = 0; j < app_width; j++)
        pset(j, i, color);
}

// Рисование блока
void block(int x, int y, int w, int h, long k) {
    
    for (int i = y; i <= y + h; i++)
    for (int j = x; j <= x + w; j++)
        pset(j, i, k);
}

// Печать на экране Char
void print_char(int col, int row, unsigned char ch, long cl) {

    col *= 8;
    row *= 16;

    for (int i = 0; i < 16; i++) {

        unsigned char hl = ansi16[ch][i];
        for (int j = 0; j < 8; j++) {
            if (hl & (1<<(7-j)))
                pset(j + col, i + row, cl);
        }
    }
}

// Печать строки
void print(int col, int row, const char* s, long cl) {

    int i = 0;
    while (s[i]) {
        
        print_char(col, row, s[i], cl); 
        col++; 
        i++; 
    }
}

// ---------------------------------------------------------------------
// Работа с виртуальным экраном
// ---------------------------------------------------------------------

// Экран как ZX Spectrum
void displayout() {
    
    int colortable[16] = {0, 1, 4, 5, 2, 3, 6, 7, 0, 9, 12, 13, 10, 11, 14, 15};

    for (int y = 0; y < 192; y++)
    for (int x = 0; x < 256; x++) {
        
        int data = sram[0x4000 + (x>>3) + ((y&0x38)<<2) + (y&7)*256 + ((y&0xc0)<<5) ];
        int attr = sram[0x5800 + (x>>3) + (y>>3)*32];
        
        int cl = data & (1 << ((7 - x) & 7)) ? 1 : 0;                
        cl = cl ^ ((attr & 0x80) && flash ? 1 : 0) ? attr & 7 : (attr & 0x38)>>3;
        cl = attr & 0x40 ? (8 + cl) : cl;

        pset(900-4-256+x,4+y, dac(colortable[cl]));
    }
    
    fcounter++; if (fcounter > 12) { fcounter = 0; flash = !flash; }
}
