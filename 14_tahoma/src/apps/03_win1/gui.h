#include <fonts.h>

#define MAX_WINDOWS 4

fonts font;

enum WIN_ATTR {
    win_title2x = 1,
    win_border  = 2
};

struct cwin {

    byte x, y, w, h, attr;
    byte active;            // Загружено в память
    byte select;            // Выбрано в панели
    char name[32];
};

// -----------------------------------------------------------------------------

class GUI {

protected:

    struct cwin windows[MAX_WINDOWS];

public:

    GUI() {

        // Установить всем 0
        for (int i = 0; i < MAX_WINDOWS; i++) {
            windows[i].active = 0;
            windows[i].select = 0;
        }
    }

    // Добавить окно в систему
    // attr[0] если =1, то 16 пикселей заголовок
    int add_window(const char* name, byte x, byte y, byte w, byte h, byte attr = 0) {

        for (int i = 0; i < MAX_WINDOWS; i++) {

            // Добавить окно в систему
            if (windows[i].active == 0) {

                windows[i].active = 1;
                windows[i].x = x;
                windows[i].y = y;
                windows[i].w = w;
                windows[i].h = h;
                windows[i].attr = attr;

                strcpy(windows[i].name, name);

                return i;
            }
        }

        return -1;
    }

    // Рисовать окно (по знакоместам)
    void show(int handle) {

        cwin* win = & windows[handle];

        byte ht = (win->attr & 1) ? 16 : 8;
        byte w  = win->w;
        byte h  = win->h + ((win->attr & 1) ? 2 : 1);

        byte x1 = 8*win->x,
             y1 = 8*win->y;

        byte x2 = x1 + 8*w - 1,
             y2 = y1 + 8*h - 1;

        // Заголовок окна
        block(x1, y1, x2, y1+ht-1, 0);

        for (int i = win->x; i < win->x + w; i++) {
            setattr(i, win->y, 8*1+7+0x40);
            if (win->attr & 1) setattr(i, win->y+1, 8*1+7);
        }

        // Рабочая область
        block(x1, y1 + ht, x2, y2, 0);

        // Бордер у окна
        if (win->attr & win_border) {

            block(x1, y1+ht, x1, y2, 1);
            block(x1, y2,    x2, y2, 1);
            block(x2, y1+ht, x2, y2, 1);
        }

        for (int i = (win->attr & 1) ? 2 : 1; i < h; i++)
        for (int j = 0; j < w; j++) {
            setattr(win->x + j, win->y + i, 0x38);
        }

        // Иконка закрытия окна
        if ((win->attr & 1)) {

            for (int i = 1; i < 3; i++) {
                setattr(win->x+w-i, win->y,   8*2+7+0x40);
                setattr(win->x+w-i, win->y+1, 8*2+7);
            }

            // Кнопка X
            line(x2-11, y1+5,  x2-5, y1+11, 1);
            line(x2-10, y1+5,  x2-4, y1+11, 1);
            line(x2-11, y1+11, x2-5, y1+5,  1);
            line(x2-10, y1+11, x2-4, y1+5,  1);

            // Написать буквами слова
            font.size(11);
            font.locate(x1 + 3, y1 + 3);

        } else {

            setattr(win->x+w-1, win->y, 8*2+7+0x40);
            font.locate(x2 - 5, y1 + 0); font.prn4('x');

            font.size(4);
            font.locate(x1 + 1, y1 + 0);
        }

        font.print(win->name);

        // Выбрать текущее, скрыть остальные
        for (int i = 0; i < MAX_WINDOWS; i++)
            windows[i].select = (handle == i) ? 1 : 0;
    }

    // Рисовать нижнюю панель
    void redraw_panel() {

        byte win_x = 40;

        block(0, 192-15, 255, 191, 0);
        battr(0, 22, 31, 23, 0x38);
        block(0, 192-16, 255, 192-16, 1);

        font.size(11);

        // Кнопка "Пуск"
        button(2, 178, 32, 12, "Start!", 0);

        // Разделитель
        for (int i = 0; i < 14; i += 2) pset(37, i + 178, 1);

        // Нарисовать имена
        font.size(4);

        for (byte i = 0; i < MAX_WINDOWS; i++) {

            byte j = 0;
            cwin* win = & windows[i];

            if (win->active) {

                button(win_x, 178, 48, 12, 0, win->select);

                // Установка позиции
                font.locate(win_x+3 + win->select, 178+3 + win->select);

                // Вывести имя окна, не более 9 символов
                while (win->name[j] && j < 9) { font.prn4(win->name[j]); j++; }
                if (j == 9) font.print("..");

                win_x += 50;
            }
        }
    }

    // Отрисовать кнопку
    void button(byte x, byte y, byte w, byte h, const char* name = 0, byte active = 0) {

        block(x, y, x + w, y + h, 0);

        if (active) {

            block(x, y, x + w, y,     1);
            block(x, y, x,     y + h, 1);

            for (int i = 1; i < w; i += 2) pset(x + i, y + 1, 1);
            for (int i = 1; i < h; i += 2) pset(x + 1, y + i, 1);

            if (name) { font.locate(x + 3, y + 3); font.print(name); }

        } else {

            block(x + w, y, x + w, y + h, 1);
            block(x, y + h, x + w, y + h, 1);

            if (name) { font.locate(x + 2, y + 1); font.print(name); }
        }
    }

    void hscroller(byte x, byte y, byte h) {

        heapvm;

        byte x1 = x * 8;
        byte y1 = y * 8;
        byte y2 = y1 + 8*h;

        block(x1, y1+1, x+6, y2-2, 0);
        battr(x, y, x, y+h-1, 0x38+0x40);

        // Подложка
        for (byte _y = y1 + 8; _y < y2-8; _y++)
        for (byte _x = x1 + (_y & 1); _x < x1 + 8; _x += 2)
            pset(_x, _y, 1);

        // Ободки кнопки
        block(x1, y1+7, x1+7, y1+7, 1);
        block(x1, y2-8, x1+7, y2-8, 1);

        // Кнопка "вверх"
        setattr(x, y, 0x38);
        vm[ LPW(height[y1+3]) + x ] = 0x18;
        vm[ LPW(height[y1+4]) + x ] = 0x3C;

        // Кнопка "вниз"
        setattr(x, y+h-1, 0x38);
        vm[ LPW(height[y2-4]) + x ] = 0x18;
        vm[ LPW(height[y2-5]) + x ] = 0x3C;

        // Сам ползунок
        block(x1, y1+11, x1+7, y1+14, 0);
        lineb(x1, y1+11, x1+7, y1+14, 1);
    }
};
