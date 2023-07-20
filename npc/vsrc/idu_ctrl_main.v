`include "defines.v"

module idu_ctrl_main(
    input wire [31 : 0]inst,   
    input wire [`REG_BUS]rs1_data,
    input wire [`REG_BUS]rs2_data,

    output wire [`REG_BUS]alu_src_a,
    output wire [`REG_BUS]alu_src_b,
    output wire [3 : 0] alu_ctrl,
 
    // id_stage -> regfile
    output wire reg_write_enable,                  //reg write enable
    output wire [4 : 0]reg_write_addr
);

wire [6  : 0]opcode;
// wire [4  : 0]rd;
wire [2  : 0]funct3;
wire [6  : 0]funct7;
// wire [4  : 0]rs1;
// wire [4  : 0]rs2;

assign opcode = inst[6  :  0];
// assign rd     = inst[11 :  7];
assign funct3 = inst[14 : 12];
// assign rs1    = inst[19 : 15];
// assign rs2    = inst[24 : 20];
assign funct7 = inst[31 : 25];


wire alu_b_sel;
// wire [`REG_BUS] ALUSrcB;
wire [`REG_BUS] IMM ;
wire [1 : 0] alu_op ;


assign alu_src_a = rs1_data;
assign reg_write_addr = inst[11 :  7];

idu_ctrl idu_ctrl_inst(
    // .inst(inst),
    .opcode(opcode),
    .funct3(funct3),
    .funct7(funct7),
    .alu_b_sel(alu_b_sel),      // out
    .alu_op(alu_op),            // out

    .reg_write_enable(reg_write_enable)
);


idu_alu_ctrl idu_alu_ctrl_inst(
    .alu_op(alu_op),            // in
    .funct3(funct3),
    .funct7(funct7),
    .alu_ctrl_o(alu_ctrl)
);


idu_alu_mux idu_alu_mux_inst(
    .rs2_data(rs2_data),
    .imm(IMM),
    .alu_b_sel(alu_b_sel),      // in
    .alu_src_b_o(alu_src_b)
);

idu_imm idu_imm_inst(
    .inst(inst),
    .imm_o(IMM)
);

endmodule
