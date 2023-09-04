
// Объявить о наличии KbATnT
extern const byte KbATnT[] PROGMEM;

/*
 * 1  ^  TOP
 * 2  -> RIGHT
 * 3  v  DOWN
 * 4  <- LEFT
 * 5  Home
 * 6  End
 * 7  Caps Lock
 * 8  BkSpc
 * 9  Tab
 * 10 Enter
 * ----------------------------------
 * 11-F1  | 14-F4  | 17-F7  | 20-F10
 * 12-F2  | 15-F5  | 18-F8  | 21-F11
 * 13-F3  | 16-F6  | 19-F9  | 22-F12
 * ----------------------------------
 * 23 PgUp
 * 24 PgDn
 * 25 Del
 * 26 Ins
 * 27 Esc
 * 28 Win
 * 29 Scroll
 * 30 NumLock
 * */

const byte KbATnT[] =
{
// 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
  0x00,0x13,' ',0x0F,0x0D,0x0B,0x0C,0x16, ' ',0x14,0x12,0x10,0x0E,0x09, '`', ' ', // 0
  ' ', ' ', ' ', ' ', ' ', 'q', '1', ' ', ' ', ' ', 'z', 's', 'a', 'w', '2', ' ', // 1
  ' ', 'c', 'x', 'd', 'e', '4', '3', ' ', ' ',0x20, 'v', 'f', 't', 'r', '5', ' ', // 2
  ' ', 'n', 'b', 'h', 'g', 'y', '6', ' ', ' ', ' ', 'm', 'j', 'u', '7', '8', ' ', // 3
  ' ', ',', 'k', 'i', 'o', '0', '9', ' ', ' ', '.', '/', 'l', ';', 'p', '-', ' ', // 4
  ' ', ' ', '\'',' ', '[', '=', ' ', ' ',0x07, ' ',0x0A, ']', ' ', '\\',' ', ' ', // 5
  ' ', ' ', ' ', ' ', ' ', ' ',0x08, ' ', ' ', '1', ' ', '4', '7', ' ', ' ', ' ', // 6
  '0', '.', '2', '5', '6', '8',0x1B,0x1E,0x15, '+', '3', '-', '*', '9',0x1D, ' ', // 7
  // ---------------------------------------------------------------------------
    0,   0,   0,0x11,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 8
  ' ', ' ', ' ', ' ', ' ', 'Q', '!', ' ', ' ', ' ', 'Z', 'S', 'A', 'W', '@', ' ', // 9
  ' ', 'C', 'X', 'D', 'E', '$', '#', ' ', ' ', ' ', 'V', 'F', 'T', 'R', '%', ' ', // A
  ' ', 'N', 'B', 'H', 'G', 'Y', '^', ' ', ' ', ' ', 'M', 'J', 'U', '&', '*', ' ', // B
  ' ', ',', 'K', 'I', 'O', ')', '(', ' ', ' ', '.', '?', '!', ':', 'P', '_', ' ', // C
  ' ', ' ', '"', ' ', '{', '+', ' ', ' ', ' ', ' ', ' ', '}', ' ', '|', ' ', ' ', // D
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '!', ' ', '$', '&', ' ', ' ', ' ', // E
  ')', '>', '@', '%', '^', '*', ' ', ' ', ' ', '+', '#', '_', '*', '(', ' ', ' ', // F
};

class Keyboard {
protected:

    volatile byte release, st, ed, spec, shift;
    volatile byte queue[16];

public:

    Keyboard() { release = st = ed = spec = shift = 0; }

    // Разобрать клавишу AT&T -> очередь FIFO
    void recv() {

        byte in = inp(KEYB);

        if      (in == 0xE0) spec    = 1; // Специальная клавиша
        else if (in == 0xF0) release = 1; // Признак отпущенной клавиши
        else {

            // Нажатие на шифт
            if (spec == 0 && (in == 0x12 || in == 0x59)) {
                shift = release ? 0 : 1;
            }
            // Нажатая клавиша обычного набора
            else if (release == 0 && spec == 0) {

                queue[ed] = pgm_read_byte(& KbATnT[in | (shift ? 0x80 : 0)]);
                ed = (ed + 1) & 15;
            }

            spec = release = 0;
        }
    }

    // Взять символ из очереди
    byte get() {

        if (st != ed) {

            byte ch = queue[st];
            st = (st + 1) & 15;
            return ch;
        }

        return 0;
    }
};