const UNDEFINED = 0,
      CPC    = 1,   SBC    = 2,  ADD    = 3,   CP = 4,
      ADC    = 5,   AND    = 6,  EOR    = 7,   OR       = 8,
      SUB    = 9,   CPI    = 10, SBCI   = 11,  SUBI     = 12,
      ORI    = 13,  ANDI   = 14, ADIW   = 15,  SBIW     = 16,
      RJMP   = 17,  RCALL  = 18, RET    = 19,  RETI     = 20,
      BRBS   = 21,  BRBC   = 22, SBRC   = 23,  SBRS     = 24,
      SBIC   = 25,  SBIS   = 26, CPSE   = 27,  ICALL    = 28,
      EICALL = 29,  IJMP   = 30, EIJMP  = 31,  CALL     = 32,
      JMP    = 33,  LDI    = 34, MOV    = 35,  LDS      = 36,
      STS    = 37,  MOVW   = 38, BLD    = 39,  BST      = 40,
      XCH    = 41,  INC    = 42, DEC    = 43,  LSR      = 44,
      ASR    = 45,  ROR    = 46, NEG    = 47,  COM      = 48,
      SWAP   = 49,  LAC    = 50, LAS    = 51,  LAT      = 52,
      BCLR   = 53,  BSET   = 54, CBI    = 55,  SBI      = 56,
      LDX    = 57,  LDX_   = 58, LD_X   = 59,  LDY_     = 60,
      LD_Y   = 61,  LDYQ   = 62, LDZ_   = 63,  LD_Z     = 64,
      LDZQ   = 65,  STX    = 66, STX_   = 67,  ST_X     = 68,
      STY_   = 69,  ST_Y   = 70, STYQ   = 71,  STZ_     = 72,
      ST_Z   = 73,  STZQ   = 74, LPM0Z  = 75,  LPMRZ    = 76,
      LPMRZ_ = 77,  ELPM0Z = 78, ELPMRZ = 79,  ELPMRZ_  = 80,
      SPM    = 81,  SPM2   = 82, SLEEP  = 83,  WDR      = 84,
      BREAK  = 85,  NOP    = 86, IN     = 87,  OUT      = 88,
      PUSH   = 89,  POP    = 90, DES    = 91,  MUL      = 92,
      MULS   = 93,  MULSU  = 94, FMUL   = 95

class Core {

    constructor(core, model = 'generic') {

        let i;

        // Инициализация памяти
        this.sram    = new Uint8Array(65536);
        this.program = new Uint16Array(65536);
        this.map     = new Array(65536);
        this.model   = model;

        this._put    = core.put;
        this._get    = core.get;

        // Очистка памяти и сброс кодов map
        for (i = 0; i < 65536; i++) { this.sram[i] = i < 32 ? 0xFF : 0; this.program[i] = 0; }
        for (i = 0; i < 65536; i++) { this.map[i] = UNDEFINED; }

        // Копирование программы в память программ
        for (i = 0; i < core.program.length; i++) this.program[i] = core.program[i];

        // SREG
        this.sram[0x5F] = 0x00;

        // Очистка флагов
        this.flag       = {c: 0, s: 0, z: 0, h: 0, n: 0, t: 0, v: 0, i: 0};
        this.pc         = 0;
        this.opcode     = 0;
        this.cpu_halt   = 0;

        this.assign();
    }

    // Исполнение шага процессора
    step() {

        let R, r, d, a, b, A, v, Z;
        let p;

        let cycles = 1;

        this.opcode  = this.fetch();
        this.command = this.map[this.opcode];

        // Исполнение опкода
        switch (this.command) {

            // --------------------------------
            // Управляющие
            // --------------------------------

            case WDR:
            case NOP: {
                break;
            }
            case SLEEP:
            case BREAK: {

                this.cpu_halt = 1;
                break;
            }
            case RJMP: {

                this.pc = this.get_rjmp();
                break;
            }
            case RCALL: {

                this.push16(this.pc);
                this.pc = this.get_rjmp();
                cycles = 2;
                break;
            }
            case RET: {

                this.pc = this.pop16();
                cycles = 3;
                break;
            }
            case RETI: {

                this.pc = this.pop16();
                this.flag.i = 1;
                this.flag_to_byte();
                this.cycles = 3;
                break;
            }
            case BCLR: {

                this.sram[0x5F] &= ~(1 << this.get_s3());
                this.byte_to_flag(this.sram[0x5F]);
                break;
            }
            case BSET: {

                this.sram[0x5F] |=  (1 << this.get_s3());
                this.byte_to_flag(this.sram[0x5F]);
                break;
            }

            // --------------------------------
            // Условные переходы
            // --------------------------------

            case BRBS: {

                if ((this.sram[0x5F] & (1 << (this.opcode & 7))))
                    this.pc = this.get_branch();

                break;
            }
            case BRBC: {

                if (!(this.sram[0x5F] & (1 << (this.opcode & 7))))
                    this.pc = this.get_branch();

                break;
            }
            case CPSE: {

                if (this.get_rd() == this.get_rr())
                    cycles = this.skip_instr();

                break;
            }
            case SBRC: {

                if (!(this.get_rd() & (1 << (this.opcode & 7))))
                    cycles = 1 + this.skip_instr();

                break;
            }
            case SBRS: {

                if (this.get_rd() & (1 << (this.opcode & 7)))
                    cycles = 1 + this.skip_instr();

                break;
            }
            case SBIS:
            case SBIC: {

                b = (this.opcode & 7);
                A = (this.opcode & 0xF8) >> 3;
                v = this.get(0x20 + A) & (1 << b);

                // Пропуск, если в порту Ap есть бит (или нет бита)
                if ((this.command == SBIS && v) || (this.command == SBIC && !v))
                    cycles = 1 + this.skip_instr();

                break;
            }

            // --------------------------------
            // Ввод-вывод
            // --------------------------------

            case IN: {

                this.put_rd(this.get(0x20 + this.get_ap()));
                cycles = 2;
                break;
            }
            case OUT: {

                this.put(0x20 + this.get_ap(), this.get_rd());
                cycles = 2;
                break;
            }
            case CBI:
            case SBI: {

                b = 1 << (this.opcode & 0x07);
                A = (this.opcode & 0xF8) >> 3;

                // Сброс/установка бита в I/O
                if (this.command == CBI)
                     this.put(0x20 + A, this.get(0x20 + A) & ~b);
                else this.put(0x20 + A, this.get(0x20 + A) |  b);

                cycles = 2;
                break;
            }

            // --------------------------------
            // Стек
            // --------------------------------

            case PUSH: {

                this.push8(this.get_rd());
                cycles = 1;
                break;
            }
            case POP: {

                this.put_rd(pop8());
                cycles = 2;
                break;
            }

            // --------------------------------
            // Специальные
            // --------------------------------

            case SWAP: {

                d = this.get_rd();
                this.put_rd(((d & 0x0F) << 4) + ((d & 0xF0) >> 4));
                break;
            }
            case BST: {

                this.flag.t = (this.get_rd() & (1 << (this.opcode & 7))) > 0 ? 1 : 0;
                this.flag_to_byte();
                break;
            }
            case BLD: {

                a = this.get_rd();
                b = (1 << (this.opcode & 7));
                this.put_rd( this.flag.t ? (a | b) : (a & ~b) );
                break;
            }

            // --------------------------------
            // Арифметико-логические инструкции
            // --------------------------------

            case CPC: {

                d = this.get_rd();
                r = this.get_rr();
                R = (d - r - this.flag.c) & 0xff;
                this.set_subcarry_flag(d, r, R, this.flag.c);
                break;
            }
            case SBC: {

                d = this.get_rd();
                r = this.get_rr();
                R = (d - r - this.flag.c) & 0xFF;
                this.set_subcarry_flag(d, r, R, this.flag.c);
                this.put_rd(R);
                break;
            }
            case ADD: {

                d = this.get_rd();
                r = this.get_rr();
                R = (d + r) & 0xff;
                this.set_add_flag(d, r, R, 0);
                this.put_rd(R);
                break;
            }
            case CP: {

                d = this.get_rd();
                r = this.get_rr();
                R = (d - r) & 0xFF;
                this.set_subtract_flag(d, r, R);
                break;
            }
            case SUB: {

                d = this.get_rd();
                r = this.get_rr();
                R = (d - r) & 0xFF;
                this.set_subtract_flag(d, r, R);
                this.put_rd(R);
                break;
            }
            case ADC: {

                d = this.get_rd();
                r = this.get_rr();
                R = (d + r + this.flag.c) & 0xff;
                this.set_add_flag(d, r, R, this.flag.c);
                this.put_rd(R);
                break;
            }
            case AND: {

                R = this.get_rd() & this.get_rr();
                this.set_logic_flags(R);
                this.put_rd(R);
                break;
            }
            case OR: {

                R = this.get_rd() | this.get_rr();
                this.set_logic_flags(R);
                this.put_rd(R);
                break;
            }
            case EOR: {

                R = this.get_rd() ^ this.get_rr();
                this.set_logic_flags(R);
                this.put_rd(R);
                break;
            }
            case ANDI: {

                R = this.get_rdi() & this.get_imm8();
                this.set_logic_flags(R);
                this.put_rdi(R);
                break;
            }
            case ORI: {

                R = this.get_rdi() | this.get_imm8();
                this.set_logic_flags(R);
                this.put_rdi(R);
                break;
            }
            case SUBI: {

                d = this.get_rdi();
                r = this.get_imm8();
                R = (d - r) & 0xFF;
                this.set_subtract_flag(d, r, R);
                this.put_rdi(R);
                break;
            }
            case SBCI: {

                d = this.get_rdi();
                r = this.get_imm8();
                R = (d - r - this.flag.c) & 0xFF;
                this.set_subcarry_flag(d, r, R, this.flag.c);
                this.put_rdi(R);
                break;
            }
            case CPI: {

                d = this.get_rdi();
                r = this.get_imm8();
                R = (d - r) & 0xFF;
                this.set_subtract_flag(d, r, R);
                break;
            }
            case COM: {

                d = this.get_rd();
                r = (d ^ 0xFF) & 0xFF;
                this.set_logic_flags(r);
                this.flag.c = 1;
                this.flag_to_byte();
                this.put_rd(r);
                break;
            }
            case DEC: {

                d = this.get_rd();
                r = (d - 1) & 0xff;
                this.put_rd(r);

                this.flag.v = (r == 0x7F) ? 1 : 0;
                this.flag.n = (r & 0x80) > 0 ? 1 : 0;
                this.flag.z = (r == 0x00) ? 1 : 0;
                this.flag.s = this.flag.v ^ this.flag.n;
                this.flag_to_byte();
                break;
            }
            case INC: {

                d = this.get_rd();
                r = (d + 1) & 0xff;
                this.put_rd(r);

                this.flag.v = (r == 0x80) ? 1 : 0;
                this.flag.n = (r & 0x80) > 0 ? 1 : 0;
                this.flag.z = (r == 0x00) ? 1 : 0;
                this.flag.s = this.flag.v ^ this.flag.n;
                this.flag_to_byte();
                break;
            }
            case ADIW: {

                d = 24 + ((this.opcode & 0x30) >> 3);
                a = this.get16(d);
                b = this.get_ka();
                r = a + b;
                this.set_adiw_flag(a, r);
                this.put16(d, r);
                break;
            }
            case SBIW: {

                d = 24 + ((this.opcode & 0x30) >> 3);
                a = this.get16(d);
                b = this.get_ka();
                r = a - b;
                this.set_sbiw_flag(a, r);
                this.put16(d, r);
                break;
            }
            case LSR: {

                d = this.get_rd();
                r = d >> 1;
                this.set_lsr_flag(d, r);
                this.put_rd(r);
                break;
            }
            case ASR: {

                d = this.get_rd();
                r = (d >> 1) | (d & 0x80);
                this.set_lsr_flag(d, r);
                this.put_rd(r);
                break;
            }
            case ROR: {

                d = this.get_rd();
                r = (d >> 1) | (this.flag.c ? 0x80 : 0x00);
                this.set_lsr_flag(d, r);
                this.put_rd(r);
                break;
            }
            case NEG: {

                d = this.get_rd();
                R = (-d) & 0xFF;
                this.set_subtract_flag(0, d, R);
                this.put_rd(R);
                break;
            }

            // --------------------------------
            // Перемещения
            // --------------------------------

            case LDI: {

                this.put_rdi(this.get_imm8());
                break;
            }
            case LPM0Z: {

                this.sram[0] = this.read_program(get_Z());
                cycles = 3;
                break;
            }
            case LPMRZ: {

                this.put_rd(this.read_program(get_Z()));
                cycles = 3;
                break;
            }
            case LPMRZ_: {

                p = get_Z();
                this.put_rd(this.read_program(p));
                this.put_Z(p + 1);
                cycles = 3;
                break;
            }
            case STX: {

                this.put(get_X(), this.get_rd());
                cycles = 2;
                break;
            }
            case STX_: {

                p = this.get_X();
                this.put(p, this.get_rd());
                this.put_X(p + 1);
                cycles = 2;
                break;
            }
            case ST_X: {

                p = this.get_X() - 1;
                this.put(p, this.get_rd());
                this.put_X(p);
                cycles = 2;
                break;
            }
            case STYQ: {

                this.put((get_Y() + this.get_qi()), this.get_rd());
                cycles = 2;
                break;
            }
            case STY_: {

                p = this.get_Y();
                this.put(p, this.get_rd());
                this.put_Y(p+1);
                cycles = 2;
                break;
            }
            case ST_Y: {

                p = this.get_Y() - 1;
                this.put(p, this.get_rd());
                this.put_Y(p);
                cycles = 2;
                break;
            }
            case STZQ: {

                this.put((this.get_Z() + this.get_qi()), this.get_rd());
                cycles = 2;
                break;
            }
            case STZ_: {

                p = this.get_Z();
                this.put(p, this.get_rd());
                this.put_Z(p+1);
                cycles = 2;
                break;
            }
            case ST_Z: {

                p = this.get_Z() - 1;
                this.put(p, this.get_rd());
                this.put_Z(p);
                cycles = 2;
                break;
            }
            case LDX: {

                this.put_rd(this.get(this.get_X()));
                break;
            }
            case LDX_: {

                p = this.get_X();
                this.put_rd(this.get(p));
                this.put_X(p+1);
                break;
            }
            case LD_X: {

                p = this.get_X() - 1;
                this.put_rd(this.get(p));
                this.put_X(p);
                break;
            }
            case LDYQ: {

                this.put_rd(this.get((this.get_Y() + this.get_qi())));
                cycles = 2;
                break;
            }
            case LDY_: {

                p = this.get_Y();
                this.put_rd(this.get(p));
                this.put_Y(p+1);
                cycles = 2;
                break;
            }
            case LD_Y: {

                p = this.get_Y() - 1;
                this.put_rd(this.get(p));
                this.put_Y(p);
                cycles = 2;
                break;
            }
            case LDZQ: {

                this.put_rd(this.get((this.get_Z() + this.get_qi())));
                cycles = 2;
                break;
            }
            case LDZ_: {

                p = this.get_Z();
                this.put_rd(this.get(p));
                this.put_Z(p+1);
                cycles = 2;
                break;
            }
            case LD_Z: {

                p = this.get_Z() - 1;
                this.put_rd(this.get(p));
                this.put_Z(p);
                cycles = 2;
                break;
            }
            case MOV: {

                this.put_rd(this.get_rr());
                break;
            }
            case MOVW: {

                r = (this.get_rr_index() & 0xF) << 1;
                d = (this.get_rd_index() & 0xF) << 1;
                this.put16(d, this.get16(r));
                break;
            }
            case LDS: {

                d = this.fetch();
                this.put_rd( this.get(d) );
                break;
            }
            case STS: {

                d = this.fetch();
                this.put(d, this.get_rd());
                break;
            }

            case ELPM0Z: {

                this.sram[0] = this.read_program(this.get_Z() + 65536*this.sram[0x5B]);
                cycles = 3;
                break;
            }
            case ELPMRZ: {

                this.put_rd(this.read_program(this.get_Z() + 65536*this.sram[0x5B]));
                break;
            }
            case ELPMRZ_: { // ELPM Z+

                p = this.get_Z() + 65536*this.sram[0x5B];
                this.put_rd(this.read_program(p));
                this.put_Z(p + 1);
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

                pc = this.get_Z();
                cycles = 2;
                break;
            }
            case EIJMP: {

                this.pc = (this.get_Z() + (this.sram[0x5B] << 16));
                cycles = 2;
                break;
            }
            case JMP: {

                this.pc = ((this.get_jmp() << 16) | this.fetch());
                cycles = 2;
                break;
            }
            case CALL: {

                this.push16(this.pc + 1);
                this.pc = ((this.get_jmp() << 16) | this.fetch());
                cycles = 2;
                break;
            }
            case ICALL: {

                this.push16(pc);
                this.pc = get_Z();
                cycles = 3;
                break;
            }

            // --------------------------------
            // Аппаратное умножение
            // --------------------------------

            case MUL: {

                d = this.get_rd();
                r = this.get_rr();
                v = (r * d) & 0xffff;
                this.put16(0, v);

                this.flag.c = v >> 15;
                this.flag.z = v == 0;
                this.flag_to_byte();
                break;
            }
            case MULS: {

                d = this.sram[ 0x10 | ((this.opcode & 0xf0) >> 4) ];
                r = this.sram[ 0x10 |  (this.opcode & 0x0f) ];
                d = (d & 0x80 ? 0xff00 : 0) | d;
                r = (r & 0x80 ? 0xff00 : 0) | r;
                v = (r * d) & 0xffff;
                this.put16(0, v);

                this.flag.c = v >> 15;
                this.flag.z = v == 0;
                this.flag_to_byte();
                break;
            }
            case MULSU: {

                d = sram[ 0x10 | ((this.opcode & 0x70)>>4) ];
                r = sram[ 0x10 |  (this.opcode & 0x07) ];
                d = (d & 0x80 ? 0xff00 : 0) | d; // Перевод в знаковый
                v = (r * d) & 0xffff;
                this.put16(0, v);

                this.flag.c = v >> 15;
                this.flag.z = v == 0;
                this.flag_to_byte();
                break;
            }
            case FMUL: { // Аналогично MUL, но со сдвигом влево

                d = this.sram[ 0x10 | ((this.opcode & 0x70)>>4) ];
                r = this.sram[ 0x10 |  (this.opcode & 0x07) ];
                v = ((r * d) << 1) & 0xffff;
                this.put16(0, v);

                this.flag.c = v >> 15;
                this.flag.z = v == 0;
                this.flag_to_byte();
                break;
            }

            default:

                console.log("Неизвестная инструкция " + this.opcode + " в pc=" + this.pc);
        }

        return cycles ? cycles : 1;
    }

    // Запись в память
    put(a, d) {

        this.sram[a] = d & 0xFF;
        this._put(a, d);
    }

    // Чтение из памяти
    get(a) {

        let v = this._get(a);
        if (v < 0) v = this.sram[a & 0xFFFF];
        return v & 255;
    }

    assign() {

        // Арифметические на 2 регистра
        this.assign_mask("000001rdddddrrrr", CPC);
        this.assign_mask("000010rdddddrrrr", SBC);
        this.assign_mask("000011rdddddrrrr", ADD);
        this.assign_mask("000101rdddddrrrr", CP);
        this.assign_mask("000110rdddddrrrr", SUB);
        this.assign_mask("000111rdddddrrrr", ADC);
        this.assign_mask("001000rdddddrrrr", AND);
        this.assign_mask("001001rdddddrrrr", EOR);
        this.assign_mask("001010rdddddrrrr", OR);

        // Арифметика регистр + операнд K
        this.assign_mask("0011KKKKddddKKKK", CPI);
        this.assign_mask("0100KKKKddddKKKK", SBCI);
        this.assign_mask("0101KKKKddddKKKK", SUBI);
        this.assign_mask("0110KKKKddddKKKK", ORI);
        this.assign_mask("0111KKKKddddKKKK", ANDI);
        this.assign_mask("10010110KKddKKKK", ADIW);
        this.assign_mask("10010111KKddKKKK", SBIW);

        // Условные и безусловные переходы
        this.assign_mask("1100kkkkkkkkkkkk", RJMP);
        this.assign_mask("1101kkkkkkkkkkkk", RCALL);
        this.assign_mask("1001010100001000", RET);
        this.assign_mask("1001010100011000", RETI);
        this.assign_mask("111100kkkkkkksss", BRBS);
        this.assign_mask("111101kkkkkkksss", BRBC);
        this.assign_mask("1111110ddddd0bbb", SBRC);
        this.assign_mask("1111111ddddd0bbb", SBRS);
        this.assign_mask("10011001AAAAAbbb", SBIC);
        this.assign_mask("10011011AAAAAbbb", SBIS);
        this.assign_mask("000100rdddddrrrr", CPSE);

        // Непрямые и длинные переходы
        this.assign_mask("1001010100001001", ICALL);
        this.assign_mask("1001010100011001", EICALL);
        this.assign_mask("1001010000001001", IJMP);
        this.assign_mask("1001010000011001", EIJMP);
        this.assign_mask("1001010kkkkk111k", CALL);
        this.assign_mask("1001010kkkkk110k", JMP);

        // Перемещения
        this.assign_mask("1110KKKKddddKKKK", LDI);
        this.assign_mask("001011rdddddrrrr", MOV);
        this.assign_mask("1001000ddddd0000", LDS);
        this.assign_mask("1001001ddddd0000", STS);
        this.assign_mask("00000001ddddrrrr", MOVW);
        this.assign_mask("1111100ddddd0bbb", BLD);
        this.assign_mask("1111101ddddd0bbb", BST);
        this.assign_mask("1001001ddddd0100", XCH);

        // Однооперандные
        this.assign_mask("1001010ddddd0011", INC);
        this.assign_mask("1001010ddddd1010", DEC);
        this.assign_mask("1001010ddddd0110", LSR);
        this.assign_mask("1001010ddddd0101", ASR);
        this.assign_mask("1001010ddddd0111", ROR);
        this.assign_mask("1001010ddddd0001", NEG);
        this.assign_mask("1001010ddddd0000", COM);
        this.assign_mask("1001010ddddd0010", SWAP);
        this.assign_mask("1001001ddddd0110", LAC);
        this.assign_mask("1001001ddddd0101", LAS);
        this.assign_mask("1001001ddddd0111", LAT);
        this.assign_mask("100101001sss1000", BCLR);
        this.assign_mask("100101000sss1000", BSET);
        this.assign_mask("10011000AAAAAbbb", CBI);
        this.assign_mask("10011010AAAAAbbb", SBI);

        // Косвенная загрузка из памяти
        this.assign_mask("1001000ddddd1100", LDX);
        this.assign_mask("1001000ddddd1101", LDX_);
        this.assign_mask("1001000ddddd1110", LD_X);
        this.assign_mask("1001000ddddd1001", LDY_);
        this.assign_mask("1001000ddddd1010", LD_Y);
        this.assign_mask("10q0qq0ddddd1qqq", LDYQ);
        this.assign_mask("1001000ddddd0001", LDZ_);
        this.assign_mask("1001000ddddd0010", LD_Z);
        this.assign_mask("10q0qq0ddddd0qqq", LDZQ);

        // Косвенное сохранение
        this.assign_mask("1001001ddddd1100", STX);
        this.assign_mask("1001001ddddd1101", STX_);
        this.assign_mask("1001001ddddd1110", ST_X);
        this.assign_mask("1001001ddddd1001", STY_);
        this.assign_mask("1001001ddddd1010", ST_Y);
        this.assign_mask("10q0qq1ddddd1qqq", STYQ);
        this.assign_mask("1001001ddddd0001", STZ_);
        this.assign_mask("1001001ddddd0010", ST_Z);
        this.assign_mask("10q0qq1ddddd0qqq", STZQ);

        // Загрузка из запись в память программ
        this.assign_mask("1001010111001000", LPM0Z);
        this.assign_mask("1001000ddddd0100", LPMRZ);
        this.assign_mask("1001000ddddd0101", LPMRZ_);
        this.assign_mask("1001010111101000", SPM);
        this.assign_mask("1001010111111000", SPM2);

        // Особые инструкции расширенной загрузки из памяти
        this.assign_mask("1001010111011000", ELPM0Z);
        this.assign_mask("1001000ddddd0110", ELPMRZ);
        this.assign_mask("1001000ddddd0111", ELPMRZ_);

        // Специальные
        this.assign_mask("1001010110001000", SLEEP);
        this.assign_mask("1001010110101000", WDR);
        this.assign_mask("1001010110011000", BREAK);
        this.assign_mask("0000000000000000", NOP);
        this.assign_mask("10110AAdddddAAAA", IN);
        this.assign_mask("10111AAdddddAAAA", OUT);
        this.assign_mask("1001001ddddd1111", PUSH);
        this.assign_mask("1001000ddddd1111", POP);
        this.assign_mask("10010100KKKK1011", DES);

        // Расширенные
        this.assign_mask("100111rdddddrrrr", MUL);
        this.assign_mask("00000010ddddrrrr", MULS);
        this.assign_mask("000000110ddd0rrr", MULSU);
        this.assign_mask("000000110ddd1rrr", FMUL);
    }

    // Применить опкоды по маске
    assign_mask(mask, opcode) {

        let i, j;
        let mk = new Array(16), nmask = 0, baseop = 0;

        // Расчет позиции, где находится маска
        for (i = 0; i < 16; i++) {

            let ch = mask.substr(15-i,1);
            if (ch == '0' || ch == '1') {
                baseop |= (ch == '1' ? 1 << i : 0);
            } else {
                mk[nmask++] = i;
            }
        }

        // Перебрать 2^nmask вариантов, по количеству битов и назначить всем опкод
        for (i = 0; i < (1 << nmask); i++) {

            let instn = baseop;
            for (j = 0; j < nmask; j++) if (i & (1 << j)) instn |= (1 << mk[j]);
            this.map[instn] = opcode;
        }
    }

    // Байт во флаги
    byte_to_flag(f) {

        this.flag.c = (f >> 0) & 1;
        this.flag.z = (f >> 1) & 1;
        this.flag.n = (f >> 2) & 1;
        this.flag.v = (f >> 3) & 1;
        this.flag.s = (f >> 4) & 1;
        this.flag.h = (f >> 5) & 1;
        this.flag.t = (f >> 6) & 1;
        this.flag.i = (f >> 7) & 1;
    };

    // Флаги в байты
    flag_to_byte() {

        let f =
            (this.flag.i<<7) |
            (this.flag.t<<6) |
            (this.flag.h<<5) |
            (this.flag.s<<4) |
            (this.flag.v<<3) |
            (this.flag.n<<2) |
            (this.flag.z<<1) |
            (this.flag.c<<0);

        this.sram[0x5F] = f;
        return f;
    }

    // Развернуть итоговые биты
    neg(n) {
        return n ^ 0xffff;
    }

    // Установка флагов
    set_logic_flags(r) {

        this.flag.v = 0;
        this.flag.n = (r & 0x80) ? 1 : 0;
        this.flag.s = this.flag.n;
        this.flag.z = (r & 0xFF == 0) ? 1 : 0;
        this.flag_to_byte();
    }

    // Флаги после вычитания с переносом
    set_subcarry_flag(d, r, R, carry) {

        d &= 0xFF; r &= 0xFF; R &= 0xFF;
        let _d = this.neg(d), _r = this.neg(r), _R = this.neg(R);

        this.flag.c = (((_d & r) | (r & R) | (R & _d)) >> 7) & 1;
        this.flag.z = (R == 0 && this.flag.z) ? 1 : 0;
        this.flag.n = (R & 0x80) > 1 ? 1 : 0;
        this.flag.v = (((d & _r & _R) | (_d & r & R)) & 0x80) > 0 ? 1 : 0;
        this.flag.s = this.flag.n ^ this.flag.v;
        this.flag.h = (((_d & r) | (r & R) | (R & _d)) & 0x80) > 0 ? 1 : 0;
        this.flag_to_byte();
    }

    // Флаги после вычитания
    set_subtract_flag(d, r, R) {

        d &= 0xFF; r &= 0xFF; R &= 0xFF;
        let _d = this.neg(d), _r = this.neg(r), _R = this.neg(R);

        this.flag.c = d < r ? 1 : 0;
        this.flag.z = (R & 0xFF) == 0 ? 1 : 0;
        this.flag.n = (R & 0x80) > 1 ? 1 : 0;
        this.flag.v = (((d & _r & _R) | (_d & r & R)) & 0x80) > 0 ? 1 : 0;
        this.flag.s = this.flag.n ^ this.flag.v;
        this.flag.h = (((_d & r) | (r & R) | (R & _d)) & 0x40) > 0 ? 1 : 0;
        this.flag_to_byte();
    }

    // Флаги после сложение с переносом
    set_add_flag(d, r, R, carry) {

        d &= 0xFF; r &= 0xFF; R &= 0xFF;
        let _d = this.neg(d), _r = this.neg(r), _R = this.neg(R);

        this.flag.c = d + r + carry >= 0x100;
        this.flag.h = (((d & r) | (r & _R) | (_R & d)) & 0x08) > 0 ? 1 : 0;
        this.flag.z = R == 0 ? 1 : 0;
        this.flag.n = (R & 0x80) > 0 ? 1 : 0;
        this.flag.v = (((d & r & _R) | (_d & _r & R)) & 0x80) > 0 ? 1 : 0;
        this.flag.s = this.flag.n ^ this.flag.v;
        this.flag_to_byte();
    }

    // Флаги после логической операции сдвига
    set_lsr_flag(d, r) {

        d &= 0xFF; r &= 0xFF;

        this.flag.c = d & 1;
        this.flag.n = (r & 0x80) > 0 ? 1 : 0;
        this.flag.z = (r == 0x00) ? 1 : 0;
        this.flag.v = this.flag.n ^ this.flag.c;
        this.flag.s = this.flag.n ^ this.flag.v;
        this.flag_to_byte();
    }

    // Флаги после сложения 16 бит
    set_adiw_flag(a, r) {

        a &= 0xFF; r &= 0xFF;

        this.flag.v = ((this.neg(a) & r) >> 15) & 1;
        this.flag.c = ((this.neg(r) & a) >> 15) & 1;
        this.flag.n = (r & 0x8000) > 0 ? 1 : 0;
        this.flag.z = (r & 0xFFFF) == 0 ? 1 : 0;
        this.flag.s = this.flag.v ^ this.flag.n;
        this.flag_to_byte();
    }

    // Флаги после вычитания 16 бит
    set_sbiw_flag(a, r) {

        a &= 0xFF; r &= 0xFF;

        this.flag.v = ((this.neg(r) & a) >> 15) & 1;
        this.flag.c = ((this.neg(a) & r) >> 15) & 1;
        this.flag.n = (r & 0x8000) > 0 ? 1 : 0;
        this.flag.z = (r & 0xFFFF) == 0 ? 1 : 0;
        this.flag.s = this.flag.v ^ this.flag.n;
        this.flag_to_byte();
    }

    // В зависимости от инструкции CALL/JMP/LDS/STS
    skip_instr() {

        switch (this.map[ this.fetch() ]) {

            case CALL:
            case JMP:
            case LDS:
            case STS:

                this.pc += 1;
                break;
        }

        return 2;
    }

    // Вызов прерывания
    interruptcall() {

        this.flag.i = 0;
        this.flag_to_byte();
        this.push16(pc);
        this.pc = 2;
    }

    // Прочесть следующую инструкцию
    fetch() {

        let pc  = this.program[this.pc];
        this.pc = (this.pc + 1) & 0xFFFF;
        return pc;
    }

    // Извлечение операндов
    get_rd_index() {
        return (this.opcode & 0x1F0) >> 4;
    }

    get_rr_index() {
        return (this.opcode & 0x00F) | ((this.opcode & 0x200)>>5);
    }

    get_rd() {
        return this.sram[ this.get_rd_index() ];
    }

    get_rr()  {
        return this.sram[ this.get_rr_index() ];
    }

    get_rdi() {
        return this.sram[ this.get_rd_index() | 0x10 ];
    }

    get_rri() {
        return this.sram[ this.get_rr_index() | 0x10 ];
    }

    get_imm8() {
        return (this.opcode & 0xF) + ((this.opcode & 0xF00) >> 4);
    }

    get_ap() {
        return (this.opcode & 0x00F) | ((this.opcode & 0x600) >> 5);
    }

    get_ka() {
        return (this.opcode & 0x00F) | ((this.opcode & 0x0C0) >> 2);
    }

    get_qi() {
        return (this.opcode & 0x007) | ((this.opcode & 0xC00) >> 7) | ((this.opcode & 0x2000) >> 8);
    }

    get_s3() {
        return (this.opcode & 0x070) >> 4;
    }

    get_jmp() {
        return (this.opcode & 1) | ((this.opcode & 0x1F0) >> 3);
    }

    // 16 bit
    get_S() {
        return this.sram[0x5D] + this.sram[0x5E]*256;
    }

    get_X() {
        return this.sram[0x1A] + this.sram[0x1B]*256;
    }

    get_Y() {
        return this.sram[0x1C] + this.sram[0x1D]*256;
    }

    get_Z() {
        return this.sram[0x1E] + this.sram[0x1F]*256;
    }

    put_S(a) {
        this.sram[0x5D] = a & 0xFF; this.sram[0x5E] = (a >> 8) & 0xFF;
    }

    put_X(a) {
        this.sram[0x1A] = a & 0xFF; this.sram[0x1B] = (a >> 8) & 0xFF;
    }

    put_Y(a) {
        this.sram[0x1C] = a & 0xFF; this.sram[0x1D] = (a >> 8) & 0xFF;
    }

    put_Z(a) {
        this.sram[0x1E] = a & 0xFF; this.sram[0x1F] = (a >> 8) & 0xFF;
    }

    put16(a, v) {
        this.put(a, v);
        this.put(a + 1, v >> 8);
    }

    get16(a) {
        return this.sram[a] + 256*this.sram[a+1];
    }

    put_rd(value) {
        this.sram[this.get_rd_index()] = value & 0xFF;
    }

    put_rr(value) {
        this.sram[this.get_rr_index()] = value & 0xFF;
    }

    put_rdi(value) {
        this.sram[this.get_rd_index() | 0x10] = value & 0xFF;
    }

    // Работа со стеком
    push8(v8) {

        let sp = this.get_S();
        this.put(sp, v8);
        this.put_S((sp - 1) & 0xffff);
    }

    pop8() {

        let sp = (this.get_S() + 1) & 0xffff;
        this.put_S(sp);
        return this.get(sp);
    }

    push16(v16) {

        this.push8(v16 & 0xff);
        this.push8(v16 >> 8);
    }

    pop16() {

        let h = this.pop8();
        let l = this.pop8();
        return h*256 + l;
    }

    get_rjmp() {

        return  (this.pc + ((this.opcode & 0x800) > 0 ?
                ((this.opcode & 0x7FF) - 0x800) :
                (this.opcode & 0x7FF))) & 0xFFFF;
    }

    get_branch() {

        return  (this.pc + ((this.opcode & 0x200) > 0 ?
                ((this.opcode & 0x1F8) >> 3) - 0x40 :
                ((this.opcode & 0x1F8) >> 3) )) & 0xFFFF;
    }

    read_program(a) {

        let m = a >> 1;
        let v = this.program[m];
        return (a & 1 ? (v >> 8) : v) & 0xFF;
    }
}
