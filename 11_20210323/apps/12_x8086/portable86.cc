#include "portable86.h"

// ---------------------------------------------------------------------
// ПРОЦЕДУРЫ ДЛЯ АЛУ
// ---------------------------------------------------------------------

// Базовые флаги N,Z,P после 8-битных инструкции
void setznp8(uint8_t val) {

    flags &= ~0xC4;
    flags |= znptable8[val];
}

// Базовые флаги N,Z,P после 16-битных инструкции
void setznp16(uint16_t val) {

    flags &= ~0xC4;

    if (val & 0x8000) flags |= N_FLAG;
    if (val == 0)     flags |= Z_FLAG;
    flags |= (znptable8[val&0xff] & P_FLAG);
}

uint8_t setadd8(uint8_t a, uint8_t b) {

    uint16_t c = (uint16_t)a + (uint16_t)b;

    flags &= ~0x8D5;
    flags |= znptable8[c & 0xFF];

    if (c & 0x100) flags |= C_FLAG;
    if (!((a^b)&0x80)&&((a^c)&0x80)) flags |= V_FLAG;
    if (((a&0xF)+(b&0xF))&0x10)      flags |= A_FLAG;

    return c;
}

uint8_t setadd8nc(uint8_t a, uint8_t b) {

    uint16_t c = (uint16_t)a + (uint16_t)b;

    flags &= ~0x8D4;
    flags |= znptable8[c&0xFF];

    if (!((a^b)&0x80)&&((a^c)&0x80)) flags |= V_FLAG;
    if (((a&0xF)+(b&0xF))&0x10)      flags |= A_FLAG;

    return c;
}

uint8_t setadc8(uint8_t a, uint8_t b, uint8_t tempc) {

    uint16_t c = (uint16_t)a + (uint16_t)b + tempc;

    flags &= ~0x8D5;
    flags |= znptable8[c&0xFF];

    if (c & 0x100) flags |= C_FLAG;
    if (!((a^b)&0x80)&&((a^c)&0x80)) flags |= V_FLAG;
    if (((a&0xF)+(b&0xF))&0x10)       flags |= A_FLAG;

    return c;
}

uint16_t setadd16(uint16_t a, uint16_t b) {

    uint32_t c = (uint32_t)a + (uint32_t)b;

    flags &= ~0x8D5;
    setznp16(c & 0xffff);

    if (c & 0x10000) flags |= C_FLAG;
    if (!((a^b)&0x8000)&&((a^c)&0x8000)) flags |= V_FLAG;
    if (((a&0xF)+(b&0xF))&0x10)          flags |= A_FLAG;

    return c;
}

uint16_t setadd16nc(uint16_t a, uint16_t b) {

    uint32_t c = (uint32_t)a + (uint32_t)b;

    flags &= ~0x8D4;
    setznp16(c & 0xffff);

    if (!((a^b)&0x8000)&&((a^c)&0x8000)) flags |= V_FLAG;
    if (((a&0xF)+(b&0xF))&0x10)          flags |= A_FLAG;

    return c;
}

uint16_t setadc16(uint16_t a, uint16_t b, uint8_t tempc) {

    uint32_t c = (uint32_t)a + (uint32_t)b + tempc;

    flags &= ~0x8D5;
    setznp16(c & 0xffff);

    if (c & 0x10000) flags |= C_FLAG;
    if (!((a^b)&0x8000)&&((a^c)&0x8000)) flags |= V_FLAG;
    if (((a&0xF)+(b&0xF))&0x10)          flags |= A_FLAG;

    return c;
}

uint8_t setsub8(uint8_t a, uint8_t b) {

    uint16_t c = (uint16_t)a - (uint16_t)b;

    flags &= ~0x8D5;
    flags |= znptable8[c&0xFF];

    if (c&0x100) flags|=C_FLAG;
    if ((a^b)&(a^c)&0x80)       flags|=V_FLAG;
    if (((a&0xF)-(b&0xF))&0x10) flags|=A_FLAG;

    return c;
}

uint8_t setsub8nc(uint8_t a, uint8_t b) {

    uint16_t c=(uint16_t)a - (uint16_t)b;

    flags &= ~0x8D4;
    flags |= znptable8[c&0xFF];

    if ((a^b)&(a^c)&0x80)       flags |= V_FLAG;
    if (((a&0xF)-(b&0xF))&0x10) flags |= A_FLAG;

    return c;
}

uint8_t setsbc8(uint8_t a, uint8_t b, uint8_t tempc) {

    uint16_t c = (uint16_t)a - (((uint16_t)b) + tempc);

    flags &= ~0x8D5;
    flags |= znptable8[c&0xFF];

    if (c&0x100)                flags |= C_FLAG;
    if ((a^b)&(a^c)&0x80)       flags |= V_FLAG;
    if (((a&0xF)-(b&0xF))&0x10) flags |= A_FLAG;

    return c;
}

uint16_t setsub16(uint16_t a, uint16_t b) {

    uint32_t c = (uint32_t)a - (uint32_t)b;

    flags &= ~0x8D5;
    setznp16(c & 0xffff);

    if (c&0x10000) flags |= C_FLAG;
    if ((a^b)&(a^c)&0x8000)     flags |= V_FLAG;
    if (((a&0xF)-(b&0xF))&0x10) flags |= A_FLAG;

    return c;
}

uint16_t setsub16nc(uint16_t a, uint16_t b) {

    uint32_t c = (uint32_t)a - (uint32_t)b;

    flags &= ~0x8D4;
    setznp16(c & 0xffff);
    flags &= ~0x4;
    flags |= (znptable8[c&0xFF]&4);

    if ((a^b)&(a^c)&0x8000)     flags |= V_FLAG;
    if (((a&0xF)-(b&0xF))&0x10) flags |= A_FLAG;

    return c;
}

uint16_t setsbc16(uint16_t a, uint16_t b, uint8_t tempc) {

    uint32_t c = (uint32_t)a - (((uint32_t)b) + tempc);

    flags &= ~0x8D5;
    setznp16(c & 0xffff);
    flags &= ~0x4;
    flags |= (znptable8[c&0xFF]&4);

    if (c&0x10000)              flags |= C_FLAG;
    if ((a^b)&(a^c)&0x8000)     flags |= V_FLAG;
    if (((a&0xF)-(b&0xF))&0x10) flags |= A_FLAG;

    return c;
}

uint8_t setor8(uint8_t a, uint8_t b) {

    uint8_t c = a | b;

    setznp8(c);
    flags &=~ (C_FLAG | V_FLAG | A_FLAG);

    return c;
}

uint16_t setor16(uint16_t a, uint16_t b) {

    uint16_t c = a | b;

    setznp16(c);
    flags &=~ (C_FLAG | V_FLAG | A_FLAG);

    return c;
}

uint8_t setand8(uint8_t a, uint8_t b) {

    uint8_t c = a & b;

    setznp8(c);
    flags &=~ (C_FLAG | V_FLAG | A_FLAG);

    return c;
}

uint16_t setand16(uint16_t a, uint16_t b) {

    uint16_t c = a & b;

    setznp16(c);
    flags &=~ (C_FLAG | V_FLAG | A_FLAG);

    return c;
}

uint8_t setxor8(uint8_t a, uint8_t b) {

    uint8_t c = a ^ b;

    setznp8(c);
    flags &=~ (C_FLAG | V_FLAG | A_FLAG);

    return c;
}

uint16_t setxor16(uint16_t a, uint16_t b) {

    uint16_t c = a ^ b;

    setznp16(c);
    flags &=~ (C_FLAG | V_FLAG | A_FLAG);

    return c;
}

// ---------------------------------------------------------------------
// СЧИТЫВАНИЕ И ДЕКОДИРОВАНИЕ
// ---------------------------------------------------------------------

// Читать слово из памяти
uint16_t readmemw(uint32_t s, uint16_t a) {

    uint8_t l = readmemb(s + a);
    uint8_t h = readmemb(s + ((a+1) & 0xffff));
    return (h<<8) | l;
}

// Писать слово в память
void writememw(uint32_t s, uint16_t a, uint16_t v) {

    writememb(s + a, v);
    writememb(s + ((a+1) & 0xffff), v >> 8);
}

// Загрузка нового значения CS
void loadcs(uint16_t s) { segs[SEG_CS] = s; seg_cs = (uint32_t)s << 4; }
void loadss(uint16_t s) { segs[SEG_SS] = s; seg_ss = (uint32_t)s << 4; }
void loades(uint16_t s) { segs[SEG_ES] = s; seg_es = (uint32_t)s << 4; }
void loadds(uint16_t s) { segs[SEG_DS] = s; seg_ds = (uint32_t)s << 4; }
void loadfs(uint16_t s) { segs[SEG_FS] = s; seg_fs = (uint32_t)s << 4; }
void loadgs(uint16_t s) { segs[SEG_GS] = s; seg_gs = (uint32_t)s << 4; }

// Считывание следующего кода из CS:IP
inline uint8_t getbyte() { return readmemb(seg_cs + (ip++)); }

// Считывание WORD из потока CS:IP
uint16_t getword() {

    uint8_t l = getbyte();
    return ((uint16_t)getbyte() << 8) | l;
}

// Прочитать эффективный адрес
void fetchea() {

    rmdat   = getbyte();
    cpu_mod = (rmdat >> 6) & 3;
    cpu_reg = (rmdat >> 3) & 7;
    cpu_rm  = rmdat & 7;
    eaaddr  = 0;

    // Расчет эффективного адреса
    switch (cpu_rm) {

        case 0: eaaddr = (BX_ + SI_); break;
        case 1: eaaddr = (BX_ + DI_); break;
        case 2: eaaddr = (BP_ + SI_); break;
        case 3: eaaddr = (BP_ + DI_); break;
        case 4: eaaddr =  SI_; break;
        case 5: eaaddr =  DI_; break;
        case 6: eaaddr =  BP_; break;
        case 7: eaaddr =  BX_; break;
    }

    // Выбор сегмента
    if (sel_seg == 0 && ((cpu_rm == 6 && cpu_mod) || (cpu_rm == 2) || (cpu_rm == 3)))
        segment = seg_ss;

    // Модифицирующие биты modrm
    switch (cpu_mod) {

        case 0: if (cpu_rm == 6) eaaddr = getword(); break;
        case 1: eaaddr += (signed char) getbyte(); break;
        case 2: eaaddr += getword(); break;
        case 3: eaaddr = 0; break;
    }
}

// Чтение из 8-битного регистра
uint8_t getr8(uint8_t id) { return id & 4 ? regs[id&3] >> 8 : regs[id&3]; }

// Запись в 8-битный регистр
void setr8(uint8_t id, uint8_t v) {

    if (id & 4) {
        regs[id&3] = (regs[id&3] & 0x00ff) | ((uint16_t)v << 8);
    } else {
        regs[id&3] = (regs[id&3] & 0xff00) | v;
    }
}

// Чтение и запись в 16-битные регистры
inline uint16_t getr16(uint8_t id) { return regs[id]; }
inline void     setr16(uint8_t id, uint16_t v) { regs[id] = v; }

// Прочитать byte из r/m
uint8_t geteab() {

    if (cpu_mod == 3)
        return getr8(cpu_rm);
    else
        return readmemb(segment + eaaddr);
}

// Прочитать word из r/m
uint16_t geteaw() {

    if (cpu_mod == 3) return getr16(cpu_rm);
    return readmemw(segment, eaaddr);
}

// Записать byte в r/m
void seteab(uint8_t v) {

    if (cpu_mod == 3) {
        setr8(cpu_rm, v);
    } else {
        writememb(segment + eaaddr, v);
    }
}

// Записать word в r/m
void seteaw(uint16_t v) {

    if (cpu_mod == 3) {
        regs[cpu_rm] = v;
    } else {
        writememw(segment, eaaddr, v);
    }
}

// Запись в стек
void push(uint16_t v) {

    writememw(seg_ss, ((SP_ - 2) & 0xffff), v);
    SP_ -= 2;
}

// Извлечение из стека
uint16_t pop() {

    uint16_t r = readmemw(seg_ss, SP_);
    SP_ += 2;
    return r;
}

// Вызов прерывания
void interrupt(uint8_t int_id) {

    uint16_t a = (int_id<<2);
    uint16_t l = readmemw(0, a);
    uint16_t h = readmemw(0, a+2);

    push(flags | 0xF000);
    push(segs[SEG_CS]);
    push(ip);

    flags &= ~(I_FLAG | T_FLAG);
    loadcs(h); ip = l;
}

// Десятичная коррекция после сложения
void daa() {

    uint8_t  AL = AL_;
    uint16_t tempi;

    if ((flags & A_FLAG) || ((AL & 0xF) > 9))
    {
        tempi = ((uint16_t) AL) + 6;
        AL += 6;
        flags |= A_FLAG;
        if (tempi & 0x100) flags |= C_FLAG;
    }

    if ((flags & C_FLAG) || (AL > 0x9F))
    {
        AL += 0x60;
        flags |= C_FLAG;
    }

    setznp8(AL);
    setr8(REG_AL, AL);
}

// Десятичная коррекция после вычитания
void das() {

    uint8_t  AL = regs[REG_AL];
    uint16_t tempi;

    if ((flags & A_FLAG) || ((AL & 0xF)>9))
    {
        tempi = ((uint16_t)AL) - 6;
        AL -= 6;
        flags |= A_FLAG;
        if (tempi & 0x100) flags |= C_FLAG;
    }

    if ((flags & C_FLAG) || (AL>0x9F))
    {
        AL -= 0x60;
        flags |= C_FLAG;
    }

    setznp8(AL);
    setr8(REG_AL, AL);
}

// ASCII-коррекция после сложения
void aaa() {

    uint8_t  AL = regs[REG_AL];
    uint8_t  AH = regs[REG_AX] >> 8;

    if ((flags & A_FLAG) || ((AL & 0xF) > 9))
    {
        AL+=6;
        AH++;
        flags |= (A_FLAG|C_FLAG);
    }
    else
       flags &= ~(A_FLAG|C_FLAG);

    setr16(REG_AX, (AL&15) + ((uint16_t)AH << 8));
}

// ASCII-коррекция после вычитания
void aas() {

    uint8_t  AL = regs[REG_AL];
    uint8_t  AH = regs[REG_AX] >> 8;

    if ((flags & A_FLAG) || ((AL & 0xF) > 9))
    {
        AL-=6;
        AH--;
        flags |= (A_FLAG|C_FLAG);
    }
    else
       flags &= ~(A_FLAG|C_FLAG);

    setr16(REG_AX, (AL&15) + ((uint16_t)AH<<8));
}

// Коррекция после умножения
inline void aam() {

    uint8_t tb = getbyte();
    setr8(REG_AH, AL_ / tb);
    setr8(REG_AL, AL_ % tb);
    setznp16(AX_);
}

// Коррекция после деления
inline void aad() {

    uint8_t tb = getbyte();
    AX_ = (AH_*tb + AL_) & 0x00FF;
    setznp16(AX_);
}

// Дальний вызов
void callfar(uint16_t _cs, uint16_t _ip) {

    uint16_t cs_ = segs[SEG_CS], ip_ = ip;
    ip = _ip;
    loadcs(_cs);
    push(cs_);
    push(ip_);
}

// Групповые операции АЛУ 8 бит
uint8_t groupalu8(uint8_t mode, uint8_t a, uint8_t b) {

    switch (mode) {

        case 0: return setadd8(a, b);
        case 1: return setor8 (a, b);
        case 2: return setadc8(a, b, flags & C_FLAG);
        case 3: return setsbc8(a, b, flags & C_FLAG);
        case 4: return setand8(a, b);
        case 5:
        case 7: return setsub8(a, b);
        case 6: return setxor8(a, b);
    }

    return 0;
}

// Групповые операции АЛУ 16 бит
uint16_t groupalu16(uint8_t mode, uint16_t a, uint16_t b) {

    switch (mode) {

        case 0: return setadd16(a, b);
        case 1: return setor16 (a, b);
        case 2: return setadc16(a, b, flags & C_FLAG);
        case 3: return setsbc16(a, b, flags & C_FLAG);
        case 4: return setand16(a, b);
        case 5:
        case 7: return setsub16(a, b);
        case 6: return setxor16(a, b);
    }

    return 0;
}

// Сдвиговые операции
// * mode = 0..7
// * bit  = 0/1 (8/16)
// * temp = значение
// * n    = кол-во сдвигов
uint16_t groupshift(uint8_t mode, uint8_t bit, uint16_t temp, uint8_t n) {

    uint8_t  tmpc = 0;
    uint16_t temp2;
    uint16_t sign_bit = bit ? 0x8000 : 0x80;
    uint16_t prev_bit = sign_bit >> 1;

    if (n == 0) return temp;

    switch (mode)
    {
        // ROL
        case 0: {

            flags &= ~(C_FLAG | V_FLAG);
            while (n > 0) {

                tmpc = !!(temp & sign_bit);
                temp = (temp << 1) | tmpc;
                n--;
            }

            if (tmpc) flags |= C_FLAG;
            if (!!(flags & C_FLAG) != !!(temp & sign_bit)) flags |= V_FLAG;
            break;
        }

        // ROR
        case 1: {

            flags &= ~(C_FLAG | V_FLAG);
            while (n > 0) {

                tmpc = temp & 1;
                temp >>= 1;
                if (tmpc) temp |= sign_bit;
                n--;
            }

            if (tmpc) flags |= C_FLAG;
            if ((temp ^ (temp >> 1)) & prev_bit) flags |= V_FLAG;
            break;
        }

        // RCL
        case 2: {

            flags &= ~(V_FLAG);
            while (n > 0) {

                tmpc  = flags & C_FLAG;
                if (temp & sign_bit) flags |= C_FLAG; else flags &= ~C_FLAG;
                temp  = (temp << 1) | tmpc;
                n--;
            }

            // Установить флаги после обновления
            if (!!(flags & C_FLAG) != !!(temp & sign_bit)) flags |= V_FLAG;
            break;
        }

        // RCR
        case 3: {

            flags &= ~(V_FLAG);
            while (n > 0) {

                tmpc  = flags&C_FLAG;
                temp2 = temp&1;
                temp >>= 1;
                if (temp2) flags |= C_FLAG; else flags &= ~C_FLAG;
                if (tmpc) temp |= sign_bit;
                n--;
            }

            if ((temp ^ (temp >> 1)) & prev_bit) flags |= V_FLAG;
            break;
        }

        // SHL
        case 4:
        case 6: {

            flags &= ~(C_FLAG);
            if (n > (bit ? 16 : 8)) {
                temp = 0;
            } else {
                if ((temp << (n-1)) & sign_bit) flags |= C_FLAG;
                temp <<= n;
            }

            if (bit) setznp16(temp); else setznp8(temp);
            flags |= A_FLAG;
            break;
        }

        // SHR
        case 5: {

            flags &= ~(C_FLAG);
            if (n > (bit ? 16 : 8)) {
                temp = 0;
            } else {
                if ((temp >> (n-1)) & 1) flags |= C_FLAG;
                temp >>= n;
            }

            if (bit) setznp16(temp); else setznp8(temp);
            flags |= A_FLAG;
            break;
        }

        // SAR
        case 7: {

            flags &= ~(C_FLAG);
            if ((temp >> (n-1)) & 1) flags |= C_FLAG;

            while (n > 0) {

                temp >>= 1;
                if (temp & prev_bit) temp |= sign_bit;
                n--;
            }

            if (bit) setznp16(temp); else setznp8(temp);
            flags |= A_FLAG;
            break;
        }
    }

    return temp;
}

// ---------------------------------------------------------------------
// ЦИКЛ ВЫПОЛНЕНИЯ ОДНОЙ ИНСТРУКЦИИ
// ---------------------------------------------------------------------

// Автоинкремент для строковых инструкции
void autorep(uint8_t flag_test) {

    // Есть префикс <REP | REPNZ | REPZ>
    if (rep) {

        CX_--;

        // Проверка на REPNZ, REPZ
        if (flag_test) {

            // Если REPZ, но не ZERO, переход к следующей инструкции
            if ((rep == REPZ)  && !(flags & Z_FLAG))
                return;

            // Если REPNZ, но ZERO, переход к следующей инструкции
            if ((rep == REPNZ) &&  (flags & Z_FLAG))
                return;
        }

        // Повтор инструкции
        if (CX_) ip = ip_start;
    }
}

// Заполнение данными
void initcpu() {

    unsigned char d, c = 0;

    do
    {
        d = (c>>4) ^ c;
        d = (d>>2) ^ d;
        d = (d>>1) ^ d;

        znptable8[c] = (d&1) ? 0 : P_FLAG;
        if (c == 0) znptable8[c] |= Z_FLAG;
        if (c&0x80) znptable8[c] |= N_FLAG;

        c++;
    }
    while (c != 0);

    loadcs(0xf000);
    loadds(0x0000);
    loades(0x0000);
    loadss(0x0000);

    ip  = 0x0000;
    SP_ = 0x0000;
    AX_ = 0x0000;

    inhlt = 0;
    tstates = 0;
}

// Undefined
void ud(int type) {

    ip--;

    // printf("[%04x:%04x] %02x UNDEFINED %02x\n", seg_cs, ip, readmemb(seg_cs + ip), type);
    // printf("fs=%04x\n", seg_fs);
    // exit(1);
}

// Расширенная инструкция
void extended() {

    opcode = getbyte();

    int16_t offset;

    switch (opcode) {

        // RDTSC
        case 0x31: AX_ = rdtsc&0xffff; DX_ = 0; break;

        // JO, JNO
        case 0x80: offset = (int16_t)getword(); if   (flags&V_FLAG)  ip += offset; break;
        case 0x81: offset = (int16_t)getword(); if (!(flags&V_FLAG)) ip += offset; break;

        // JB, JNB
        case 0x82: offset = (int16_t)getword(); if   (flags&C_FLAG)  ip += offset; break;
        case 0x83: offset = (int16_t)getword(); if (!(flags&C_FLAG)) ip += offset; break;

        // JZ, JNZ
        case 0x84: offset = (int16_t)getword(); if   (flags&Z_FLAG)  ip += offset; break;
        case 0x85: offset = (int16_t)getword(); if (!(flags&Z_FLAG)) ip += offset; break;

        // JBE, JNBE
        case 0x86: offset = (int16_t)getword(); if   (flags&(C_FLAG|Z_FLAG))  ip += offset; break;
        case 0x87: offset = (int16_t)getword(); if (!(flags&(C_FLAG|Z_FLAG))) ip += offset; break;

        // JS, JNS
        case 0x88: offset = (int16_t)getword(); if   (flags&N_FLAG)  ip += offset; break;
        case 0x89: offset = (int16_t)getword(); if (!(flags&N_FLAG)) ip += offset; break;

        // JP, JNP
        case 0x8A: offset = (int16_t)getword(); if   (flags&P_FLAG)  ip += offset; break;
        case 0x8B: offset = (int16_t)getword(); if (!(flags&P_FLAG)) ip += offset; break;

        // JL, JNL
        case 0x8C: offset = (int16_t)getword(); if (!!(flags&N_FLAG) != !!(flags&V_FLAG)) ip += offset; break;
        case 0x8D: offset = (int16_t)getword(); if (!!(flags&N_FLAG) == !!(flags&V_FLAG)) ip += offset; break;

        // JLE, JNLE: ZF=1 OR (SF!=OF); ZF=0 AND (SF=OF)
        case 0x8E: offset = (int16_t)getword(); if ( (flags&Z_FLAG) ||  !!(flags&N_FLAG) != !!(flags&V_FLAG))  ip += offset; break;
        case 0x8F: offset = (int16_t)getword(); if (!(flags&Z_FLAG) && (!!(flags&N_FLAG) == !!(flags&V_FLAG))) ip += offset; break;

        // MOVZX r16, rm8/16
        case 0xB6: fetchea(); setr16(cpu_reg, geteab()); break;
        case 0xB7: fetchea(); setr16(cpu_reg, geteaw()); break;

        default:

            ud(7);
            break;
    }
}

// Запуск в работу
int x86run(int32_t instr_cnt) {

    int8_t   offset;
    int16_t  tempws;
    uint8_t  cont, tempc, noint;
    uint8_t  tempb, tempb2;
    uint16_t tempw, tempw2;
    uint32_t templ;
    int32_t  templs;
    int8_t   trap;
    int8_t   p66, p67;

#ifdef DEBUGLOG
    FILE* fdebug = fopen("debug.log", "a+");
    char  sdebug[256];
#endif

    while (instr_cnt-- >= 0) {

        // Остановка процессора
        if (inhlt) return 1;

        rep     = 0;
        trap    = flags & T_FLAG;
        sel_seg = 0;
        noint   = 0;
        p66     = 0;
        p67     = 0;
        segment = seg_ds;
        tempc   = flags & C_FLAG;
        tstates++;
        ip_start = ip;

        do {

            rdtsc++;
            cont   = 0;
            opcode = getbyte();

#ifdef DEBUGLOG
        if (segs[SEG_CS] == 0x7B0) {
            sprintf(sdebug, "%04x:%04x %02x\n", segs[SEG_CS], ip, opcode);
            fputs(sdebug, fdebug);
        }
#endif

            switch (opcode) {

                // ADD
                case 0x00: fetchea(); seteab(setadd8 (geteab(), getr8 (cpu_reg))); break;
                case 0x01: fetchea(); seteaw(setadd16(geteaw(), getr16(cpu_reg))); break;
                case 0x02: fetchea(); setr8 (cpu_reg, setadd8 (getr8 (cpu_reg), geteab())); break;
                case 0x03: fetchea(); setr16(cpu_reg, setadd16(getr16(cpu_reg), geteaw())); break;
                case 0x04: setr8 (REG_AL, setadd8 (regs[REG_AL], getbyte())); break;
                case 0x05: setr16(REG_AX, setadd16(regs[REG_AX], getword())); break;
                case 0x06: push(segs[SEG_ES]); break;
                case 0x07: loades(pop()); break;

                // OR
                case 0x08: fetchea(); seteab(setor8 (geteab(), getr8 (cpu_reg))); break;
                case 0x09: fetchea(); seteaw(setor16(geteaw(), getr16(cpu_reg))); break;
                case 0x0A: fetchea(); setr8 (cpu_reg, setor8 (getr8 (cpu_reg), geteab())); break;
                case 0x0B: fetchea(); setr16(cpu_reg, setor16(getr16(cpu_reg), geteaw())); break;
                case 0x0C: setr8 (REG_AL, setor8 (regs[REG_AL], getbyte())); break;
                case 0x0D: setr16(REG_AX, setor16(regs[REG_AX], getword())); break;
                case 0x0E: push(segs[SEG_CS]); break;
                case 0x0F: extended(); break;

                // ADС
                case 0x10: fetchea(); seteab(setadc8 (geteab(), getr8 (cpu_reg), tempc)); break;
                case 0x11: fetchea(); seteaw(setadc16(geteaw(), getr16(cpu_reg), tempc)); break;
                case 0x12: fetchea(); setr8 (cpu_reg, setadc8 (getr8 (cpu_reg), geteab(), tempc)); break;
                case 0x13: fetchea(); setr16(cpu_reg, setadc16(getr16(cpu_reg), geteaw(), tempc)); break;
                case 0x14: setr8 (REG_AL, setadc8 (regs[REG_AL], getbyte(), tempc)); break;
                case 0x15: setr16(REG_AX, setadc16(regs[REG_AX], getword(), tempc)); break;
                case 0x16: push(segs[SEG_SS]); break;
                case 0x17: loadss(pop()); noint = 1; break;

                // SBB
                case 0x18: fetchea(); seteab(setsbc8 (geteab(), getr8 (cpu_reg), tempc)); break;
                case 0x19: fetchea(); seteaw(setsbc16(geteaw(), getr16(cpu_reg), tempc)); break;
                case 0x1A: fetchea(); setr8 (cpu_reg, setsbc8 (getr8 (cpu_reg), geteab(), tempc)); break;
                case 0x1B: fetchea(); setr16(cpu_reg, setsbc16(getr16(cpu_reg), geteaw(), tempc)); break;
                case 0x1C: setr8 (REG_AL, setsbc8 (regs[REG_AL], getbyte(), tempc)); break;
                case 0x1D: setr16(REG_AX, setsbc16(regs[REG_AX], getword(), tempc)); break;
                case 0x1E: push(segs[SEG_DS]); break;
                case 0x1F: loadds(pop()); break;

                // AND
                case 0x20: fetchea(); seteab(setand8 (geteab(), getr8 (cpu_reg))); break;
                case 0x21: fetchea(); seteaw(setand16(geteaw(), getr16(cpu_reg))); break;
                case 0x22: fetchea(); setr8 (cpu_reg, setand8 (getr8 (cpu_reg), geteab())); break;
                case 0x23: fetchea(); setr16(cpu_reg, setand16(getr16(cpu_reg), geteaw())); break;
                case 0x24: setr8 (REG_AL, setand8 (regs[REG_AL], getbyte())); break;
                case 0x25: setr16(REG_AX, setand16(regs[REG_AX], getword())); break;
                case 0x26: sel_seg = 1; segment = seg_es; cont = 1; break;
                case 0x27: daa(); break;

                // SUB
                case 0x28: fetchea(); seteab(setsub8 (geteab(), getr8 (cpu_reg))); break;
                case 0x29: fetchea(); seteaw(setsub16(geteaw(), getr16(cpu_reg))); break;
                case 0x2A: fetchea(); setr8 (cpu_reg, setsub8 (getr8 (cpu_reg), geteab())); break;
                case 0x2B: fetchea(); setr16(cpu_reg, setsub16(getr16(cpu_reg), geteaw())); break;
                case 0x2C: setr8 (REG_AL, setsub8 (regs[REG_AL], getbyte())); break;
                case 0x2D: setr16(REG_AX, setsub16(regs[REG_AX], getword())); break;
                case 0x2E: sel_seg = 1; segment = seg_cs; cont = 1; break;
                case 0x2F: das(); break;

                // XOR
                case 0x30: fetchea(); seteab(setxor8 (geteab(), getr8 (cpu_reg))); break;
                case 0x31: fetchea(); seteaw(setxor16(geteaw(), getr16(cpu_reg))); break;
                case 0x32: fetchea(); setr8 (cpu_reg, setxor8 (getr8 (cpu_reg), geteab())); break;
                case 0x33: fetchea(); setr16(cpu_reg, setxor16(getr16(cpu_reg), geteaw())); break;
                case 0x34: setr8 (REG_AL, setxor8 (regs[REG_AL], getbyte())); break;
                case 0x35: setr16(REG_AX, setxor16(regs[REG_AX], getword())); break;
                case 0x36: sel_seg = 1; segment = seg_ss; cont = 1; break;
                case 0x37: aaa(); break;

                // CMP
                case 0x38: fetchea(); setsub8 (geteab(), getr8 (cpu_reg)); break;
                case 0x39: fetchea(); setsub16(geteaw(), getr16(cpu_reg)); break;
                case 0x3A: fetchea(); setsub8 (getr8 (cpu_reg), geteab()); break;
                case 0x3B: fetchea(); setsub16(getr16(cpu_reg), geteaw()); break;
                case 0x3C: setsub8 (regs[REG_AL], getbyte()); break;
                case 0x3D: setsub16(regs[REG_AX], getword()); break;
                case 0x3E: sel_seg = 1; segment = seg_ds; cont = 1; break;
                case 0x3F: aas(); break;

                // INC r16
                case 0x40: case 0x41: case 0x42: case 0x43:
                case 0x44: case 0x45: case 0x46: case 0x47: {

                    regs[opcode&7] = setadd16nc(regs[opcode&7], 1);
                    break;
                }

                // DEC r16
                case 0x48: case 0x49: case 0x4A: case 0x4B:
                case 0x4C: case 0x4D: case 0x4E: case 0x4F: {

                    regs[opcode&7] = setsub16nc(regs[opcode&7], 1);
                    break;
                }

                // PUSH r16
                case 0x50: case 0x51: case 0x52: case 0x53:
                case 0x54: case 0x55: case 0x56: case 0x57: {

                    push(regs[opcode&7]);
                    break;
                }

                // POP r16
                case 0x58: case 0x59: case 0x5A: case 0x5B:
                case 0x5C: case 0x5D: case 0x5E: case 0x5F: {

                    regs[opcode&7] = pop();
                    break;
                }

                // PUSHA
                case 0x60: {

                    tempw = regs[REG_SP];
                    for (int i = 0; i < 8; i++)
                        push(i == REG_SP ? tempw : regs[i]);

                    break;
                }

                // POPA
                case 0x61: {

                    tempw = 0;
                    for (int i = 7; i >= 0; i--) {
                        if (i == REG_SP) tempw = pop();
                        else regs[i] = pop();
                    }
                    regs[REG_SP] = tempw;
                    break;
                }

                // FS: GS:
                case 0x64: sel_seg = 1; segment = seg_fs; cont = 1; break;
                case 0x65: sel_seg = 1; segment = seg_gs; cont = 1; break;

                // opsize, adsize
                case 0x66: p66 = ~p66; cont = 1; break;
                case 0x67: p67 = ~p67; cont = 1; break;

                // PUSH imm16
                case 0x68: push(getword()); break;
                case 0x6a: tempw = getbyte(); tempw = (tempw & 0x80 ? 0xff00 : 0) | tempw; push(tempw); break;

                // IMUL r16,rm,i16
                case 0x69: {

                    fetchea();
                    tempw = getword();
                    templ = (long)((int16_t)geteaw()) * (long)((int16_t)tempw);
                    setr16(cpu_reg, templ);

                    if (templ & 0xffff0000)
                         flags |=  (C_FLAG | V_FLAG);
                    else flags &= ~(C_FLAG | V_FLAG);

                    break;
                }

                // IMUL r16,rm,i8
                case 0x6B: {

                    fetchea();
                    tempw = getbyte();
                    tempw = tempw & 0x80 ? (0xff00 | tempw) : tempw;
                    templ = (long)((int16_t)geteaw()) * (long)((int16_t)tempw);
                    setr16(cpu_reg, templ);

                    if (templ & 0xffff0000)
                         flags |=  (C_FLAG | V_FLAG);
                    else flags &= ~(C_FLAG | V_FLAG);

                    break;
                }

                // JO, JNO
                case 0x70: offset = (int8_t)getbyte(); if   (flags&V_FLAG)  ip += offset; break;
                case 0x71: offset = (int8_t)getbyte(); if (!(flags&V_FLAG)) ip += offset; break;

                // JB, JNB
                case 0x72: offset = (int8_t)getbyte(); if   (flags&C_FLAG)  ip += offset; break;
                case 0x73: offset = (int8_t)getbyte(); if (!(flags&C_FLAG)) ip += offset; break;

                // JZ, JNZ
                case 0x74: offset = (int8_t)getbyte(); if   (flags&Z_FLAG)  ip += offset; break;
                case 0x75: offset = (int8_t)getbyte(); if (!(flags&Z_FLAG)) ip += offset; break;

                // JBE, JNBE
                case 0x76: offset = (int8_t)getbyte(); if   (flags&(C_FLAG|Z_FLAG))  ip += offset; break;
                case 0x77: offset = (int8_t)getbyte(); if (!(flags&(C_FLAG|Z_FLAG))) ip += offset; break;

                // JS, JNS
                case 0x78: offset = (int8_t)getbyte(); if   (flags&N_FLAG)  ip += offset; break;
                case 0x79: offset = (int8_t)getbyte(); if (!(flags&N_FLAG)) ip += offset; break;

                // JP, JNP
                case 0x7A: offset = (int8_t)getbyte(); if   (flags&P_FLAG)  ip += offset; break;
                case 0x7B: offset = (int8_t)getbyte(); if (!(flags&P_FLAG)) ip += offset; break;

                // JL, JNL
                case 0x7C: offset = (int8_t)getbyte(); if (!!(flags&N_FLAG) != !!(flags&V_FLAG)) ip += offset; break;
                case 0x7D: offset = (int8_t)getbyte(); if (!!(flags&N_FLAG) == !!(flags&V_FLAG)) ip += offset; break;

                // JLE, JNLE: ZF=1 OR (SF!=OF); ZF=0 AND (SF=OF)
                case 0x7E: offset = (int8_t)getbyte(); if ( (flags&Z_FLAG) ||  !!(flags&N_FLAG) != !!(flags&V_FLAG))  ip += offset; break;
                case 0x7F: offset = (int8_t)getbyte(); if (!(flags&Z_FLAG) && (!!(flags&N_FLAG) == !!(flags&V_FLAG))) ip += offset; break;

                // GRP#1 e,#8
                case 0x80:
                case 0x82: {

                    fetchea();
                    tempb  = geteab();
                    tempb2 = getbyte();
                    tempb = groupalu8(cpu_reg, tempb, tempb2);
                    if (cpu_reg < 7) seteab(tempb);
                    break;
                }

                // GRP#1 e,#16
                case 0x81: {

                    fetchea();
                    tempw  = geteaw();
                    tempw2 = getword();
                    tempw = groupalu16(cpu_reg, tempw, tempw2);
                    if (cpu_reg < 7) seteaw(tempw);
                    break;
                }

                // GRP#1 e16,#8
                case 0x83: {

                    fetchea();
                    tempw = geteaw();
                    tempb = getbyte();
                    tempw = groupalu16(cpu_reg, tempw, tempb | (tempb&0x80 ? 0xFF00 : 0));
                    if (cpu_reg < 7) seteaw(tempw);
                    break;
                }

                // TEST 8/16,r
                case 0x84: fetchea(); setand8 (geteab(), getr8 (cpu_reg)); break;
                case 0x85: fetchea(); setand16(geteaw(), getr16(cpu_reg)); break;

                // XCHG 8/16,r
                case 0x86: fetchea(); tempb = geteab(); seteab(getr8 (cpu_reg)); setr8 (cpu_reg, tempb); break;
                case 0x87: fetchea(); tempw = geteaw(); seteaw(getr16(cpu_reg)); setr16(cpu_reg, tempw); break;

                // MOV modrm
                case 0x88: fetchea(); seteab(getr8 (cpu_reg)); break;
                case 0x89: fetchea(); seteaw(getr16(cpu_reg)); break;
                case 0x8A: fetchea(); setr8 (cpu_reg, geteab()); break;
                case 0x8B: fetchea(); setr16(cpu_reg, geteaw()); break;

                // MOV w, sreg
                case 0x8C: {

                    fetchea();
                    switch (rmdat & 0x38) {
                        case 0x00: seteaw(segs[SEG_ES]); break;
                        case 0x08: seteaw(segs[SEG_CS]); break;
                        case 0x10: seteaw(segs[SEG_SS]); break;
                        case 0x18: seteaw(segs[SEG_DS]); break;
                        case 0x20: seteaw(segs[SEG_FS]); break;
                        case 0x28: seteaw(segs[SEG_GS]); break;
                    }
                    break;
                }

                // LEA r16, ea
                case 0x8D: fetchea(); regs[cpu_reg] = eaaddr; break;

                // MOV sreg, r16
                case 0x8E: {

                    fetchea();
                    noint = 1;
                    switch (rmdat & 0x38) {
                        case 0x00: loades(geteaw()); break;
                        case 0x08: ud(6); break;
                        case 0x10: loadss(geteaw()); break;
                        case 0x18: loadds(geteaw()); break;
                        case 0x20: loadfs(geteaw()); break;
                        case 0x28: loadgs(geteaw()); break;
                    }
                    break;
                }

                // POP rm
                case 0x8F: fetchea(); seteaw(pop()); break;

                // XCHG ax, r16
                case 0x90: case 0x91: case 0x92: case 0x93:
                case 0x94: case 0x95: case 0x96: case 0x97: {

                    tempw = regs[opcode&7];
                    regs[opcode&7] = regs[REG_AX];
                    regs[REG_AX] = tempw;
                    break;
                }

                // CBW, CWD
                case 0x98: AX_ = (AX_ & 0x0080  ? 0xff00 : 0) | (AX_ & 0xff); break;
                case 0x99: DX_ = (AX_ & 0x8000) ? 0xffff : 0; break;

                // CALL cs:ip
                case 0x9A: tempw = getword(); callfar(getword(), tempw); break;
                case 0x9B: /* FWAIT */ break;

                // PUSHF/POPF
                case 0x9C: push((flags & ~0x2A) | 2); break;
                case 0x9D: flags = pop() & 0xfff; break;

                // SAHF, LAHF
                case 0x9E: flags = (flags & 0xFF00) | (AX_ >> 8); break;
                case 0x9F: setr8(REG_AH, flags); break;

                // MOV acc,16
                case 0xA0: tempw = getword(); setr8(REG_AX, readmemb(segment + tempw)); break;
                case 0xA1: tempw = getword(); AX_ = readmemw(segment, tempw); break;
                case 0xA2: tempw = getword(); writememb(segment + tempw, AX_); break;
                case 0xA3: tempw = getword(); writememw(segment, tempw, AX_); break;

                // MOVSB: Переместить байт
                case 0xA4: {

                    REPINIT;
                    writememb(seg_es + DI_, readmemb(segment + SI_));
                    REPINC(SI_,1);
                    REPINC(DI_,1);
                    autorep(0);
                    break;
                }

                // MOVSW: Переместить слово
                case 0xA5: {

                    REPINIT;
                    writememw(seg_es, DI_, readmemw(segment, SI_));
                    REPINC(SI_, p66 ? 4 : 2);
                    REPINC(DI_, p66 ? 4 : 2);
                    autorep(0);
                    break;
                }

                // CMPSB
                case 0xA6: {

                    REPINIT;
                    setsub8(readmemb(segment + SI_), readmemb(seg_es  + DI_));
                    REPINC(SI_,1);
                    REPINC(DI_,1);
                    autorep(0);
                    break;
                }

                // CMPSW
                case 0xA7: {

                    REPINIT;
                    setsub16(readmemw(segment, SI_), readmemw(seg_es,  DI_));
                    REPINC(SI_, p66 ? 4 : 2);
                    REPINC(DI_, p66 ? 4 : 2);
                    autorep(0);
                    break;
                }

                // TEST al,#8
                case 0xA8: setand8 (AX_, getbyte()); break;
                case 0xA9: setand16(AX_, getword()); break;

                // STOSB
                case 0xAA: {

                    REPINIT;
                    writememb(seg_es + DI_, AL_);
                    REPINC(DI_, 1);
                    autorep(0);
                    break;
                }

                // STOSW
                case 0xAB: {

                    REPINIT;
                    writememw(seg_es, DI_, AX_);
                    REPINC(DI_, p66 ? 4 : 2);
                    autorep(0);
                    break;
                }

                // LODSB
                case 0xAC: {

                    REPINIT;
                    setr8(REG_AL, readmemb(segment + SI_));
                    REPINC(SI_, 1);
                    autorep(0);
                    break;
                }

                // LODSW
                case 0xAD: {

                    REPINIT;
                    AX_ = readmemw(segment, SI_);
                    REPINC(SI_, p66 ? 4 : 2);
                    autorep(0);
                    break;
                }

                // SCASB
                case 0xAE: {

                    REPINIT;
                    setsub8(AL_, readmemb(seg_es + DI_));
                    REPINC(DI_, 1);
                    autorep(1);
                    break;
                }

                // SCASW
                case 0xAF: {

                    REPINIT;
                    setsub16(AX_, readmemw(seg_es,  DI_));
                    REPINC(DI_, p66 ? 4 : 2);
                    autorep(1);
                    break;
                }

                // MOV r8, #8
                case 0xB0: case 0xB1: case 0xB2: case 0xB3:
                case 0xB4: case 0xB5: case 0xB6: case 0xB7: {

                    setr8(opcode&7, getbyte());
                    break;
                }

                // MOV r16, #16
                case 0xB8: case 0xB9: case 0xBA: case 0xBB:
                case 0xBC: case 0xBD: case 0xBE: case 0xBF: {

                    regs[opcode&7] = getword();
                    break;
                }

                // Grp#shift
                case 0xC0: fetchea(); tempb = getbyte(); seteab(groupshift(cpu_reg, 0, geteab(), tempb & 7)); break;
                case 0xC1: fetchea(); tempb = getbyte(); seteaw(groupshift(cpu_reg, 1, geteaw(), tempb & 31)); break;

                // RET #16/RET
                case 0xC2: tempw = getword(); ip = pop(); SP_ += tempw; break;
                case 0xC3: ip = pop(); break;

                // LES, LDS
                case 0xC4: fetchea(); setr16(cpu_reg, readmemw(segment, eaaddr)); loades(readmemw(segment, eaaddr+2)); break;
                case 0xC5: fetchea(); setr16(cpu_reg, readmemw(segment, eaaddr)); loadds(readmemw(segment, eaaddr+2)); break;

                // MOV e, #8/16
                case 0xC6: fetchea(); seteab(getbyte()); break;
                case 0xC7: fetchea(); seteaw(getword()); break;

                // ENTER / LEAVE
                case 0xC8: ud(4); break;
                case 0xC9: ud(5); break;

                // RETF [#16]
                case 0xCA: tempw = getword(); tempw2 = pop(); loadcs(pop()); ip = tempw; SP_ += tempw; break;
                case 0xCB: tempw = getword(); tempw2 = pop(); loadcs(pop()); ip = tempw; break;

                // INT xx
                case 0xCC: interrupt(3); break;
                case 0xCD: interrupt(getbyte()); break;
                case 0xCE: if (flags & V_FLAG) interrupt(4); break;

                // IRET
                case 0xCF: tempw = pop(); tempw2 = pop(); flags = pop() & 0xfff; loadcs(tempw2); ip = tempw; break;

                // Сдвиговые
                case 0xD0: fetchea(); seteab(groupshift(cpu_reg, 0, geteab(), 1)); break;
                case 0xD1: fetchea(); seteaw(groupshift(cpu_reg, 1, geteaw(), 1)); break;
                case 0xD2: fetchea(); seteab(groupshift(cpu_reg, 0, geteab(), CX_&7));  break;
                case 0xD3: fetchea(); seteaw(groupshift(cpu_reg, 1, geteaw(), CX_&31)); break;

                // AAM, AAD
                case 0xD4: aam(); break;
                case 0xD5: aad(); break;

                // SALC, XLAT
                case 0xD6: setr8(REG_AL, flags & C_FLAG ? 0xff : 00);
                case 0xD7: setr8(REG_AL, readmemb(segment + BX_ + (AX_ & 255))); break;

                // ESC-последовательности
                case 0xD8: case 0xD9: case 0xDA: case 0xDB:
                case 0xDC: case 0xDD: case 0xDE: case 0xDF: {

                    fetchea();
                    break;
                }

                // LOOP[NZ|Z] JCXZ
                case 0xE0: offset = (int8_t) getbyte(); CX_--; if ( CX_ && !(flags & Z_FLAG)) ip += offset; break;
                case 0xE1: offset = (int8_t) getbyte(); CX_--; if ( CX_ &&  (flags & Z_FLAG)) ip += offset; break;
                case 0xE2: offset = (int8_t) getbyte(); CX_--; if ( CX_) ip += offset; break;
                case 0xE3: offset = (int8_t) getbyte();        if (!CX_) ip += offset; break;

                // IN/OUT #8
                case 0xE4: tempb = getbyte(); setr8(REG_AL, ioread(tempb)); break;
                case 0xE5: tempb = getbyte(); setr8(REG_AL, ioread(tempb)); setr8(REG_AH, ioread(tempb+1)); break;
                case 0xE6: tempb = getbyte(); iowrite(tempb, AL_); break;
                case 0xE7: tempb = getbyte(); iowrite(tempb, AL_); iowrite(tempb+1, AH_); break;

                // CALL, JMP #8/16
                case 0xE8: tempw = getword(); push(ip); ip += tempw; break;
                case 0xE9: tempw = getword(); ip += tempw; break;
                case 0xEA: tempw = getword(); tempw2 = getword(); loadcs(tempw2); ip = tempw; break;
                case 0xEB: tempb = getbyte(); ip += (tempb & 0x80 ? tempb - 256 : tempb); break;

                // IN/OUT dx
                case 0xEC: setr8(REG_AL, ioread(DX_)); break;
                case 0xED: setr8(REG_AL, ioread(DX_)); setr8(REG_AH, ioread(DX_+1)); break;
                case 0xEE: iowrite(DX_, AL_); break;
                case 0xEF: iowrite(DX_, AL_); iowrite(DX_+1, AH_); break;

                // LOCK: INT1, REP, HLT, CMC
                case 0xF0: cont = 1; break;
                case 0xF1: interrupt(1); break;
                case 0xF2: rep = REPNZ; cont = 1; break;
                case 0xF3: rep = REPZ; cont = 1; break;
                case 0xF4: inhlt = 1; ip--; break;
                case 0xF5: flags ^= C_FLAG; break;

                // Групповые инструкции #byte
                case 0xF6: {

                    fetchea(); tempb = geteab();
                    switch (cpu_reg) {

                        // TEST/NOT/NEG b
                        case 0:
                        case 1: setand8(tempb, getbyte()); break;
                        case 2: seteab(~tempb); break;
                        case 3: seteab(setsub8(0, tempb)); break;

                        // MUL AL, b
                        case 4: {

                            setznp8(AL_);
                            AX_ = AL_ * tempb;
                            if (AX_) flags &= ~Z_FLAG;           else flags |= Z_FLAG;
                            if (AH_) flags |= (C_FLAG | V_FLAG); else flags &= ~(C_FLAG|V_FLAG);
                            break;
                        }

                        // IMUL AL, b
                        case 5: {

                            setznp8(AL_);
                            AX_ = (int16_t)((int8_t)AL_)*(int16_t)((int8_t)tempb);
                            if (AX_) flags &= ~Z_FLAG;         else flags |= Z_FLAG;
                            if (AH_) flags |= (C_FLAG|V_FLAG); else flags &= ~(C_FLAG|V_FLAG);
                            break;
                        }

                        // DIV AL, b
                        case 6: {

                            tempw = AX_;
                            if (tempb) {

                                tempw2 = tempw % tempb;
                                tempw /= tempb;
                                setr8(REG_AH, tempw2);
                                setr8(REG_AL, tempw);

                            } else interrupt(0);

                            break;
                        }

                        // IDIV AL, b
                        case 7: {

                            tempws = (int16_t) AX_;
                            if (tempb) {

                                tempw2  = tempws % (int16_t)((int8_t)tempb);
                                tempws /= (int16_t)((int8_t)tempb);
                                setr8(REG_AH, tempw2);
                                setr8(REG_AL, tempws);

                            } else interrupt(0);

                            break;
                        }
                    }
                    break;
                }

                // Групповые инструкции #word
                case 0xF7: {

                    fetchea(); tempw = geteaw();
                    switch (cpu_reg) {

                        // TEST/NOT/NEG w
                        case 0:
                        case 1: setand16(tempw, getword()); break;
                        case 2: seteaw(~tempw); break;
                        case 3: seteaw(setsub16(0, tempw)); break;

                        // MUL AX, w
                        case 4: {

                            setznp16(AX_);
                            templ = AX_ * tempw;
                            AX_ = templ;
                            DX_ = templ >> 16;
                            if (AX_ | DX_)   flags &= ~Z_FLAG; else flags |= Z_FLAG;
                            if (DX_) flags |= (C_FLAG|V_FLAG); else flags &= ~(C_FLAG|V_FLAG);
                            break;
                        }

                        // IMUL AX, w
                        case 5: {

                            setznp16(AX_);
                            templ = (long)((int16_t)AX_) * (long)((int16_t)tempw);

                            AX_ = templ;
                            DX_ = (uint16_t)(templ >> 16);

                            if (AX_ && DX_ != 0xFFFF)
                                 flags |=  (C_FLAG | V_FLAG);
                            else flags &= ~(C_FLAG | V_FLAG);

                            if (AX_ | DX_) flags &= ~Z_FLAG; else flags |= Z_FLAG;
                            break;
                        }

                        // DIV AX, w
                        case 6: {

                            templ = ((uint32_t)DX_ << 16) | AX_;
                            if (tempw)
                            {
                                tempw2 = templ % tempw;
                                templ /= tempw;
                                DX_ = tempw2;
                                AX_ = templ;
                            }
                            else interrupt(0);
                            break;
                        }

                        // IDIV AX, w
                        case 7: {

                            if (DX_ != 0 && DX_ != 0xFFFF) {
                                interrupt(0);
                                break;
                            }

                            templs = (int)(((int32_t)DX_ << 16) | AX_);

                            if (tempw)
                            {
                                tempw2  = templs % (int32_t)(int16_t)tempw;
                                templs /= (int32_t)(int16_t)tempw;
                                DX_ = tempw2;
                                AX_ = templs;
                            }
                            else interrupt(0);
                            break;
                        }
                    }
                    break;
                }

                // CLC, STC, CLI, STI
                case 0xF8: flags &= ~C_FLAG; break;
                case 0xF9: flags |=  C_FLAG; break;
                case 0xFA: flags &= ~I_FLAG; break;
                case 0xFB: flags |=  I_FLAG; break;
                case 0xFC: flags &= ~D_FLAG; break;
                case 0xFD: flags |=  D_FLAG; break;

                // INC/DEC b8
                case 0xFE: {

                    fetchea();
                    tempb = geteab();
                    switch (cpu_reg) {

                        case 0: seteab(setadd8nc(tempb, 1)); break;
                        case 1: seteab(setsub8nc(tempb, 1)); break;
                        default: ud(3);
                    }
                    break;
                }

                // Групповые смешанные
                case 0xFF: {

                    fetchea();
                    tempw = geteaw();
                    switch (cpu_reg) {

                        case 0: seteaw(setadd16nc(tempw, 1)); break;
                        case 1: seteaw(setsub16nc(tempw, 1)); break;

                        // CALL
                        case 2: push(ip); ip = tempw; break;

                        // CALL far
                        case 3: {

                            tempw  = readmemw(segment, eaaddr);
                            tempw2 = readmemw(segment, eaaddr + 2);
                            callfar(tempw2, tempw);
                            break;
                        }

                        // JMP
                        case 4: ip = tempw; break;

                        // JMP far
                        case 5: {

                            tempw  = readmemw(segment, eaaddr);
                            tempw2 = readmemw(segment, eaaddr+2);
                            loadcs(tempw2); ip = tempw;
                            break;
                        }

                        // PUSH w
                        case 6: push(geteaw()); break;

                        default: ud(1);
                    }
                    break;
                }

                default: ud(2); break;
            }
        }
        while (cont);

        // Если вызван trap
        if (trap && (flags & T_FLAG) && !noint) interrupt(1);
    }

#ifdef DEBUGLOG
    fclose(fdebug);
#endif

    return 0;
}

