
/*
register file
*/
`include "defines.v"

module regfile (
    input  wire clk,
	input  wire rst,

	// idu ctrl ---> regfile
	input  wire 		  	register_write_enable,	// write enable
	input  wire  [4  : 0] 	register_write_addr,			// write reg addr
	input  wire  [`REG_BUS] register_write_data,				// write data
	
	input  wire  [4  : 0] 	read_register_1,		// read reg addr
	output reg   [`REG_BUS] rs1_data,			// [out] read data
	
	input  wire  [4  : 0] 	read_register_2,		// read reg addr
	output reg   [`REG_BUS] rs2_data			// [out] read data
);

    // 32 registers
	reg [`REG_BUS] 	regs[0 : 31];
	
	always @(posedge clk) 
	begin
		if ( rst == 1'b1 ) 
		begin
			regs[ 0] <= `ZERO_WORD;
			regs[ 1] <= `ZERO_WORD;
			regs[ 2] <= `ZERO_WORD;
			regs[ 3] <= `ZERO_WORD;
			regs[ 4] <= `ZERO_WORD;
			regs[ 5] <= `ZERO_WORD;
			regs[ 6] <= `ZERO_WORD;
			regs[ 7] <= `ZERO_WORD;
			regs[ 8] <= `ZERO_WORD;
			regs[ 9] <= `ZERO_WORD;
			regs[10] <= `ZERO_WORD;
			regs[11] <= `ZERO_WORD;
			regs[12] <= `ZERO_WORD;
			regs[13] <= `ZERO_WORD;
			regs[14] <= `ZERO_WORD;
			regs[15] <= `ZERO_WORD;
			regs[16] <= `ZERO_WORD;
			regs[17] <= `ZERO_WORD;
			regs[18] <= `ZERO_WORD;
			regs[19] <= `ZERO_WORD;
			regs[20] <= `ZERO_WORD;
			regs[21] <= `ZERO_WORD;
			regs[22] <= `ZERO_WORD;
			regs[23] <= `ZERO_WORD;
			regs[24] <= `ZERO_WORD;
			regs[25] <= `ZERO_WORD;
			regs[26] <= `ZERO_WORD;
			regs[27] <= `ZERO_WORD;
			regs[28] <= `ZERO_WORD;
			regs[29] <= `ZERO_WORD;
			regs[30] <= `ZERO_WORD;
			regs[31] <= `ZERO_WORD;
		end
		else 
		begin
			if ((register_write_enable == `REG_WRITE_ENABLE) && (register_write_addr != `REGISTER_X0))	
				regs[register_write_addr] <= register_write_data;
		end
	end

	always @(*) begin
		if (rst == 1'b1)
			rs1_data = `ZERO_WORD;
		else
			rs1_data = regs[read_register_1];
	end
	
	always @(*) begin
		if (rst == 1'b1)
			rs2_data = `ZERO_WORD;
		else
			rs2_data = regs[read_register_2];
	end
	// always @(*) begin
	// 	if (rst == 1'b1)
	// 		read_data_1 = `ZERO_WORD;
	// 	else if (r_ena1 == 1'b1)
	// 		read_data_1 = regs[read_register_1];
	// 	else
	// 		read_data_1 = `ZERO_WORD;
	// end
	
	// always @(*) begin
	// 	if (rst == 1'b1)
	// 		read_data_2 = `ZERO_WORD;
	// 	else if (r_ena2 == 1'b1)
	// 		read_data_2 = regs[read_register_2];
	// 	else
	// 		read_data_2 = `ZERO_WORD;
	// end

endmodule

