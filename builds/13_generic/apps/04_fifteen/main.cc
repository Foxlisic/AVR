#include <graphics.cc>
#include <kb.cc>

Graphics g;
Keyb     kb;

class Game {
protected:

    byte map[4][4];
    int  step = 0;

public:

    Game() {

        g.start();

        init();

        kb.getch();
        shuffle();

        for (;;) {

            int key = kb.getch();
            int dx = 0, dy = 0;

            g.cursor(8,8)->color(14)->wiped(0x11)->printint(++step);

            switch (key) {

                case key_LF: dx = -1; dy =  0; break;
                case key_RT: dx =  1; dy =  0; break;
                case key_UP: dx =  0; dy = -1; break;
                case key_DN: dx =  0; dy =  1; break;
            }

            if (dx || dy) move(dx, dy);
        }
    }

    // Инициализация игрового поля
    void init() {

        // Расставить фишки
        for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {

            map[i][j] = (i == 3 && j == 3) ? 0 : 1 + 4*i + j;
            draw(i, j);
        }
    }

    // Отрисовать новую позицию
    void draw(int i, int j) {

        int x1 = 80 + j*40,
            x2 = 80 + j*40 + 38,
            y1 = 20 + i*40,
            y2 = 20 + i*40+38,
            n  = map[i][j];

        g.block(x1, y1, x2, y2, n ? 7 : 0);
        g.wiped(0);

        if (n) {

            g.block(x1, y1, x2, y2, 7);
            g.lineb(x1, y1, x2, y2, 15);
            g.lineb(x2, y1, x2, y2, 8);
            g.lineb(x1, y2, x2, y2, 8);
            g.cursor(x1 + (n < 10 ? 16 : 12), y1 + 12)->color(0);
            g.printint(n);
        }
    }

    // Перемешать
    void shuffle() {

        unsigned long r = TIMERD;
        for (int i = 0; i < 256; i++) {

            int dx = (r % 3) - 1; r = (r*1103515245) + 12345;
            int dy = (r % 3) - 1; r = (r*1103515245) + 12345;
            move(dx, dy);
        }
    }

    // Двинуть фишку
    void move(int dx, int dy) {

        // Найти местоположение дырки
        int x = 0, y = 0;
        for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            if (map[i][j] == 0) { x = j; y = i; break; }
        }

        int nx = x - dx,
            ny = y - dy;

        // Ничего не перемещать
        if (nx < 0 || ny < 0 || nx > 3 || ny > 3)
            return;

        map[y][x] = map[ny][nx];
        map[ny][nx] = 0;

        draw(y,  x);
        draw(ny, nx);
    }

};

int main() { Game game; }
