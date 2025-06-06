
initial begin we = 0; dataw = 0; pc = 0; tstate = 0; end
// -----------------------------------------------------------------------------

wire [7:0] __r16 = regs[16];
wire [7:0] __r17 = regs[17];

// -----------------------------------------------------------------------------
reg [15:0]  ir;
reg [15:0]  opcode;
reg [ 1:0]  tstate;
reg [ 7:0]  sreg = 0;
            // ITHSVNZC
            // 76543210
// -----------------------------------------------------------------------------
reg [ 7:0]  regs[32];
// -----------------------------------------------------------------------------
wire [4:0]  Rdi = ir[8:4];
wire [4:0]  Rri = {ir[9], ir[3:0]};
wire [7:0]  Rd  = regs[Rdi];
wire [7:0]  Rr  = regs[Rri];
// -----------------------------------------------------------------------------
