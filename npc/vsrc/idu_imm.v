
`include "defines.v"

module idu_imm(
  input  wire [31 : 0]inst,   
  output reg [`REG_BUS] imm_o
);

wire	[6:0] opcode;
assign	opcode = inst[6:0];
//?????
always@(*)
begin
	case(opcode)
        7'b0010111 :    imm_o = { {32 {inst[31]} }, inst[31:12], 12'b0000_0000_0000 };  //auipc             // U
        7'b0110111 :    imm_o = { {32 {inst[31]} }, inst[31:12], 12'b0000_0000_0000 };  //lui               // U
        7'b1100011 :    imm_o = { {51 {inst[31]} }, inst[31], inst[7], inst[30:25], inst[11:8], 1'b0};            // B 19+13
        7'b1101111 :    imm_o = { {43 {inst[31]} }, inst[31], inst[19:12], inst[20], inst[30:21] , 1'b0};         //jal
        7'b1100111 :    imm_o = { {52 {inst[31]} }, inst[31:20] };                //jalr
        7'b0000011 :    imm_o = { {52 {inst[31]} }, inst[31:20] };                //I
        7'b0100011 :    imm_o = { {52 {inst[31]} }, inst[31:25], inst[11:7] };    //S             
        7'b0010011 :    imm_o = { {52 {inst[31]} }, inst[31:20] };                //I             20 + 12
        default imm_o = `ZERO_WORD;
	endcase
end 

endmodule

