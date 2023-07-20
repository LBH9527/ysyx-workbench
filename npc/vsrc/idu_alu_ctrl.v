
`include "defines.v"

module idu_alu_ctrl(
  input wire [1:0]alu_op,
  input wire [2:0]funct3,

  input wire [6:0]funct7,
  
  output reg [3:0]alu_ctrl_o
);

wire is_sub_operation;		//subtraction operation

assign  is_sub_operation  = (alu_op[1] == 1) ? ( (funct7 != 7'b0000000) ? 1 : 0 ) : 0
							 ;

	always @(*)
	begin
		case (funct3)
			3'b000:
			begin
				if( is_sub_operation )
					alu_ctrl_o = `ALU_CTRL_SUB;
				else
					alu_ctrl_o = `ALU_CTRL_ADD;
			end
			3'b001: alu_ctrl_o = `ALU_CTRL_SLL;
			3'b010: alu_ctrl_o = `ALU_CTRL_SLT;
			3'b011: alu_ctrl_o = `ALU_CTRL_SLTU;
			3'b100: alu_ctrl_o = `ALU_CTRL_XOR;
			3'b101: 
			begin
				if(funct7 != 7'b0000000)
					alu_ctrl_o = `ALU_CTRL_SRA;
					else
					alu_ctrl_o = `ALU_CTRL_SRL;
			end
			3'b110: alu_ctrl_o = `ALU_CTRL_OR;
			3'b111: alu_ctrl_o = `ALU_CTRL_AND;
            
			default:alu_ctrl_o = `ALU_CTRL_ADD;
		endcase
	end

endmodule

