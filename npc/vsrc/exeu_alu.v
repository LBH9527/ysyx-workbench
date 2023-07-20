
`include "defines.v"
// alu unit


module exeu_alu(
  input wire [`REG_BUS]alu_src_a,
  input wire [`REG_BUS]alu_src_b,
  input wire [3:0]  alu_ctrl,
  output reg  [`REG_BUS]alu_out
);

wire signed [`REG_BUS] signed_a;
wire signed [`REG_BUS] signed_b;

assign signed_a = alu_src_a;
assign signed_b = alu_src_b;

always@(*)
begin
  case (alu_ctrl)
    `ALU_CTRL_ADD : alu_out = alu_src_a + alu_src_b;
    // `ALU_CTRL_SUB : ALUout <= ALUSrcA - ALUSrcB;
    // `ALU_CTRL_SLL : ALUout <=  ALUSrcA <<  ALUSrcB[4:0] ;          //srcA << srcB[4:0], 
    // `ALU_CTRL_SLT : ALUout <= (signed_a < signed_b) ? 1 : 0;
    // `ALU_CTRL_SLTU : ALUout <= (ALUSrcA < ALUSrcB) ? 1: 0;
    // `ALU_CTRL_XOR : ALUout <= ALUSrcA ^ ALUSrcB;
    // `ALU_CTRL_SRL : ALUout <= ALUSrcA >> ALUSrcB[4:0];
    // `ALU_CTRL_SRA : ALUout <= signed_a >>> signed_b[4:0];
    // `ALU_CTRL_OR  : ALUout <= ALUSrcA | ALUSrcB;
    // `ALU_CTRL_AND : ALUout <= ALUSrcA & ALUSrcB;

    default : alu_out = 0;
	endcase
end 

endmodule

