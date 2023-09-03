// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// RUS Портативный 16-битный процессор x86
// ENG Portalble 16 bit processor x86
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// (C)opyleft The Pont Silver Edition (R)esearch
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define u8  unsigned char
#define u16 unsigned short int
#define u32 unsigned long long
#define s8  signed char
#define s16 signed short int
#define s32 signed long long

static const char parity_bits[256] = {
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
      1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

enum SEGNAMES { _ES = 0, _CS = 1, _SS = 2, _DS = 3 };
enum REGNAMES { _AX = 0, _CX = 1, _DX = 2, _BX = 3, _SP = 4, _BP = 5, _SI = 6, _DI = 7 };
enum FLAGBITS { CF = 1, PF = 4, AF = 0x10, ZF = 0x40, SF = 0x80, TF = 0x100, IF = 0x200, DF = 0x400, OF = 0x800 };

class K86 {

protected:

    u16 regs[8];
    u16 segs[4];
    u16 ip;
    u16 flags;
    u8  opcode;
    u8  modrm, mod, reg, rm, rep, halt;
    u8  overseg, size;
    u16 segment_id;
    u16 ea;

public:

    K86() { reset(); }

    // Прототипы для общения с "внешним миром"
    // -----------------------------------------------------------------------------
    virtual u8   read(u32 address) { return 0xFF; }   // Чтение из памяти
    virtual u8   pin(u16 address)  { return 0xFF;  }  // Чтение из порта
    virtual void write(u32 address, u8 data) { }      // Запись в память
    virtual void pout(u16 address, u8 data) { }       // Запись в порт
    // -----------------------------------------------------------------------------

    // Сброс процессора
    void reset() {

        halt = 0;

        segs[_CS] = 0xF000;
        segs[_DS] = 0x0000;
        segs[_ES] = 0x0000;
        segs[_SS] = 0x0000;

        ip = 0xF000;
    }

    u8 halted() { return halt; }

    // === Считывание и запись ===

    // Прочитать байт из потока
    u8 fetchb() {
        return read(16*segs[_CS] + (ip++));
    }

    // Прочитать 16-битное слово
    u16 fetchw() {

        u8 lb = fetchb();
        return lb + 256*fetchb();
    }

    // Чтение из памяти
    u8  readb(u16 _seg, u16 _ea) { return read(16*segs[_seg] + _ea); }
    u16 readw(u16 _seg, u16 _ea) { return readb(_seg, _ea) + 256*readb(_seg, _ea + 1); }

    // Запись в память
    void writeb(u16 _seg, u16 _ea, u8 data)  { write(16*segs[_seg] + _ea, data); }
    void writew(u16 _seg, u16 _ea, u16 data) { writeb(_seg, _ea, data); writeb(_seg, _ea + 1, data >> 8); }

    // === Работа с байтом MODRM ===

    // Разбор байта modrm
    void getmodrm() {

        modrm =  fetchb();
        mod   =  modrm >> 6;
        reg   = (modrm >> 3) & 7;
        rm    =  modrm & 7;
        ea    =  0;

        // Расчет индекса
        switch (rm) {

            case 0: ea = (regs[_BX] + regs[_SI]); break;
            case 1: ea = (regs[_BX] + regs[_DI]); break;
            case 2: ea = (regs[_BP] + regs[_SI]); break;
            case 3: ea = (regs[_BP] + regs[_DI]); break;
            case 4: ea =  regs[_SI]; break;
            case 5: ea =  regs[_DI]; break;
            case 6: ea =  regs[_BP]; break;
            case 7: ea =  regs[_BX]; break;
        }

        // В случае если не segment override
        if (!overseg && ((rm == 6 && mod) || (rm == 2) || (rm == 3))) {
            segment_id = _SS;
        }

        // Модифицирующие биты modrm
        switch (mod) {

            case 0: if (rm == 6) ea = fetchw(); break;
            case 1: ea += (signed char) fetchb(); break;
            case 2: ea += fetchw(); break;
            case 3: ea = 0; break;
        }
    }

    // Получение значения регистра 8 бит по номеру
    u8 get8(u8 id) {

        u16 r = regs[id & 3];
        return id & 4 ? r >> 8 : r;
    };

    // Запись в регистр
    void put8(u8 id, u8 val) {

        if (id & 4) regs[id & 3] = (regs[id & 3] & 0x00FF) | (val << 8);
        else        regs[id & 3] = (regs[id & 3] & 0xFF00) | (val);
    }

    // Сохранение в REG-часть MODRM
    u16  get_reg() { return size ? regs[reg] : get8(reg); }
    void put_reg(u16 data) { if (size) regs[reg] = data; else put8(reg, data); }

    // Получение RM-части
    u16 get_rm() {

        if (mod == 3) {
            return size ? regs[rm] : get8(rm);
        } else {
            return size ? readw(segment_id, ea) : readb(segment_id, ea);
        }
    }

    // Сохранение RM-части
    void put_rm(u16 data) {

        if (mod == 3) {
            if (size) regs[rm] = data;
            else      put8(rm, data);
        } else {
            if (size) writew(segment_id, ea, data);
            else      writeb(segment_id, ea, data);
        }
    }

    // === Модуль АЛУ ===

    // Сложение и вычитание операндов
    u16 do_add(u16 a, u16 b) {

        u32 r = (u32)a + (u32)b;
        flags_arith(a, b, r, 0);
        return size ? r : r & 255;
    }

    u16 do_adc(u16 a, u16 b) {

        u32 r = (u32)a + (u32)b + (u32)(flags & 1);
        flags_arith(a, b, r, 0);
        return size ? r : r & 255;
    }

    u16 do_sub(u16 a, u16 b) {

        u32 r = (u32)a - (u32)b;
        flags_arith(a, b, r, 1);
        return size ? r : r & 255;
    }

    u16 do_sbb(u16 a, u16 b) {

        u32 r = (u32)a - (u32)b - (u32)(flags & 1);
        flags_arith(a, b, r, 1);
        return size ? r : r & 255;
    }

    // Логические
    u16 do_and(u16 a, u16 b) {

        u16 r = a & b;
        flags_logic(r);
        return size ? r : r & 255;
    }

    u16 do_or(u16 a, u16 b) {

        u16 r = a | b;
        flags_logic(r);
        return size ? r : r & 255;
    }

    u16 do_xor(u16 a, u16 b) {

        u16 r = a ^ b;
        flags_logic(r);
        return size ? r : r & 255;
    }

    // Флаги после арифметических операции
    void flags_arith(u16 a, u16 b, u32 r, u8 _sub) {

        flags &= 0x700;
        u16 usize = size ? 0x8000 : 0x80;

        // AF=1 Для всех одинаково
        if ((a ^ b ^ r) & 0x10) flags |= AF;

        // CF=1 Для вычитания и сложения одинаково
        if ((size && (r & (u32)0x10000)) || (!size && (r & 0x100))) flags |= CF;

        // OF=1 Для вычитания не будет реверса старшего бита
        if (((a ^ b ^ (_sub ? 0 : usize)) & (a ^ r)) & usize) flags |= OF;

        // Флаги SF, ZF, PF
        do_szp(r);
    }

    // OF=0, CF=0, AF=0
    void flags_logic(u16 r) {

        flags &= 0x700;
        do_szp(r);
    }

    // Установка флагов S,Z,P, очистка 5,3 и установка 1-го
    void do_szp(u16 r) {

        flags &= 0xF11; // 0001 0001
        flags |= parity_bits[r & 255] ? (PF | 0x02) : 0x02;

        if (size) {

            if (r & 0x8000) flags |= SF;
            if (!r) flags |= ZF;

        } else {

            if (r & 0x80) flags |= SF;
            if (!(r & 255)) flags |= ZF;
        }
    }

    // Исполнение группы инструкции
    u16 do_group_alu(u16 a, u16 b, u8 mode) {

        switch (mode & 7) {

            case 0: return do_add(a, b);
            case 1: return do_or(a, b);
            case 2: return do_adc(a, b);
            case 3: return do_sbb(a, b);
            case 4: return do_and(a, b);
            case 6: return do_xor(a, b);
            case 5:
            case 7: return do_sub(a, b);
        }

        return 0;
    }

    // Сдвиги
    // mode = номер функции
    // temp = сдвигаемое значение
    // n    = кол-во сдвигов
    u16 do_group_shift(u8 mode, u16 temp, u8 n) {

        u8  tmpc = 0;
        u16 temp2;
        u16 sign_bit = size ? 0x8000 : 0x80;
        u16 prev_bit = sign_bit >> 1;

        if (n == 0) return temp;

        switch (mode)
        {
            case 0: { // ROL

                flags &= ~(CF | OF);
                while (n > 0) {

                    tmpc = !!(temp & sign_bit);
                    temp = (temp << 1) | tmpc;
                    n--;
                }

                if (tmpc) flags |= CF;
                if (!!(flags & CF) != !!(temp & sign_bit)) flags |= OF;
                break;
            }
            case 1: { // ROR

                flags &= ~(CF | OF);
                while (n > 0) {

                    tmpc = temp & 1;
                    temp >>= 1;
                    if (tmpc) temp |= sign_bit;
                    n--;
                }

                if (tmpc) flags |= CF;
                if ((temp ^ (temp >> 1)) & prev_bit) flags |= OF;
                break;
            }
            case 2: { // RCL

                flags &= ~(OF);
                while (n > 0) {

                    tmpc  = flags & CF;
                    if (temp & sign_bit) flags |= CF; else flags &= ~CF;
                    temp  = (temp << 1) | tmpc;
                    n--;
                }

                // Установить флаги после обновления
                if (!!(flags & CF) != !!(temp & sign_bit)) flags |= OF;
                break;
            }
            case 3: { // RCR

                flags &= ~(OF);
                while (n > 0) {

                    tmpc  = flags & CF;
                    temp2 = temp&1;
                    temp >>= 1;
                    if (temp2) flags |= CF; else flags &= ~CF;
                    if (tmpc) temp |= sign_bit;
                    n--;
                }

                if ((temp ^ (temp >> 1)) & prev_bit) flags |= OF;
                break;
            }
            case 4:
            case 6: { // SHL

                flags &= ~(CF);
                if (n > (size ? 16 : 8)) {
                    temp = 0;
                } else {
                    if ((temp << (n-1)) & sign_bit) flags |= CF;
                    temp <<= n;
                }

                do_szp(temp);
                flags |= AF;
                break;
            }
            case 5: { // SHR

                flags &= ~(CF);
                if (n > (size ? 16 : 8)) {
                    temp = 0;
                } else {
                    if ((temp >> (n-1)) & 1) flags |= CF;
                    temp >>= n;
                }

                do_szp(temp);
                flags |= AF;
                break;
            }
            case 7: { // SAR

                flags &= ~(CF);
                if ((temp >> (n-1)) & 1) flags |= CF;

                while (n > 0) {

                    temp >>= 1;
                    if (temp & prev_bit) temp |= sign_bit;
                    n--;
                }

                do_szp(temp);
                flags |= AF;
                break;
            }
        }

        return temp;
    }

    // Десятичная коррекция после сложения
    void daa() {

        u8  AL = regs[_AX] & 255;
        u16 tempi;

        if ((flags & AF) || ((AL & 0xF) > 9)) {

            tempi = ((u16) AL) + 6;
            AL += 6;
            flags |= AF;
            if (tempi & 0x100) flags |= CF;
        }

        if ((flags & CF) || (AL > 0x9F)) {

            AL += 0x60;
            flags |= CF;
        }

        size = 0; do_szp(AL); put8(0, AL);
    }

    // Десятичная коррекция после вычитания
    void das() {

        u8  AL = regs[_AX] & 255;
        u16 tempi;

        if ((flags & AF) || ((AL & 0xF) > 9)) {

            tempi = ((u16)AL) - 6;
            AL -= 6;
            flags |= AF;
            if (tempi & 0x100) flags |= CF;
        }

        if ((flags & CF) || (AL>0x9F)) {

            AL -= 0x60;
            flags |= CF;
        }

        size = 0; do_szp(AL); put8(0, AL);
    }

    // ASCII-коррекция после сложения
    void aaa() {

        u8 AL = regs[_AX] & 255;
        u8 AH = regs[_AX] >> 8;

        if ((flags & AF) || ((AL & 0xF) > 9)) {

            AL += 6;
            AH++;
            flags |= (AF|CF);
        }
        else
           flags &= ~(AF|CF);

        regs[_AX] = (AL & 15) + ((u16)AH << 8);
    }

    // ASCII-коррекция после вычитания
    void aas() {

        u8 AL = regs[_AX]  & 255;
        u8 AH = regs[_AX] >> 8;

        if ((flags & AF) || ((AL & 0xF) > 9)) {

            AL -= 6;
            AH--;
            flags |= (AF|CF);
        }
        else
           flags &= ~(AF|CF);

        regs[_AX] = (AL & 15) + ((u16) AH <<8);
    }

    // Коррекция после умножения
    void aam() {

        u8 tb = fetchb();
        u8 AL = regs[_AX] & 255;
        regs[_AX] = (AL / tb)*256 + (AL % tb);
        size = 1; do_szp(regs[_AX]);
    }

    // Коррекция после деления
    void aad() {

        u8 tb = fetchb();
        regs[_AX] = ((regs[_AX]>>8)*tb + (regs[_AX]&255)) & 0x00FF;
        size = 1; do_szp(regs[_AX]);
    }

    // Если =1, то условие выполняется
    u8 get_cond(u8 n) {

        switch (n & 7) {

            case 0:  return  (flags & OF);
            case 1:  return !(flags & OF);
            case 2:  return  (flags & CF);
            case 3:  return !(flags & CF);
            case 4:  return  (flags & ZF);
            case 5:  return !(flags & ZF);
            case 6:  return  (flags & (CF | ZF));
            case 7:  return !(flags & (CF | ZF));
            case 8:  return  (flags & SF);
            case 9:  return !(flags & SF);
            case 10: return  (flags & PF);
            case 11: return !(flags & PF);
            case 12: return  (!!(flags & SF) != !!(flags & OF));
            case 13: return !(!!(flags & SF) != !!(flags & OF));
            case 14: return  ((flags & ZF) || !!(flags & SF) != !!(flags & OF));
            case 15: return !((flags & ZF) || !!(flags & SF) != !!(flags & OF));
        }

        return 0;
    }

    // === Процедуры ===

    // Сохранение в стек
    void push(u16 t) {

        regs[_SP] -= 2;
        writew(segs[_SS], regs[_SP], t);
    }

    // Извлечение из стека
    u16 pop() {

        u16 r = readw(segs[_SS], regs[_SP]);
        regs[_SP] += 2;
        return r;
    }

    // Вызов прерывания
    void interrupt(u8 int_id) {

        u16 a = (int_id << 2);
        u16 l = read(a+0) + 256*read(a+1);
        u16 h = read(a+2) + 256*read(a+3);

        push(flags | 0xF000);
        push(segs[_CS]);
        push(ip);

        flags &= ~(IF | TF);
        segs[_CS] = h; ip = l;
    }


    // Дальний вызов
    void callfar(u16 _cs, u16 _ip) {

        u16 cs_ = segs[_CS],
            ip_ = ip;

        segs[_CS] = _cs;
        ip = _ip;

        push(cs_);
        push(ip_);
    }

    // Ошибочная инструкция
    void ud() {

        // Undefined Instruction
    }

    // -------------------------------------------------------------------------
    // Выполнение шага инструкции
    // -------------------------------------------------------------------------

    void step(int instr_cnt = 1000) {

        // Выполнять инструкции пакетами, так быстрее
        for (int i = 0; i < instr_cnt; i++) {

            s8  offset;
            u16 t1, t2, t3;
            s16 tws;
            s32 templ, templs;
            // u16 ipstart = ip;

            segment_id  = _DS;
            overseg = 0;
            rep = 0;

            // Считывание префиксов
            while (1) {

                opcode = fetchb();

                if      (opcode == 0x26) { segment_id = _ES; }
                else if (opcode == 0x2E) { segment_id = _CS; }
                else if (opcode == 0x36) { segment_id = _SS; }
                else if (opcode == 0x3E) { segment_id = _DS; }
                else if (opcode == 0xF2) { rep = 1; }
                else if (opcode == 0xF3) { rep = 2; }
                else break;
            }

            // Размер операнда по умолчанию
            size = opcode & 1;

            switch (opcode) {

            // <ALU> rm, reg
            case 0x00: case 0x01: case 0x08: case 0x09:
            case 0x10: case 0x11: case 0x18: case 0x19:
            case 0x20: case 0x21: case 0x28: case 0x29:
            case 0x30: case 0x31: case 0x38: case 0x39: {

                getmodrm();
                opcode = (opcode >> 3) & 7;
                t1 = do_group_alu(get_rm(), get_reg(), opcode);
                if (opcode < 7) put_rm(t1);
                break;
            }

            // <ALU> reg, rm
            case 0x02: case 0x03: case 0x0A: case 0x0B:
            case 0x12: case 0x13: case 0x1A: case 0x1B:
            case 0x22: case 0x23: case 0x2A: case 0x2B:
            case 0x32: case 0x33: case 0x3A: case 0x3B: {

                getmodrm();
                opcode = (opcode >> 3) & 7;
                t1 = do_group_alu(get_reg(), get_rm(), opcode);
                if (opcode < 7) put_reg(t1);
                break;
            }

            // <ALU> a, imm
            case 0x04: case 0x05: case 0x0C: case 0x0D:
            case 0x14: case 0x15: case 0x1C: case 0x1D:
            case 0x24: case 0x25: case 0x2C: case 0x2D:
            case 0x34: case 0x35: case 0x3C: case 0x3D: {

                reg = 0;

                opcode = (opcode >> 3) & 7;
                t1 = do_group_alu(regs[_AX], size ? fetchw() : fetchb(), opcode);
                if (opcode < 7) put_reg(t1);
                break;
            }

            // PUSH, POP seg
            case 0x06: push(segs[_ES]); break;
            case 0x0E: push(segs[_CS]); break;
            case 0x16: push(segs[_SS]); break;
            case 0x1E: push(segs[_DS]); break;
            case 0x07: segs[_ES] = pop(); break;
            case 0x17: segs[_SS] = pop(); break;
            case 0x1F: segs[_DS] = pop(); break;
            case 0x0F: ud(); break;

            // Арифметические коррекции
            case 0x27: daa(); break;
            case 0x2F: das(); break;
            case 0x37: aaa(); break;
            case 0x3F: aas(); break;

            // INC, DEC r
            case 0x40: case 0x41: case 0x42: case 0x43:
            case 0x44: case 0x45: case 0x46: case 0x47:
            case 0x48: case 0x49: case 0x4A: case 0x4B:
            case 0x4C: case 0x4D: case 0x4E: case 0x4F: {

                size = 1;
                t1 = flags & 1;
                t2 = regs[opcode & 7];
                regs[opcode & 7] = opcode & 8 ? do_sub(t2, 1) : do_add(t2, 1);
                flags = (flags & 0xFFE) | (t1 & 1);
                break;
            }

            // PUSH r
            case 0x50: case 0x51: case 0x52: case 0x53:
            case 0x54: case 0x55: case 0x56: case 0x57: {

                push(regs[opcode & 7]);
                break;
            }

            // POP r
            case 0x58: case 0x59: case 0x5A: case 0x5B:
            case 0x5C: case 0x5D: case 0x5E: case 0x5F: {

                regs[opcode & 7] = pop();
                break;
            }

            // J<cccc> условные переходы
            case 0x70: case 0x71: case 0x72: case 0x73:
            case 0x74: case 0x75: case 0x76: case 0x77:
            case 0x78: case 0x79: case 0x7A: case 0x7B:
            case 0x7C: case 0x7D: case 0x7E: case 0x7F: {

                t1 = fetchb();
                if (get_cond(opcode & 15)) ip += (signed char) t1;
                break;
            }

            // GRP#1 e, imm
            case 0x80: case 0x81: case 0x82: case 0x83: {

                getmodrm();
                t1 = get_rm();

                // Второй операнд может быть знакорасширяемым
                if (opcode == 0x83) {
                    t2 = fetchb();
                    if (t2 & 0x80) t2 |= 0xFF00;
                } else {
                    t2 = size ? fetchw() : fetchb();
                }

                t1 = do_group_alu(t1, t2, reg);
                if (reg < 7) put_rm(t1);
                break;
            }

            // TEST 8/16,r
            case 0x84: case 0x85: getmodrm(); do_and(get_rm(), get_reg()); break;

            // XCHG 8/16,r
            case 0x86: case 0x87: {

                getmodrm();
                t1 = get_rm();
                put_rm(get_reg());
                put_reg(t1);
                break;
            }

            // MOV rm
            case 0x88: case 0x89: getmodrm(); put_rm(get_reg()); break;
            case 0x8A: case 0x8B: getmodrm(); put_reg(get_rm()); break;

            // MOV w, sreg
            case 0x8C: {

                size = 1;
                getmodrm();
                switch (reg) {

                    case 0: put_rm(segs[_ES]); break;
                    case 1: put_rm(segs[_CS]); break;
                    case 2: put_rm(segs[_SS]); break;
                    case 3: put_rm(segs[_DS]); break;
                }

                break;
            }

            // LEA r16, ea
            case 0x8D: getmodrm(); regs[reg] = ea; break;

            // MOV sreg, r16
            case 0x8E: {

                size = 1;
                getmodrm();

                switch (reg) {

                    case 0: segs[_ES] = get_rm(); break;
                    case 1: ud(); break;
                    case 2: segs[_SS] = get_rm(); break;
                    case 3: segs[_DS] = get_rm(); break;
                }

                break;
            }

            // POP rm
            case 0x8F: getmodrm(); put_rm(pop()); break;

            // XCHG ax, r
            case 0x90: case 0x91: case 0x92: case 0x93:
            case 0x94: case 0x95: case 0x96: case 0x97: {

                t1 = regs[opcode & 7];
                regs[opcode & 7] = regs[_AX];
                regs[_AX] = regs[t1];
                break;
            }

            // CBW, CWD
            case 0x98: regs[_AX] = (regs[_AX] & 0x0080  ? 0xFF00 : 0) | (regs[_AX] & 0xFF); break;
            case 0x99: regs[_DX] = (regs[_AX] & 0x8000) ? 0xFFFF : 0; break;

            // CALL CS:IP
            case 0x9A: t1 = fetchw(); t2 = fetchw(); callfar(t2, t1); break;

            // FWAIT
            case 0x9B: break;

            // PUSHF / POPF
            case 0x9C: push((flags & ~0x2A) | 2); break;
            case 0x9D: flags = (pop() & ~0x2A) | 2; break;

            // SAHF, LAHF
            case 0x9E: flags = (flags & 0xFF00) | ((regs[_AX] >> 8) & ~0x2A) | 2; break;
            case 0x9F: put8(4, flags | 2); break;

            // MOV [moffset] <> Acc
            case 0xA0: t1 = fetchw(); put8(_AX, readb(segment_id, t1)); break;
            case 0xA1: t1 = fetchw(); regs[_AX] = readw(segment_id, t1); break;
            case 0xA2: t1 = fetchw(); writeb(segment_id, t1, regs[_AX]); break;
            case 0xA3: t1 = fetchw(); writew(segment_id, t1, regs[_AX]); break;

            // TEST al, #8
            case 0xA8: case 0xA9: do_and(regs[_AX], size ? fetchw() : fetchb()); break;

            // MOV r, imm
            case 0xB0: case 0xB1: case 0xB2: case 0xB3:
            case 0xB4: case 0xB5: case 0xB6: case 0xB7:
            case 0xB8: case 0xB9: case 0xBA: case 0xBB:
            case 0xBC: case 0xBD: case 0xBE: case 0xBF: {

                size = opcode & 8;
                reg  = opcode & 7;
                put_reg(size ? fetchw() : fetchb());
                break;
            }

            // Сдвиг на Imm
            case 0xC0: case 0xC1: {

                getmodrm();
                t1 = fetchb();
                put_rm(do_group_shift(reg, get_rm(), t1 & (size ? 31 : 7)));
                break;
            }

            // RET Imm
            case 0xC2: {

                t1 = fetchw();
                ip = pop();
                regs[_SP] += (u16) t1;
                break;
            }

            // RET
            case 0xC3: ip = pop(); break;

            // LES, LDS
            case 0xC4: case 0xC5: {

                size = 1;
                getmodrm();
                put_reg(readw(segment_id, ea));
                segs[opcode & 1 ? _DS : _ES] = readw(segment_id, ea + 2);
                break;
            }

            // MOV e, #8/16
            case 0xC6: case 0xC7: {

                getmodrm();
                put_rm(size ? fetchw() : fetchb());
                break;
            }

            // RETF [#16]
            case 0xCA: {

                t1 = fetchw();
                t2 = pop();
                segs[_CS] = pop();
                ip = t2;
                regs[_SP] += (u16) t1;
                break;
            }

            // RETF
            case 0xCB: t1 = pop(); segs[_CS] = pop(); ip = t1; break;

            // INT xx
            case 0xCC: interrupt(3); break;
            case 0xCD: interrupt(fetchb()); break;
            case 0xCE: if (flags & OF) interrupt(4); break;

            // IRET
            case 0xCF: {

                t1 = pop();
                t2 = pop();
                flags = pop() & 0xFFF;
                segs[_CS] = t2;
                ip = t1;
                break;
            }

            // Сдвиг на 1
            case 0xD0: case 0xD1: {

                getmodrm();
                put_rm(do_group_shift(reg, get_rm(), 1));
                break;
            }

            // Сдвиг на CL
            case 0xD2: case 0xD3: {

                getmodrm();
                put_rm(do_group_shift(reg, get_rm(), regs[_CX] & (size ? 31 : 7)));
                break;
            }

            // AAM, AAD
            case 0xD4: aam(); break;
            case 0xD5: aad(); break;

            // SALC, XLAT
            case 0xD6: put8(_AX, flags & CF ? 0xFF : 00); break;
            case 0xD7: put8(_AX, readb(segment_id, regs[_BX] + (regs[_AX] & 255))); break;

            // ESC-последовательности: прочесть операнды, но не делать ровным счетом ничего
            case 0xD8: case 0xD9: case 0xDA: case 0xDB:
            case 0xDC: case 0xDD: case 0xDE: case 0xDF: {

                getmodrm();
                break;
            }

            // LOOP[NZ|Z] JCXZ
            case 0xE0: case 0xE1: case 0xE2: {

                offset = (signed char) fetchb();
                regs[_CX]--;

                // LOOPNZ, LOOPZ, LOOP
                // opcode[1] = 0: Срабатывает если ZF == opcode[0]
                //           = 1: Безусловно срабатывет без учета ZF
                if (regs[_CX] && ((!(opcode & 2) && (!!(flags & ZF) == !!(opcode & 1))) || (opcode & 2)))
                    ip += offset;

                break;
            }

            // JCXZ b8
            case 0xE3: {

                offset = (signed char) fetchb();
                if (regs[_CX] == 0) ip += offset;
                break;
            }

            // IN; OUT imm
            case 0xE4: t1 = fetchb(); put8(_AX, pin(t1)); break;
            case 0xE5: t1 = fetchb(); t2 = pin(t1); regs[_AX] = t2 + 256*pin(t1+1); break;
            case 0xE6: t1 = fetchb(); pout(t1, regs[_AX]); break;
            case 0xE7: t1 = fetchb(); pout(t1, regs[_AX]); pout(t1 + 1, regs[_AX] >> 8); break;

            // CALL r16; JMP r16; JMP s:o; JMP r8
            case 0xE8: t1 = fetchw(); push(ip); ip += t1; break;
            case 0xE9: t1 = fetchw(); ip += t1; break;
            case 0xEA: t1 = fetchw(); t2 = fetchw(); segs[_CS] = t2; ip = t1; break;
            case 0xEB: t1 = fetchb(); ip += (signed char) t1; break;

            // IN/OUT #8
            case 0xEC: put8(_AX, pin(regs[_DX])); break;
            case 0xED: t2 = pin(regs[_DX]); regs[_AX] = t2 + 256*pin(regs[_DX]+1); break;
            case 0xEE: pout(regs[_DX], regs[_AX]); break;
            case 0xEF: pout(regs[_DX], regs[_AX]); pout(regs[_DX] + 1, regs[_AX] >> 8); break;

            // INT 1
            case 0xF1: interrupt(1); break;

            // HLT, CMC
            case 0xF4: ip--; halt = 1; break;
            case 0xF5: flags ^= CF; break;

            // Групповые инструкции #byte
            case 0xF6: {

                getmodrm();
                t1 = get_rm();

                switch (reg) {

                    // TEST/NOT/NEG b
                    case 0:
                    case 1: do_and(t1, fetchb()); break;
                    case 2: put_rm(~t1); break;
                    case 3: put_rm(do_sub(0, t1)); break;

                    // MUL AL, b
                    case 4: {

                        do_szp(regs[_AX] & 255);
                        flags &= ~(CF|OF|ZF);

                        regs[_AX] = (u16)(regs[_AX] & 255) * t1;

                        // Флаги ZF и переполнение
                        if (regs[_AX] == 0) flags |= ZF;
                        if (regs[_AX] & 0xFF00) flags |= (CF|OF); else flags &= ~(CF|OF);
                        break;
                    }

                    // IMUL AL, b
                    case 5: {

                        do_szp(regs[_AX] & 255);
                        flags &= ~(CF|OF|ZF);

                        regs[_AX] = (u16)((s8)(regs[_AX] & 255))*(u16)((s8) t1);

                        // Флаги ZF и переполнение
                        if (regs[_AX] == 0) flags |= ZF;
                        if (regs[_AX] & 0xFF00) flags |= (CF|OF);
                        break;
                    }

                    // DIV AL, b
                    case 6: {

                        t2 = regs[_AX];
                        if (t1) {

                            t3  = t2 % t1;
                            t2 /= t1;
                            regs[_AX] = 256*(t3 & 255) + (t2 & 255);

                        } else {
                            interrupt(0);
                        }

                        break;
                    }

                    // IDIV AL, b
                    case 7: {

                        tws = (signed short int) regs[_AX];

                        if (t1) {

                            t3   = tws % (u16)((signed char)t1);
                            tws /= (u16)((signed char)t1);
                            regs[_AX] = 256*(t3 & 255) + (tws & 255);

                        } else {
                            interrupt(0);
                        }

                        break;
                    }
                }

                break;
            }

            // Групповые инструкции #word
            case 0xF7: {

                getmodrm();
                t1 = get_rm();

                switch (reg) {

                    // TEST/NOT/NEG w
                    case 0:
                    case 1: do_and(t1, fetchw()); break;
                    case 2: put_rm(~t1); break;
                    case 3: put_rm(do_sub(0, t1)); break;

                    // MUL AX, w
                    case 4: {

                        do_szp(regs[_AX]);

                        flags &= ~(CF | OF | ZF);
                        templ = (u32)regs[_AX] * (u32)t1;

                        regs[_AX] = templ;
                        regs[_DX] = (u32) templ >> 16;

                        if (regs[_AX] == 0 && regs[_DX] == 0) flags |= ZF;
                        if (regs[_DX]) flags |= (CF|OF);
                        break;
                    }

                    // IMUL AX, w
                    case 5: {

                        do_szp(regs[_AX]);

                        flags &= ~(CF | OF | ZF);
                        templ = (s32)regs[_AX] * (s32)t1;

                        regs[_AX] = templ;
                        regs[_DX] = (u32) templ >> 16;

                        if (regs[_AX] && regs[_DX] != 0xFFFF) flags |= (CF|OF);
                        if (regs[_AX] == 0 && regs[_DX] == 0) flags |= ZF;
                        break;
                    }

                    // DIV AX, w
                    case 6: {

                        templ = ((u32) regs[_DX] << 16) | (u32) regs[_AX];

                        if (t1) {

                            t2 = templ % t1;
                            templ /= t1;
                            regs[_DX] = t2;
                            regs[_AX] = templ;
                        }
                        else {
                            interrupt(0);
                        }

                        break;
                    }

                    // IDIV AX, w
                    case 7: {

                        if (regs[_DX] != 0 && regs[_DX] != 0xFFFF) {
                            interrupt(0);
                            break;
                        }

                        templs = (((s32)regs[_DX] << 16) | (s32)regs[_AX]);

                        if (t1)
                        {
                            t2  = templs % (s32)t1;
                            templs /= (s32) t1;
                            regs[_DX] = t2;
                            regs[_AX] = templs;
                        }
                        else {
                            interrupt(0);
                        }

                        break;
                    }
                }
                break;
            }

            // Флаги
            case 0xF8: flags &= 0xFFE; break; // CF=0
            case 0xF9: flags |= 0x001; break; // CF=1
            case 0xFA: flags &= 0xDFF; break; // IF=0
            case 0xFB: flags |= 0x200; break; // IF=1
            case 0xFC: flags &= 0xBFF; break; // DF=0
            case 0xFD: flags |= 0x400; break; // DF=1

            // INC/DEC b8
            case 0xFE: {

                getmodrm();

                t1 = get_rm();
                t2 = flags & CF;

                switch (reg) {

                    case 0: put_rm(do_add(t1, 1)); break;
                    case 1: put_rm(do_sub(t1, 1)); break;
                    default: ud();
                }

                flags = (flags & ~CF) | t2;
                break;
            }

            // Групповые смешанные
            case 0xFF: {

                getmodrm();

                t1 = get_rm();
                t2 = flags & CF;

                switch (reg) {

                    // INC, DEC r16
                    case 0: put_rm(do_add(t1, 1)); flags = (flags & ~CF) | t2; break;
                    case 1: put_rm(do_sub(t1, 1)); flags = (flags & ~CF) | t2;  break;

                    // CALL
                    case 2: push(ip); ip = t1; break;

                    // CALL far
                    case 3: callfar(readw(segment_id, ea + 2), readw(segment_id, ea)); break;

                    // JMP
                    case 4: ip = t1; break;

                    // JMP far
                    case 5: {

                        t2 = readw(segment_id, ea  );
                        t3 = readw(segment_id, ea+2);
                        segs[_CS] = t3; ip = t2;
                        break;
                    }

                    // PUSH w
                    case 6: push(t1); break;

                    default: ud();
                }

                break;
            }
            }

            if (halt) break;
        }
    }
};
