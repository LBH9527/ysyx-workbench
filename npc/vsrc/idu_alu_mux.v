`include "defines.v"

module idu_alu_mux(
    input wire  [`REG_BUS]  rs2_data,
    input wire  [`REG_BUS]  imm,
    input wire              alu_b_sel,
    output reg  [`REG_BUS] alu_src_b_o
);

always @(*) begin
    if ( alu_b_sel == `ALUB_SRC_IMM ) 
        alu_src_b_o = imm;
    else 
        alu_src_b_o = rs2_data;
end
endmodule
