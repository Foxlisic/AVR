/*
 * Эмулятора работы UFM
 */

module ufm_emu
(
    // clock
    input           oscena,
    output          osc,
    // address
    input           arclk,
    input           ardin,
    input           arshft,
    // data
    input           drclk,
    input           drdin,
    input           drshft,
    output          drdout,
    input           prgram
);

// ---------------------------------------------------------------------
reg [15:0] ufm[512];

initial begin $readmemh("ufm.hex", ufm, 0); end
// ---------------------------------------------------------------------

assign drdout = datareg[15];

// 512 x 16 = 1024 байт памяти
reg [ 8:0] address;
reg [15:0] datareg;

// Вдвиг адреса или увеличение на +1
always @(posedge arclk) 
    address <= arshft ? {address[7:0], ardin} : address + 1;

always @(posedge drclk or posedge prgram)
if (prgram)
    ufm[address] <= datareg;
else if (drshft)
    datareg <= {datareg[14:0], drdin};
else
    datareg <= ufm[address];

endmodule
