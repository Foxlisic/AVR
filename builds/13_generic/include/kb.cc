#include <avrio.cc>

class Keyb {

protected:

    byte key_shift, key_spec, key_ctrl;

public:

    Keyb() {

        key_shift = 0;
        key_ctrl  = 0;
        key_spec  = 0;
    }

    byte hit() {
        return inp(STATUS) & 1;
    }

    byte key() {
        return inp(KEYB);
    }

    // Чтение из порта нового символа
    byte inkey() {

        byte k_id = 0;

        // Ожидание нажатия клавиши
        if (hit()) {

            byte xt = key();

            if      (xt == key_LSHIFT         ) key_shift = 1;
            else if (xt == (key_LSHIFT | 0x80)) key_shift = 0;

            // Что-то нажато (не отжато)
            if ((xt & 0x80) == 0) {

                if (key_shift && xt >= 'A' && xt <= 'Z') {
                    xt += ('a'-'A');
                }

                k_id = xt;
            }
        }

        return k_id;
    }

    // Ожидание получения нажатия клавиши
    byte getch() {

        byte k;
        while ((k = inkey()) == 0);
        return k;
    }
};
