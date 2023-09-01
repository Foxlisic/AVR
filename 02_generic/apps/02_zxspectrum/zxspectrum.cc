class zxspectrum : public Z80 {
public:

    word lookupfb[192];
    word frame_counter;
    byte border_id;
    byte flash_counter, first_sta, flash_state;

    zxspectrum() {

        g.block(0,0,319,199,0);

        // Заполнение таблицы адресов
        for (int y = 0; y < 192; y++) {
            lookupfb[y] = 0x4000 + 32*((y & 0x38)>>3) + 256*(y&7) + 2048*(y>>6);
        }

        flash_counter   = 0;
        first_sta       = 1;
        flash_state     = 0;
        frame_counter   = 0;
    }

    // Интерфейс
    unsigned char mem_read(unsigned int address) {

        if (address < 0x4000)
            return pgm_read_byte(&ROMBIOS[address]);

        if (address > 0xBFFF)
            return 0xFF;

        return zxram[address - 0x4000];
    }

    void mem_write(unsigned int address, unsigned char data) {

        if (address < 0x4000 || address >= 0xBFFF)
            return;

        zxram[address - 0x4000] = data;

        // Обновление char
        if (address < 0x5800) update_charline(address);
        else if (address < 0x5b00) update_attrbox(address);
    }

    unsigned char io_read(unsigned int port) {
        return 0xff;
    }

    void io_write(unsigned int port, unsigned char data) {

        if ((port & 1) == 0) {

            border_id = get_color(data & 7);

            g.block(0,     0, 319,   3,  border_id);
            g.block(0,   196, 319,  199, border_id);
            g.block(0,     0,  31,  199, border_id);
            g.block(288,   0, 319,  199, border_id);
        }
    }

    // Обработка одного кадра http://www.zxdesign.info/vidparam.shtml
    void frame() {

        int req_int = 1;

        dword max_tstates    = 71680;
        dword t_states_cycle = 0;
        word irq_row         = 304;

        // Выполнить необходимое количество циклов
        while (t_states_cycle < max_tstates) {

            // Вызвать прерывание именно здесь, перед инструкцией
            if (t_states_cycle > (irq_row*224 + 8) && req_int) { interrupt(0, 0xff); req_int = 0; }

            // Исполнение инструкции
            t_states_cycle += run_instruction();
        }

        // Мерцающие элементы
        flash_counter++;
        frame_counter++;

        if (flash_counter >= 25 || first_sta) {

            flash_counter = 0;
            first_sta     = 0;
            flash_state   = !flash_state;
        }
    }

    // Обновить 8 бит
    void update_charline(word address) {

        address -= 0x4000;

        byte Ya = (address & 0x0700) >> 8;
        byte Yb = (address & 0x00E0) >> 5;
        byte Yc = (address & 0x1800) >> 11;

        word y = Ya + Yb*8 + Yc*64;
        word x = address & 0x1F;

        byte bytec   = mem_read(0x4000 + address);
        byte attr    = mem_read(0x5800 + x + ((address & 0x1800) >> 3) + (address & 0xE0));
        byte bgcolor = (attr & 0x38) >> 3;
        byte frcolor = (attr & 0x07) + ((attr & 0x40) >> 3);
        byte flash   = (attr & 0x80) ? 1 : 0;
        byte bright  = (attr & 0x40) ? 8 : 0;

        for (byte j = 0; j < 8; j++) {

            byte  pix = (bytec & (0x80 >> j)) ? 1 : 0;

            // Если есть атрибут мерация, то учитывать это
            byte clr = bright | ((flash ? (pix ^ flash_state) : pix) ? frcolor : bgcolor);

            // Вывести пиксель
            g.pset(32 + 8*x + j, 4 + y, get_color(clr));
        }
    }

    // Обновить все атрибуты
    void update_attrbox(word address) {

        address -= 0x1800;
        word addr = (address & 0x0FF) + ((address & 0x0300) << 3);
        for (byte j = 0; j < 8; j++)
            update_charline(0x4000 + addr + (j<<8));

    }

    byte get_color(byte color) {

        switch (color) {
            case 0:  return 0;  // 000
            case 1:  return 1;  // 001
            case 2:  return 4;  // 100
            case 3:  return 5;  // 101
            case 4:  return 2;  // 010
            case 5:  return 3;  // 011
            case 6:  return 6;  // 110
            case 7:  return 7;  // 111

            case 8:  return 0;
            case 9:  return 9;
            case 10: return 12;
            case 11: return 13;
            case 12: return 10;
            case 13: return 11;
            case 14: return 14;
            case 15: return 15;
        }

        return 0;
    };

};
