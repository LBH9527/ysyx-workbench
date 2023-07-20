// reference : https://nju-projectn.github.io/dlco-lecture-note/exp/11.html
// instruction decode
// control unit

`include "defines.v"

module idu_ctrl(

//   input wire [31 : 0]inst,   
    input wire [6  : 0]opcode,
    input wire [2  : 0]funct3,
    input wire [6  : 0]funct7,

    output reg alu_b_sel,                // ALU B 的数据来源的数据选择器控制信号, 0-选择寄存器，1-选择立即数；

    output reg [1 : 0] alu_op,             // ALUop is control-unit set and used to describe the instruction class

    // id_stage -> regfile
    output reg reg_write_enable                 //reg write enable
);

wire    is_lui;
wire    is_auipc;
wire    is_jal;
wire    is_jalr;
wire    is_beq;
wire    is_bne;
wire    is_blt;
wire    is_bge;
wire    is_bltu;
wire    is_bgeu;
wire    is_lb;
wire    is_lh;
wire    is_lw;
wire    is_lbu;
wire    is_lhu;
wire    is_sb;
wire    is_sh;
wire    is_sw;
wire    is_addi;
wire    is_slti;
wire    is_sltiu;
wire    is_xori;
wire    is_ori;
wire    is_andi;
wire    is_slli;
wire    is_srli;
wire    is_srai;
wire    is_add;
wire    is_sub;
wire    is_sll;
wire    is_slt;
wire    is_sltu;
wire    is_xor;
wire    is_srl;
wire    is_sra;
wire    is_or;
wire    is_and;

wire    is_add_type;
wire    is_u_type;
wire    is_jump_type;
wire    is_b_type;
wire    is_r_type;
wire    is_i_type;
wire    is_s_type;

// wire [6  : 0]opcode;
// // wire [4  : 0]rd;
// wire [2  : 0]funct3;
// wire [6  : 0]funct7;
// // wire [4  : 0]rs1;
// // wire [4  : 0]rs2;

// assign opcode = inst[6  :  0];
// // assign rd     = inst[11 :  7];
// assign funct3 = inst[14 : 12];
// // assign rs1    = inst[19 : 15];
// // assign rs2    = inst[24 : 20];
// assign funct7 = inst[31 : 25];


assign  is_lui  = (opcode == 7'h37) ;
assign  is_auipc= (opcode == 7'h17) ;
assign  is_jal  = (opcode == 7'h6F) ;
assign  is_jalr = (opcode == 7'h67) && (funct3 ==3'h0) ;
assign  is_beq  = (opcode == 7'h63) && (funct3 ==3'h0) ;
assign  is_bne  = (opcode == 7'h63) && (funct3 ==3'h1) ;
assign  is_blt  = (opcode == 7'h63) && (funct3 ==3'h4) ;
assign  is_bge  = (opcode == 7'h63) && (funct3 ==3'h5) ;
assign  is_bltu = (opcode == 7'h63) && (funct3 ==3'h6) ;
assign  is_bgeu = (opcode == 7'h63) && (funct3 ==3'h7) ;
assign  is_lb   = (opcode == 7'h03) && (funct3 ==3'h0) ;
assign  is_lh   = (opcode == 7'h03) && (funct3 ==3'h1) ;
assign  is_lw   = (opcode == 7'h03) && (funct3 ==3'h2) ;
assign  is_lbu  = (opcode == 7'h03) && (funct3 ==3'h4) ;
assign  is_lhu  = (opcode == 7'h03) && (funct3 ==3'h5) ;
assign  is_sb   = (opcode == 7'h23) && (funct3 ==3'h0) ;
assign  is_sh   = (opcode == 7'h23) && (funct3 ==3'h1) ;
assign  is_sw   = (opcode == 7'h23) && (funct3 ==3'h2) ;
assign  is_addi = (opcode == 7'h13) && (funct3 ==3'h0) ;
assign  is_slti = (opcode == 7'h13) && (funct3 ==3'h2) ;
assign  is_sltiu= (opcode == 7'h13) && (funct3 ==3'h3) ;
assign  is_xori = (opcode == 7'h13) && (funct3 ==3'h4) ;
assign  is_ori  = (opcode == 7'h13) && (funct3 ==3'h6) ;
assign  is_andi = (opcode == 7'h13) && (funct3 ==3'h7) ;
assign  is_slli = (opcode == 7'h13) && (funct3 ==3'h1) && (funct7 == 7'h00);
assign  is_srli = (opcode == 7'h13) && (funct3 ==3'h5) && (funct7 == 7'h00);
assign  is_srai = (opcode == 7'h13) && (funct3 ==3'h5) && (funct7 == 7'h20);
assign  is_add  = (opcode == 7'h33) && (funct3 ==3'h0) && (funct7 == 7'h00);
assign  is_sub  = (opcode == 7'h33) && (funct3 ==3'h0) && (funct7 == 7'h20);
assign  is_sll  = (opcode == 7'h33) && (funct3 ==3'h1) && (funct7 == 7'h00);
assign  is_slt  = (opcode == 7'h33) && (funct3 ==3'h2) && (funct7 == 7'h00);
assign  is_sltu = (opcode == 7'h33) && (funct3 ==3'h3) && (funct7 == 7'h00);
assign  is_xor  = (opcode == 7'h33) && (funct3 ==3'h4) && (funct7 == 7'h00);
assign  is_srl  = (opcode == 7'h33) && (funct3 ==3'h5) && (funct7 == 7'h00);
assign  is_sra  = (opcode == 7'h33) && (funct3 ==3'h5) && (funct7 == 7'h20);
assign  is_or   = (opcode == 7'h33) && (funct3 ==3'h6) && (funct7 == 7'h00);
assign  is_and  = (opcode == 7'h33) && (funct3 ==3'h7) && (funct7 == 7'h00);

// 判断指令类型 R 、I、S、B、U、J
assign  is_u_type   = is_lui | is_auipc ;
assign  is_jump_type= is_jal ;
assign  is_b_type   = is_beq | is_bne | is_blt | is_bge | is_bltu | is_bgeu ;
assign  is_r_type   = is_add | is_sub | is_sll | is_slt | is_sltu | is_xor 
                    | is_srl | is_sra | is_or | is_and ;
assign  is_i_type   = is_jalr | is_lb | is_lh | is_lw | is_lbu | is_lhu 
                    | is_addi | is_slti | is_sltiu | is_xori | is_ori | is_andi
                    | is_slli | is_srli | is_srai ;
assign  is_s_type   = is_sb | is_sh | is_sw ;


localparam INSTRUCTION_TYPE_R = 6'b000001;
localparam INSTRUCTION_TYPE_I = 6'b000010;   
localparam INSTRUCTION_TYPE_S = 6'b000100;
localparam INSTRUCTION_TYPE_B = 6'b001000;  
localparam INSTRUCTION_TYPE_U = 6'b010000;
localparam INSTRUCTION_TYPE_J = 6'b100000;   

wire [5 : 0] instr_type;


assign instr_type = {is_jump_type, is_u_type, is_b_type, is_s_type, is_i_type, is_r_type};

always @(*) 
begin
    case (instr_type)
        INSTRUCTION_TYPE_R : 
        begin
            alu_b_sel = `ALUB_SRC_REG;
            reg_write_enable = `REG_WRITE_ENABLE;
            alu_op[1:0] = 2'b10 ;
        
        end

        INSTRUCTION_TYPE_I : 
        begin
            alu_b_sel = `ALUB_SRC_IMM ;
            reg_write_enable = `REG_WRITE_ENABLE;
            alu_op[1:0] = 2'b10 ;
        end
        // INSTRUCTION_TYPE_S :
        // begin
        // end
        // INSTRUCTION_TYPE_B :
        // begin
        // end
        // INSTRUCTION_TYPE_U :
        // begin
        // end
        // INSTRUCTION_TYPE_J :
        // begin
        // end
        default :
        begin
            alu_b_sel = `ALUB_SRC_REG;
            reg_write_enable = `REG_WRITE_DISABLE ;
            alu_op[1:0] = 2'b11 ;
        end
    endcase 
end




endmodule
