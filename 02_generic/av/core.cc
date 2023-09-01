// ---------------------------------------------------------------------
// СОЗДАНИЕ МАСКИ ОПКОДОВ
// ---------------------------------------------------------------------

// Применение маски к опкоду
void AVR::assign_mask(const char* mask, int opcode) {

    int i, j;
    int mk[16], nmask = 0, baseop = 0;

    // Расчет позиции, где находится маска
    for (i = 0; i < 16; i++) {

        int ch = mask[15-i];
        if (ch == '0' || ch == '1')
            baseop |= (ch == '1' ? 1 << i : 0);
        else
            mk[nmask++] = i;
    }

    // Перебрать 2^nmask вариантов, по количеству битов и назначить всем опкод
    for (i = 0; i < (1 << nmask); i++) {

        int instn = baseop;
        for (j = 0; j < nmask; j++) if (i & (1 << j)) instn |= (1 << mk[j]);
        map[instn] = opcode;
    }
}

// Создание инструкции
void AVR::assign() {

    int i;

    // Очистка памяти и сброс кодов map
    for (i = 0; i < 65536; i++) { sram[i] = i < 32 ? 0xFF : 0; program[i] = 0; }
    for (i = 0; i < 65536; i++) { map[i] = UNDEFINED; }

    // SREG
    sram[0x5F] = 0x00;

    // Очистка флагов
    flag.c = 0; flag.s = 0;
    flag.z = 0; flag.h = 0;
    flag.n = 0; flag.t = 0;
    flag.v = 0; flag.i = 0;

    // Арифметические на 2 регистра
    assign_mask("000001rdddddrrrr", CPC);   // +
    assign_mask("000010rdddddrrrr", SBC);   // +
    assign_mask("000011rdddddrrrr", ADD);   // +
    assign_mask("000101rdddddrrrr", CP);    // +
    assign_mask("000110rdddddrrrr", SUB);   // +
    assign_mask("000111rdddddrrrr", ADC);   // +
    assign_mask("001000rdddddrrrr", AND);   // +
    assign_mask("001001rdddddrrrr", EOR);   // +
    assign_mask("001010rdddddrrrr", OR);    // +

    // Арифметика регистр + операнд K
    assign_mask("0011KKKKddddKKKK", CPI);   // +
    assign_mask("0100KKKKddddKKKK", SBCI);  // +
    assign_mask("0101KKKKddddKKKK", SUBI);  // +
    assign_mask("0110KKKKddddKKKK", ORI);   // +
    assign_mask("0111KKKKddddKKKK", ANDI);  // +
    assign_mask("10010110KKddKKKK", ADIW);  // +
    assign_mask("10010111KKddKKKK", SBIW);  // +

    // Условные и безусловные переходы
    assign_mask("1100kkkkkkkkkkkk", RJMP);  // +
    assign_mask("1101kkkkkkkkkkkk", RCALL); // +
    assign_mask("1001010100001000", RET);   // +
    assign_mask("1001010100011000", RETI);  // +
    assign_mask("111100kkkkkkksss", BRBS);  // +
    assign_mask("111101kkkkkkksss", BRBC);  // +
    assign_mask("1111110ddddd0bbb", SBRC);  // +
    assign_mask("1111111ddddd0bbb", SBRS);  // +
    assign_mask("10011001AAAAAbbb", SBIC);  // +
    assign_mask("10011011AAAAAbbb", SBIS);  // +
    assign_mask("000100rdddddrrrr", CPSE);  // +

    // Непрямые и длинные переходы
    assign_mask("1001010100001001", ICALL); // +
    assign_mask("1001010100011001", EICALL);
    assign_mask("1001010000001001", IJMP);  // +
    assign_mask("1001010000011001", EIJMP); // +
    assign_mask("1001010kkkkk111k", CALL);  // +
    assign_mask("1001010kkkkk110k", JMP);   // +

    // Перемещения
    assign_mask("1110KKKKddddKKKK", LDI);   // +
    assign_mask("001011rdddddrrrr", MOV);   // +
    assign_mask("1001000ddddd0000", LDS);   // +
    assign_mask("1001001ddddd0000", STS);   // +
    assign_mask("00000001ddddrrrr", MOVW);  // +
    assign_mask("1111100ddddd0bbb", BLD);   // +
    assign_mask("1111101ddddd0bbb", BST);   // +
    assign_mask("1001001ddddd0100", XCH);   // +

    // Однооперандные
    assign_mask("1001010ddddd0011", INC);   // +
    assign_mask("1001010ddddd1010", DEC);   // +
    assign_mask("1001010ddddd0110", LSR);   // +
    assign_mask("1001010ddddd0101", ASR);   // +
    assign_mask("1001010ddddd0111", ROR);   // +
    assign_mask("1001010ddddd0001", NEG);   // +
    assign_mask("1001010ddddd0000", COM);   // +
    assign_mask("1001010ddddd0010", SWAP);  // +
    assign_mask("1001001ddddd0110", LAC);   // +
    assign_mask("1001001ddddd0101", LAS);   // +
    assign_mask("1001001ddddd0111", LAT);   // +
    assign_mask("100101001sss1000", BCLR);  // +
    assign_mask("100101000sss1000", BSET);  // +
    assign_mask("10011000AAAAAbbb", CBI);   // +
    assign_mask("10011010AAAAAbbb", SBI);   // +

    // Косвенная загрузка из памяти
    assign_mask("1001000ddddd1100", LDX);   // +
    assign_mask("1001000ddddd1101", LDX_);  // +
    assign_mask("1001000ddddd1110", LD_X);  // +
    assign_mask("1001000ddddd1001", LDY_);  // +
    assign_mask("1001000ddddd1010", LD_Y);  // +
    assign_mask("10q0qq0ddddd1qqq", LDYQ);  // +
    assign_mask("1001000ddddd0001", LDZ_);  // +
    assign_mask("1001000ddddd0010", LD_Z);  // +
    assign_mask("10q0qq0ddddd0qqq", LDZQ);  // +

    // Косвенное сохранение
    assign_mask("1001001ddddd1100", STX);   // +
    assign_mask("1001001ddddd1101", STX_);  // +
    assign_mask("1001001ddddd1110", ST_X);  // +
    assign_mask("1001001ddddd1001", STY_);  // +
    assign_mask("1001001ddddd1010", ST_Y);  // +
    assign_mask("10q0qq1ddddd1qqq", STYQ);  // +
    assign_mask("1001001ddddd0001", STZ_);  // +
    assign_mask("1001001ddddd0010", ST_Z);  // +
    assign_mask("10q0qq1ddddd0qqq", STZQ);  // +

    // Загрузка из запись в память программ
    assign_mask("1001010111001000", LPM0Z);  // +
    assign_mask("1001000ddddd0100", LPMRZ);  // +
    assign_mask("1001000ddddd0101", LPMRZ_); // +
    assign_mask("1001010111101000", SPM);
    assign_mask("1001010111111000", SPM2);

    // Особые инструкции расширенной загрузки из памяти
    assign_mask("1001010111011000", ELPM0Z);  // +
    assign_mask("1001000ddddd0110", ELPMRZ);  // +
    assign_mask("1001000ddddd0111", ELPMRZ_); // +

    // Специальные
    assign_mask("1001010110001000", SLEEP); // +
    assign_mask("1001010110101000", WDR);   // +
    assign_mask("1001010110011000", BREAK); // +
    assign_mask("0000000000000000", NOP);   // +
    assign_mask("10110AAdddddAAAA", IN);    // +
    assign_mask("10111AAdddddAAAA", OUT);   // +
    assign_mask("1001001ddddd1111", PUSH);  // +
    assign_mask("1001000ddddd1111", POP);   // +
    assign_mask("10010100KKKK1011", DES);

    // Расширенные
    assign_mask("100111rdddddrrrr", MUL);
    assign_mask("00000010ddddrrrr", MULS);
    assign_mask("000000110ddd0rrr", MULSU);
    assign_mask("000000110ddd1rrr", FMUL);
}

// ---------------------------------------------------------------------
// ЦЕНТРАЛЬНЫЙ ПРОЦЕССОР
// ---------------------------------------------------------------------

// Исполнение шага процессора
int AVR::step() {

    int R, r, d, a, b, A, v, Z;
    uint16_t p;

    int cycles = 1;

    opcode  = fetch();
    command = map[opcode];

    // Исполнение опкода
    switch (command) {

        // --------------------------------
        // Управляющие
        // --------------------------------

        case WDR:
        case NOP: {
            break;
        }
        case SLEEP:
        case BREAK: {

            cpu_halt = 1;
            break;
        }
        case RJMP: {

            pc = get_rjmp();
            break;
        }
        case RCALL: {

            push16(pc >> 1);
            pc = get_rjmp();
            cycles = 2;
            break;
        }
        case RET: {

            pc = pop16() << 1;
            cycles = 3;
            break;
        }
        case RETI: {

            pc = pop16() << 1;
            flag.i = 1;
            flag_to_byte();
            cycles = 3;
            break;
        }
        case BCLR: {

            sram[0x5F] &= ~(1 << get_s3());
            byte_to_flag(sram[0x5F]);
            break;
        }
        case BSET: {

            sram[0x5F] |=  (1 << get_s3());
            byte_to_flag(sram[0x5F]);
            break;
        }

        // --------------------------------
        // Условные переходы
        // --------------------------------

        case BRBS: {

            if ((sram[0x5F] & (1 << (opcode & 7))))
                pc = get_branch();

            break;
        }
        case BRBC: {

            if (!(sram[0x5F] & (1 << (opcode & 7))))
                pc = get_branch();

            break;
        }
        case CPSE: {

            if (get_rd() == get_rr())
                cycles = skip_instr();

            break;
        }
        case SBRC: {

            if (!(get_rd() & (1 << (opcode & 7))))
                cycles = 1 + skip_instr();

            break;
        }
        case SBRS: {

            if (get_rd() & (1 << (opcode & 7)))
                cycles = 1 + skip_instr();

            break;
        }
        case SBIS:
        case SBIC: {

            b = (opcode & 7);
            A = (opcode & 0xF8) >> 3;
            v = get(0x20 + A) & (1 << b);

            // Пропуск, если в порту Ap есть бит (или нет бита)
            if ((command == SBIS && v) || (command == SBIC && !v))
                cycles = 1+skip_instr();

            break;
        }

        // --------------------------------
        // Ввод-вывод
        // --------------------------------

        case IN: {

            put_rd(get(0x20 + get_ap()));
            cycles = 2;
            break;
        }
        case OUT: {

            put(0x20 + get_ap(), get_rd());
            cycles = 2;
            break;
        }
        case CBI:
        case SBI: {

            b = 1 << (opcode & 0x07);
            A = (opcode & 0xF8) >> 3;

            // Сброс/установка бита в I/O
            if (command == CBI)
                 put(0x20 + A, get(0x20 + A) & ~b);
            else put(0x20 + A, get(0x20 + A) |  b);

            cycles = 2;
            break;
        }

        // --------------------------------
        // Стек
        // --------------------------------

        case PUSH: {

            push8(get_rd());
            cycles = 1;
            break;
        }
        case POP: {

            put_rd(pop8());
            cycles = 2;
            break;
        }

        // --------------------------------
        // Специальные
        // --------------------------------

        case SWAP: {

            d = get_rd();
            put_rd(((d & 0x0F) << 4) + ((d & 0xF0) >> 4));
            break;
        }
        case BST: {

            flag.t = (get_rd() & (1 << (opcode & 7))) > 0 ? 1 : 0;
            flag_to_byte();
            break;
        }
        case BLD: {

            a = get_rd();
            b = (1 << (opcode & 7));
            put_rd( flag.t ? (a | b) : (a & ~b) );
            break;
        }

        // --------------------------------
        // Арифметико-логические инструкции
        // --------------------------------

        case CPC: {

            d = get_rd();
            r = get_rr();
            R = (d - r - flag.c) & 0xFF;
            set_subcarry_flag(d, r, R, flag.c);
            break;
        }
        case SBC: {

            d = get_rd();
            r = get_rr();
            R = (d - r - flag.c) & 0xFF;
            set_subcarry_flag(d, r, R, flag.c);
            put_rd(R);
            break;
        }
        case ADD: {

            d = get_rd();
            r = get_rr();
            R = (d + r) & 0xff;
            set_add_flag(d, r, R, 0);
            put_rd(R);
            break;
        }
        case CP: {

            d = get_rd();
            r = get_rr();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            break;
        }
        case SUB: {

            d = get_rd();
            r = get_rr();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            put_rd(R);
            break;
        }
        case ADC: {

            d = get_rd();
            r = get_rr();
            R = (d + r + flag.c) & 0xff;
            set_add_flag(d, r, R, flag.c);
            put_rd(R);
            break;
        }
        case AND: {

            R = get_rd() & get_rr();
            set_logic_flags(R);
            put_rd(R);
            break;
        }
        case OR: {

            R = get_rd() | get_rr();
            set_logic_flags(R);
            put_rd(R);
            break;
        }
        case EOR: {

            R = get_rd() ^ get_rr();
            set_logic_flags(R);
            put_rd(R);
            break;
        }
        case ANDI: {

            R = get_rdi() & get_imm8();
            set_logic_flags(R);
            put_rdi(R);
            break;
        }
        case ORI: {

            R = get_rdi() | get_imm8();
            set_logic_flags(R);
            put_rdi(R);
            break;
        }
        case SUBI: {

            d = get_rdi();
            r = get_imm8();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            put_rdi(R);
            break;
        }
        case SBCI: {

            d = get_rdi();
            r = get_imm8();
            R = (d - r - flag.c) & 0xFF;
            set_subcarry_flag(d, r, R, flag.c);
            put_rdi(R);
            break;
        }
        case CPI: {

            d = get_rdi();
            r = get_imm8();
            R = (d - r) & 0xFF;
            set_subtract_flag(d, r, R);
            break;
        }
        case COM: {

            d = get_rd();
            r = (d ^ 0xFF) & 0xFF;
            set_logic_flags(r);
            flag.c = 1;
            flag_to_byte();
            put_rd(r);
            break;
        }
        case DEC: {

            d = get_rd();
            r = (d - 1) & 0xff;
            put_rd(r);

            flag.v = (r == 0x7F) ? 1 : 0;
            flag.n = (r & 0x80) > 0 ? 1 : 0;
            flag.z = (r == 0x00) ? 1 : 0;
            flag.s = flag.v ^ flag.n;
            flag_to_byte();
            break;
        }
        case INC: {

            d = get_rd();
            r = (d + 1) & 0xff;
            put_rd(r);

            flag.v = (r == 0x80) ? 1 : 0;
            flag.n = (r & 0x80) > 0 ? 1 : 0;
            flag.z = (r == 0x00) ? 1 : 0;
            flag.s = flag.v ^ flag.n;
            flag_to_byte();
            break;
        }
        case ADIW: {

            d = 24 + ((opcode & 0x30) >> 3);
            a = get16(d);
            b = get_ka();
            r = a + b;
            set_adiw_flag(a, r);
            put16(d, r);
            break;
        }
        case SBIW: {

            d = 24 + ((opcode & 0x30) >> 3);
            a = get16(d);
            b = get_ka();
            r = a - b;
            set_sbiw_flag(a, r);
            put16(d, r);
            break;
        }
        case LSR: {

            d = get_rd();
            r = d >> 1;
            set_lsr_flag(d, r);
            put_rd(r);
            break;
        }
        case ASR: {

            d = get_rd();
            r = (d >> 1) | (d & 0x80);
            set_lsr_flag(d, r);
            put_rd(r);
            break;
        }
        case ROR: {

            d = get_rd();
            r = (d >> 1) | (flag.c ? 0x80 : 0x00);
            set_lsr_flag(d, r);
            put_rd(r);
            break;
        }
        case NEG: {

            d = get_rd();
            R = (-d) & 0xFF;
            set_subtract_flag(0, d, R);
            put_rd(R);
            break;
        }

        // --------------------------------
        // Перемещения
        // --------------------------------

        case LDI: {

            put_rdi(get_imm8());
            break;
        }
        case LPM0Z: {

            sram[0] = program[get_Z()];
            cycles = 3;
            break;
        }
        case LPMRZ: {

            put_rd(program[get_Z()]);
            cycles = 3;
            break;
        }
        case LPMRZ_: {

            p = get_Z();
            put_rd(program[p]);
            put_Z(p + 1);
            cycles = 3;
            break;
        }
        case STX: {

            put(get_X(), get_rd());
            cycles = 2;
            break;
        }
        case STX_: {

            p = get_X();
            put(p, get_rd());
            put_X(p + 1);
            cycles = 2;
            break;
        }
        case ST_X: {

            p = get_X() - 1;
            put(p, get_rd());
            put_X(p);
            cycles = 2;
            break;
        }
        case STYQ: {

            put((get_Y() + get_qi()), get_rd());
            cycles = 2;
            break;
        }
        case STY_: {

            p = get_Y();
            put(p, get_rd());
            put_Y(p+1);
            cycles = 2;
            break;
        }
        case ST_Y: {

            p = get_Y() - 1;
            put(p, get_rd());
            put_Y(p);
            cycles = 2;
            break;
        }
        case STZQ: {

            put((get_Z() + get_qi()), get_rd());
            cycles = 2;
            break;
        }
        case STZ_: {

            p = get_Z();
            put(p, get_rd());
            put_Z(p+1);
            cycles = 2;
            break;
        }
        case ST_Z: {

            p = get_Z() - 1;
            put(p, get_rd());
            put_Z(p);
            cycles = 2;
            break;
        }
        case LDX: {

            put_rd(get(get_X()));
            break;
        }
        case LDX_: {

            p = get_X();
            put_rd(get(p));
            put_X(p+1);
            break;
        }
        case LD_X: {

            p = get_X() - 1;
            put_rd(get(p));
            put_X(p);
            break;
        }
        case LDYQ: {

            put_rd(get((get_Y() + get_qi())));
            cycles = 2;
            break;
        }
        case LDY_: {

            p = get_Y();
            put_rd(get(p));
            put_Y(p+1);
            cycles = 2;
            break;
        }
        case LD_Y: {

            p = get_Y() - 1;
            put_rd(get(p));
            put_Y(p);
            cycles = 2;
            break;
        }
        case LDZQ: {

            put_rd(get((get_Z() + get_qi())));
            cycles = 2;
            break;
        }
        case LDZ_: {

            p = get_Z();
            put_rd(get(p));
            put_Z(p+1);
            cycles = 2;
            break;
        }
        case LD_Z: {

            p = get_Z() - 1;
            put_rd(get(p));
            put_Z(p);
            cycles = 2;
            break;
        }
        case MOV: {

            put_rd(get_rr());
            break;
        }
        case MOVW: {

            r = (get_rr_index() & 0xF) << 1;
            d = (get_rd_index() & 0xF) << 1;
            put16(d, get16(r));
            break;
        }
        case LDS: {

            d = fetch();
            put_rd( get(d) );
            break;
        }
        case STS: {

            d = fetch();
            put(d, get_rd());
            break;
        }

        case ELPM0Z: {

            sram[0] = program[ get_Z() + (sram[0x5B] << 16) ];
            cycles = 3;
            break;
        }
        case ELPMRZ: {

            put_rd(program[ get_Z() + (sram[0x5B] << 16) ]);
            break;
        }
        case ELPMRZ_: { // ELPM Z+

            p = get_Z() + (sram[0x5B] << 16);
            put_rd(program[p]);
            put_Z(p + 1);
            cycles = 3;
            break;
        }

        // --------------------------------
        // РАСШИРЕНИЯ
        // --------------------------------

        /*
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

        // Обмен (Z) и Rd
        case XCH:

            p = get_Z();
            r = get(p);
            put(p, get_rd());
            put_rd(r);
            break;
        */

        case IJMP: {

            pc = 2 * get_Z();
            cycles = 2;
            break;
        }
        case EIJMP: {

            pc = 2 * (get_Z() + (sram[0x5B] << 16));
            cycles = 2;
            break;
        }
        case JMP: {

            pc = 2 * ((get_jmp() << 16) | fetch());
            cycles = 2;
            break;
        }
        case CALL: {

            push16((pc + 2) >> 1);
            pc = 2 * ((get_jmp() << 16) | fetch());
            cycles = 2;
            break;
        }
        case ICALL: {

            push16(pc >> 1);
            pc = 2 * get_Z();
            cycles = 3;
            break;
        }

        // --------------------------------
        // Аппаратное умножение
        // --------------------------------

        case MUL: {

            d = get_rd();
            r = get_rr();
            v = (r * d) & 0xffff;
            put16(0, v);

            flag.c = v >> 15;
            flag.z = v == 0;
            flag_to_byte();
            break;
        }
        case MULS: {

            d = sram[ 0x10 | ((opcode & 0xf0) >> 4) ];
            r = sram[ 0x10 |  (opcode & 0x0f) ];
            d = (d & 0x80 ? 0xff00 : 0) | d;
            r = (r & 0x80 ? 0xff00 : 0) | r;
            v = (r * d) & 0xffff;
            put16(0, v);

            flag.c = v >> 15;
            flag.z = v == 0;
            flag_to_byte();
            break;
        }
        case MULSU: {

            d = sram[ 0x10 | ((opcode & 0x70)>>4) ];
            r = sram[ 0x10 |  (opcode & 0x07) ];
            d = (d & 0x80 ? 0xff00 : 0) | d; // Перевод в знаковый
            v = (r * d) & 0xffff;
            put16(0, v);

            flag.c = v >> 15;
            flag.z = v == 0;
            flag_to_byte();
            break;
        }
        case FMUL: { // Аналогично MUL, но со сдвигом влево

            d = sram[ 0x10 | ((opcode & 0x70)>>4) ];
            r = sram[ 0x10 |  (opcode & 0x07) ];
            v = ((r * d) << 1) & 0xffff;
            put16(0, v);

            flag.c = v >> 15;
            flag.z = v == 0;
            flag_to_byte();
            break;
        }

        default:

            printf("Неизвестная инструкция $%04x в pc=$%04x\n", opcode, pc - 2);
            exit(1);
    }

    cycles = cycles ? cycles : 1;
    instr_counter += cycles;
    return cycles;
}

// Байт во флаги
void AVR::byte_to_flag(unsigned char f) {

    flag.c = (f >> 0) & 1;
    flag.z = (f >> 1) & 1;
    flag.n = (f >> 2) & 1;
    flag.v = (f >> 3) & 1;
    flag.s = (f >> 4) & 1;
    flag.h = (f >> 5) & 1;
    flag.t = (f >> 6) & 1;
    flag.i = (f >> 7) & 1;
};

// Флаги в байты
unsigned char AVR::flag_to_byte() {

    unsigned char f =
        (flag.i<<7) |
        (flag.t<<6) |
        (flag.h<<5) |
        (flag.s<<4) |
        (flag.v<<3) |
        (flag.n<<2) |
        (flag.z<<1) |
        (flag.c<<0);

    sram[0x5F] = f;
    return f;
}

// Развернуть итоговые биты
unsigned int AVR::neg(unsigned int n) {
    return n ^ 0xffff;
}

// Установка флагов
void AVR::set_logic_flags(unsigned char r) {

    flag.v = 0;
    flag.n = (r & 0x80) ? 1 : 0;
    flag.s = flag.n;
    flag.z = (r == 0) ? 1 : 0;
    flag_to_byte();
}

// Флаги после вычитания с переносом
void AVR::set_subcarry_flag(int d, int r, int R, int carry) {

    //flag.c = d < r + carry ? 1 : 0;
    flag.c = (((neg(d) & r) | (r & R) | (R & neg(d))) >> 7) & 1;
    flag.z = ((R & 0xFF) == 0 && flag.z) ? 1 : 0;
    flag.n = (R & 0x80) > 1 ? 1 : 0;
    flag.v = (((d & neg(r) & neg(R)) | (neg(d) & r & R)) & 0x80) > 0 ? 1 : 0;
    flag.s = flag.n ^ flag.v;
    flag.h = (((neg(d) & r) | (r & R) | (R & neg(d))) & 0x80) > 0 ? 1 : 0;
    flag_to_byte();
}

// Флаги после вычитания
void AVR::set_subtract_flag(int d, int r, int R) {

    flag.c = d < r ? 1 : 0;
    flag.z = (R & 0xFF) == 0 ? 1 : 0;
    flag.n = (R & 0x80) > 1 ? 1 : 0;
    flag.v = (((d & neg(r) & neg(R)) | (neg(d) & r & R)) & 0x80) > 0 ? 1 : 0;
    flag.s = flag.n ^ flag.v;
    flag.h = (((neg(d) & r) | (r & R) | (R & neg(d))) & 0x40) > 0 ? 1 : 0;
    flag_to_byte();
}

// Флаги после сложение с переносом
void AVR::set_add_flag(int d, int r, int R, int carry) {

    flag.c = d + r + carry >= 0x100;
    flag.h = (((d & r) | (r & neg(R)) | (neg(R) & d)) & 0x08) > 0 ? 1 : 0;
    flag.z = R == 0 ? 1 : 0;
    flag.n = (R & 0x80) > 0 ? 1 : 0;
    flag.v = (((d & r & neg(R)) | (neg(d) & neg(r) & R)) & 0x80) > 0 ? 1 : 0;
    flag.s = flag.n ^ flag.v;
    flag_to_byte();
}

// Флаги после логической операции сдвига
void AVR::set_lsr_flag(int d, int r) {

    flag.c = d & 1;
    flag.n = (r & 0x80) > 0 ? 1 : 0;
    flag.z = (r == 0x00) ? 1 : 0;
    flag.v = flag.n ^ flag.c;
    flag.s = flag.n ^ flag.v;
    flag_to_byte();
}

// Флаги после сложения 16 бит
void AVR::set_adiw_flag(int a, int r) {

    flag.v = ((neg(a) & r) >> 15) & 1;
    flag.c = ((neg(r) & a) >> 15) & 1;
    flag.n = (r & 0x8000) > 0 ? 1 : 0;
    flag.z = (r & 0xFFFF) == 0 ? 1 : 0;
    flag.s = flag.v ^ flag.n;
    flag_to_byte();
}

// Флаги после вычитания 16 бит
void AVR::set_sbiw_flag(int a, int r) {

    flag.v = ((neg(r) & a) >> 15) & 1;
    flag.c = ((neg(a) & r) >> 15) & 1;
    flag.n = (r & 0x8000) > 0 ? 1 : 0;
    flag.z = (r & 0xFFFF) == 0 ? 1 : 0;
    flag.s = flag.v ^ flag.n;
    flag_to_byte();
}

// В зависимости от инструкции CALL/JMP/LDS/STS
int AVR::skip_instr() {

    switch (map[ fetch() ]) {

        case CALL:
        case JMP:
        case LDS:
        case STS:

            pc += 2;
            break;
    }

    return 2;
}

// Вызов прерывания
void AVR::interruptcall() {

    flag.i = 0;
    flag_to_byte();
    push16(pc >> 1);
    pc = 4;
}
