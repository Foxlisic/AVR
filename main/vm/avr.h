#include <SDL2/SDL.h>

static const char* ds_brcs[4][8] = {
    {"cc", "ne", "pl", "vc", "ge", "hc", "tc", "id"},
    {"cs", "eq", "mi", "vs", "lt", "hs", "ts", "ie"},
    {"sec","sez","sen","sev","ses","seh","set","sei"},
    {"clc","clz","cln","clv","cls","clh","clt","cli"}
};

struct CPUFlags {
    int c, z, n, v, s, h, t, i;
};

enum AVROpcodes {

    UNDEFINED = 0,
    CPC,    SBC,    ADD,    CP,     SUB,    ADC,    AND,    EOR,    OR,
    CPI,    SBCI,   SUBI,   ORI,    ANDI,   ADIW,   SBIW,
    RJMP,   RCALL,  RET,    RETI,   BRBS,   BRBC,
    SBRC,   SBRS,   SBIC,   SBIS,   CPSE,
    ICALL,  EICALL, IJMP,   EIJMP,  CALL,   JMP,
    LDI,    MOV,    LDS,    STS,    MOVW,   BLD,    BST,    XCH,
    INC,    DEC,    LSR,    ASR,    ROR,    NEG,    COM,    SWAP,   LAC,
    LAS,    LAT,    BCLR,   BSET,   CBI,    SBI,
    LDX,    LDX_,   LD_X,   LDY_,   LD_Y,   LDYQ,   LDZ_,   LD_Z,   LDZQ,
    STX,    STX_,   ST_X,   STY_,   ST_Y,   STYQ,   STZ_,   ST_Z,   STZQ,
    LPM0Z,  LPMRZ,  LPMRZ_, ELPM0Z, ELPMRZ, ELPMRZ_, SPM,   SPM2,
    SLEEP,  WDR,    BREAK,  NOP,    IN,     OUT,    PUSH,   POP,    DES
};

class AVR
{
protected:

    SDL_Surface*        screen_surface;
    SDL_Window*         sdl_window;
    SDL_Renderer*       sdl_renderer;
    SDL_PixelFormat*    sdl_pixel_format;
    SDL_Texture*        sdl_screen_texture;
    SDL_Event           evt;
    Uint32*             screen_buffer;

    // Обработка фрейма
    int width, height, scale, frame_length, pticks;
    int x, y, _hs, _vs;

    // Отладчик
    char        ds_line[256];

    // Процессор
    int         instr_counter, cpu_halt, cycles;
    uint16_t    pc;
    CPUFlags    flag;
    uint16_t    program[65536];
    uint8_t     sram[65536];
    int         map[65536];
    uint16_t    opcode, command;

public:

    AVR(int argc, char** argv);

    int main();
    int destroy();
    void pset(int x, int y, Uint32 cl);

    // Объявление процессора
    uint8_t     get(int addr);
    void        put(int addr, uint8_t value);
    void        byte_to_flag(uint8_t f);
    uint8_t     flag_to_byte();
    uint16_t    neg(uint16_t n);
    void        set_logic_flags(uint8_t r);
    void        set_subcarry_flag(int d, int r, int R, int carry);
    void        set_subtract_flag(int d, int r, int R);
    void        set_add_flag(int d, int r, int R, int carry);
    void        set_lsr_flag(int d, int r);
    void        set_adiw_flag(int a, int r);
    void        set_sbiw_flag(int a, int r);
    int         skip_instr();
    void        interruptcall();
    int         step();
    void        assign_mask(const char* mask, int opcode);
    void        assign();
    int         ds_fetch(uint& addr);
    int         ds_info(uint addr);

    // Извлечение операндов
    int         get_rd_index() { return (opcode & 0x1F0) >> 4; }
    int         get_rr_index() { return (opcode & 0x00F) | ((opcode & 0x200)>>5); }
    int         get_rd()    { return sram[ get_rd_index() ]; }
    int         get_rr()    { return sram[ get_rr_index() ]; }
    int         get_rdi()   { return sram[ get_rd_index() | 0x10 ]; }
    int         get_rri()   { return sram[ get_rr_index() | 0x10 ]; }
    int         get_imm8()  { return (opcode & 0xF) + ((opcode & 0xF00) >> 4); }
    int         get_ap()    { return (opcode & 0x00F) | ((opcode & 0x600) >> 5); }
    int         get_ka()    { return (opcode & 0x00F) | ((opcode & 0x0C0) >> 2); }
    int         get_qi()    { return (opcode & 0x007) | ((opcode & 0xC00) >> 7) | ((opcode & 0x2000) >> 8); }
    int         get_s3()    { return (opcode & 0x070) >> 4; }
    int         get_jmp()    { return (opcode & 1) | ((opcode & 0x1F0) >> 3); }

    // 16 bit
    uint16_t    fetch() { int data = program[pc] + program[pc+1]*256; pc = (pc + 2) & 0xffff; return data; }
    uint16_t    get_S() { return sram[0x5D] + sram[0x5E]*256; }
    uint16_t    get_X() { return sram[0x1A] + sram[0x1B]*256; }
    uint16_t    get_Y() { return sram[0x1C] + sram[0x1D]*256; }
    uint16_t    get_Z() { return sram[0x1E] + sram[0x1F]*256; }

    void        put_S(uint16_t a) { sram[0x5D] = a & 0xFF; sram[0x5E] = (a >> 8) & 0xFF; }
    void        put_X(uint16_t a) { sram[0x1A] = a & 0xFF; sram[0x1B] = (a >> 8) & 0xFF; }
    void        put_Y(uint16_t a) { sram[0x1C] = a & 0xFF; sram[0x1D] = (a >> 8) & 0xFF; }
    void        put_Z(uint16_t a) { sram[0x1E] = a & 0xFF; sram[0x1F] = (a >> 8) & 0xFF; }

    void        put16(int a, uint16_t v) { sram[a] = v & 0xff; sram[a+1] = (v >> 8) & 0xff; }
    uint16_t    get16(int a)             { return sram[a] + 256*sram[a+1]; }

    void        put_rd(uint8_t value)   { sram[get_rd_index()] = value & 0xff; }
    void        put_rr(uint8_t value)   { sram[get_rr_index()] = value & 0xff; }
    void        put_rdi(uint8_t value)  { sram[get_rd_index() | 0x10] = value & 0xff; }

    // Работа со стеком
    void        push8(uint8_t v8)    { uint16_t sp = get_S(); put(sp, v8); put_S((sp - 1) & 0xffff); }
    void        push16(uint16_t v16) { push8(v16 & 0xff); push8(v16 >> 8); }
    uint8_t     pop8()  { uint16_t sp = (get_S() + 1) & 0xffff; put_S(sp); return get(sp); }
    uint16_t    pop16() { int h = pop8(); int l = pop8(); return h*256 + l; }

    // Относительные переходы
    int         get_rjmp()   { return (pc + 2*((opcode & 0x800) > 0 ? ((opcode & 0x7FF) - 0x800) : (opcode & 0x7FF))) & 0x1FFFF; }
    int         get_branch() { return (pc + 2*((opcode & 0x200) > 0 ? ((opcode & 0x1F8)>>3) - 0x40 : ((opcode & 0x1F8)>>3) )) & 0x1FFFF; }

};
