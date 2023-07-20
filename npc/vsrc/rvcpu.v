
//--xuezhen--

`timescale 1ns / 1ps

`include "defines.v"

//ebreak in verilog
import "DPI-C" function void ebreak();
// module EBREAK(
//     input wire [31:0] inst_i
// );
// always @(*) begin
//     if(inst_i == `INST_EBREAK) 
//     ebreak();       
// end
// endmodule


module rvcpu(
  input wire            clk,
  input wire            rst,
  input wire  [31 : 0]  inst,
  
  output wire [63 : 0]  inst_addr, 
  output wire           inst_ena
);


// id_stage
// id_stage -> regfile
// wire rs1_r_ena;
wire [4 : 0]rs1_r_addr;
// wire rs2_r_ena;
wire [4 : 0]rs2_r_addr;

wire reg_write_enable;
wire [4 : 0]reg_write_addr;
wire  [`REG_BUS] register_write_data;
// id_stage -> exe_stage
// wire [4 : 0]inst_type;      // instruction type
// wire [7 : 0]inst_opcode;

wire [3 : 0]ALUCtrl;
wire [`REG_BUS]ALUSrcA;
wire [`REG_BUS]ALUSrcB;


// regfile -> id_stage
wire [`REG_BUS] rs1_data;
wire [`REG_BUS] rs2_data;

// exe_stage
// exe_stage -> other stage
// exe_stage -> regfile
wire [`REG_BUS]ALUout;

assign rs1_r_addr = inst[19 : 15];
assign rs2_r_addr = inst[24 : 20];


always @(*) begin
    if(inst == `INST_EBREAK) 
    ebreak();       
end





ifu ifu_inst(
  .clk(clk),
  .rst(rst),
  
  .inst_addr(inst_addr),
  .inst_ena(inst_ena)
);

// idu_ctrl idu_ctrl_inst(
//   .inst(inst),
//   .rs1_data_i(r_data1),
//   .rs2_data_i(r_data2),
  
//   // // .rs1_r_ena(rs1_r_ena),
//   // .rs1_r_addr(rs1_r_addr),
//   // .rs2_r_ena(rs2_r_ena),
//   // .rs2_r_addr(rs2_r_addr),
//   .reg_write_enable(reg_write_enable),  // out
//   .reg_write_addr(reg_write_enable),
//   .alu_ctrl(ALUCtrl),
//   // .inst_opcode_o(inst_opcode),
//   .alu_src_a(ALUSrcA),
//   .alu_src_b(ALUSrcB)
// );

idu_ctrl_main idu_ctrl_main_inst(
  .inst(inst),
  .rs1_data(rs1_data),
  .rs2_data(rs2_data),

  .alu_ctrl(ALUCtrl),
  .alu_src_a(ALUSrcA),
  .alu_src_b(ALUSrcB),

  .reg_write_enable(reg_write_enable),  // out
  .reg_write_addr(reg_write_addr)
);



exeu_alu exeu_alu_inst(
  // .rst(rst),
  // .inst_type_i(inst_type),
  // .inst_opcode_i(inst_opcode),
  .alu_src_a(ALUSrcA),
  .alu_src_b(ALUSrcB),
  
  .alu_ctrl(ALUCtrl),
  .alu_out(ALUout)
);

assign register_write_data = ALUout;

regfile regfile_inst(
  .clk(clk),
  .rst(rst),

  .register_write_addr(reg_write_addr),
  .register_write_data(register_write_data),
  .register_write_enable(reg_write_enable), // in
  
  .read_register_1(rs1_r_addr), // in
  .rs1_data(rs1_data),        // out
  // .r_ena1(rs1_r_ena),
  .read_register_2(rs2_r_addr), // in
  .rs2_data(rs2_data)        // out
  // .r_ena2(rs2_r_ena)
);

endmodule
