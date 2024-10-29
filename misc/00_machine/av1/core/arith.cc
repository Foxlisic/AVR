#include <avrcpu.h>
#include <stdio.h>
#include <stdlib.h>

// Развернуть итоговые биты
unsigned int AVRCPU::neg(unsigned int n) {
    return n ^ 0xffff;
}

// Установка флагов
void AVRCPU::set_logic_flags(unsigned char r) {

    flag.v = 0;
    flag.n = (r & 0x80) ? 1 : 0;
    flag.s = flag.n;
    flag.z = (r == 0) ? 1 : 0;
    flag_to_byte();
}

// Флаги после вычитания с переносом
void AVRCPU::set_subcarry_flag(int d, int r, int R, int carry) {

    flag.c = d < r + carry ? 1 : 0;
    flag.z = ((R & 0xFF) == 0 && flag.z) ? 1 : 0;
    flag.n = (R & 0x80) > 1 ? 1 : 0;
    flag.v = (((d & neg(r) & neg(R)) | (neg(d) & r & R)) & 0x80) > 0 ? 1 : 0;
    flag.s = flag.n ^ flag.v;
    flag.h = (((neg(d) & r) | (r & R) | (R & neg(d))) & 0x80) > 0 ? 1 : 0;
    flag_to_byte();
}

// Флаги после вычитания
void AVRCPU::set_subtract_flag(int d, int r, int R) {

    flag.c = d < r ? 1 : 0;
    flag.z = (R & 0xFF) == 0 ? 1 : 0;
    flag.n = (R & 0x80) > 1 ? 1 : 0;
    flag.v = (((d & neg(r) & neg(R)) | (neg(d) & r & R)) & 0x80) > 0 ? 1 : 0;
    flag.s = flag.n ^ flag.v;
    flag.h = (((neg(d) & r) | (r & R) | (R & neg(d))) & 0x80) > 0 ? 1 : 0;
    flag_to_byte();
}

// Флаги после сложение с переносом
void AVRCPU::set_add_flag(int d, int r, int R, int carry) {

    flag.c = d + r + carry >= 0x100;
    flag.h = (((d & r) | (r & neg(R)) | (neg(R) & d)) & 0x08) > 0 ? 1 : 0;
    flag.z = R == 0 ? 1 : 0;
    flag.n = (R & 0x80) > 0 ? 1 : 0;
    flag.v = (((d & r & neg(R)) | (neg(d) & neg(r) & R)) & 0x80) > 0 ? 1 : 0;
    flag.s = flag.n ^ flag.v;
    flag_to_byte();
}

// Флаги после логической операции сдвига
void AVRCPU::set_lsr_flag(int d, int r) {

    flag.c = d & 1;
    flag.n = (r & 0x80) > 0 ? 1 : 0;
    flag.z = (r == 0x00) ? 1 : 0;
    flag.v = flag.n ^ flag.c;
    flag.s = flag.n ^ flag.v;
    flag_to_byte();
}

// Флаги после сложения 16 бит
void AVRCPU::set_adiw_flag(int a, int r) {

    flag.v = ((neg(a) & r) & 0x8000) > 0 ? 1 : 0;
    flag.c = ((neg(r) & a) & 0x8000) > 0 ? 1 : 0;
    flag.n = (r & 0x8000) > 0 ? 1 : 0;
    flag.z = (r & 0xFFFF) == 0 ? 1 : 0;
    flag.s = flag.v ^ flag.n;
    flag_to_byte();
}

// Флаги после сложения 16 бит
void AVRCPU::set_sbiw_flag(int a, int r) {

    flag.v = ((neg(a) & r) & 0x8000) > 0 ? 1 : 0;
    flag.c = ((neg(a) & r) & 0x8000) > 0 ? 1 : 0;
    flag.n = (r & 0x8000) > 0 ? 1 : 0;
    flag.z = (r & 0xFFFF) == 0 ? 1 : 0;
    flag.s = flag.v ^ flag.n;
    flag_to_byte();
}

// Выполнение арифметических действий
void AVRCPU::command_arith() {

    int R, Z, a, r, d;

    switch (command) {

        // Вычитание с переносом, но без записи
        case CPC:

            d = get_rd();
            r = get_rr();
            R = (d - r - flag.c) & 0xff;
            set_subcarry_flag(d, r, R, flag.c);
            break;

        // Вычитание с переносом
        case SBC:

            d = get_rd();
            r = get_rr();
            R = (d - r - flag.c) & 0xFF;
            set_subcarry_flag(d, r, R, flag.c);
            put_rd(R);
            break;

        // Сложение без переноса
        case ADD:

            d = get_rd();
            r = get_rr();
            R = (d + r) & 0xff;
            set_add_flag(d, r, R, 0);
            put_rd(R);
            break;

        // Вычитание без переноса
        case CP:

            d = get_rd();
            r = get_rr();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            break;

        // Вычитание без переноса
        case SUB:

            d = get_rd();
            r = get_rr();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            put_rd(R);
            break;

        // Сложение с переносом
        case ADC:

            d = get_rd();
            r = get_rr();
            R = (d + r + flag.c) & 0xff;
            set_add_flag(d, r, R, flag.c);
            put_rd(R);
            break;

        case AND: R = get_rd() & get_rr(); set_logic_flags(R); put_rd(R); break;
        case OR:  R = get_rd() | get_rr(); set_logic_flags(R); put_rd(R); break;
        case EOR: R = get_rd() ^ get_rr(); set_logic_flags(R); put_rd(R); break;
        // Логическое умножение
        case ANDI: R = get_rdi() & get_imm8(); set_logic_flags(R); put_rdi(R); break;
        case ORI:  R = get_rdi() | get_imm8(); set_logic_flags(R); put_rdi(R); break;

        // Вычитание непосредственного значения
        case SUBI:

            d = get_rdi();
            r = get_imm8();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            put_rdi(R);
            break;

        // Вычитание непосредственного значения с переносом
        case SBCI:

            d = get_rdi();
            r = get_imm8();
            R = (d - r - flag.c) & 0xFF;
            set_subcarry_flag(d, r, R, flag.c);
            put_rdi(R);
            break;

        // Сравнение без переноса
        case CPI:

            d = get_rdi();
            r = get_imm8();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            break;

        // Развернуть биты в другую сторону
        case COM:

            d = get_rd();
            r = (d ^ 0xFF) & 0xFF;
            set_logic_flags(r);
            flag.c = 1; flag_to_byte();
            put_rd(r);
            break;

        // Декремент
        case DEC:

            d = get_rd();
            r = (d - 1) & 0xff;
            put_rd(r);

            flag.v = (r == 0x7F) ? 1 : 0;
            flag.n = (r & 0x80) > 0 ? 1 : 0;
            flag.z = (r == 0x00) ? 1 : 0;
            flag.s = flag.v ^ flag.n;
            flag_to_byte();
            break;

        // Инкремент
        case INC:

            d = get_rd();
            r = (d + 1) & 0xff;
            put_rd(r);

            flag.v = (r == 0x80) ? 1 : 0;
            flag.n = (r & 0x80) > 0 ? 1 : 0;
            flag.z = (r == 0x00) ? 1 : 0;
            flag.s = flag.v ^ flag.n;
            flag_to_byte();
            break;

        // Сложение 16-битного числа
        case ADIW:

            d = 24 + ((opcode & 0x30) >> 3);
            a = get16(d);
            r = a + get_ka();
            set_adiw_flag(a, r);
            put16(d, r);
            break;

        // Вычитание 16-битного числа
        case SBIW:

            d = 24 + ((opcode & 0x30) >> 3);
            a = get16(d);
            r = a - get_ka();
            set_sbiw_flag(a, r);
            put16(d, r);
            break;

        // Логический сдвиг вправо
        case LSR:

            d = get_rd();
            r = d >> 1;
            set_lsr_flag(d, r);
            put_rd(r);
            break;

        // Арифметический вправо
        case ASR:

            d = get_rd();
            r = (d >> 1) | (d & 0x80);
            set_lsr_flag(d, r);
            put_rd(r);
            break;

        // Циклический сдвиг вправо
        case ROR:

            d = get_rd();
            r = (d >> 1) | (flag.c ? 0x80 : 0x00);
            set_lsr_flag(d, r);
            put_rd(r);
            break;

        // Отрицание
        case NEG:

            d = get_rd();
            R = (-d) & 0xFF;
            set_subtract_flag(0, d, R);
            put_rd(R);
            break;

        // Логические операции между (Z) и Rd
        case LAC:

            Z = get_Z();
            put(Z, get(Z) & (get_rd() ^ 0xFF));
            break;

        case LAS:

            Z = get_Z();
            put(Z, get(Z) | get_rd());
            break;

        case LAT:

            Z = get_Z();
            put(Z, get(Z) ^ get_rd());
            break;

        // Выдача информации о неизвестной команде
        default: printf("Ошибка исполнения команды #1"); exit(1);
    }
}
