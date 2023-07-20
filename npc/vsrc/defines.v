`timescale 1ns / 1ps

`define ZERO_WORD   64'h00000000_00000000   
`define RESET_PC    64'h00000000_00000000


`define REG_BUS    63 : 0

`define REG_WRITE_ENABLE        1'b1
`define REG_WRITE_DISABLE       1'b0

`define ALUB_SRC_REG         1'b0
`define ALUB_SRC_IMM         1'b1

// define ALU
// source : https://nju-projectn.github.io/dlco-lecture-note/exp/10.html#alu
`define ALU_CTRL_ADD  4'b0000    // 选择加法器输出，做加法 : auipc, jal, jalr, B_type, S_type, Load_type
`define ALU_CTRL_SUB  4'b1000    // 选择加法器输出，做减法 : sub
`define ALU_CTRL_SLL  4'b0001    // 选择移位器输出，左移 : sll , slli 
`define ALU_CTRL_SLT  4'b0010    // 做减法，选择带符号小于置位结果输出, Less按带符号结果设置 : slt, slti
`define ALU_CTRL_ 4'b1_010          // 做减法，选择无符号小于置位结果输出, Less按无符号结果设置 :
`define ALU_CTRL_SLTU 4'b0011    // 选择ALU输入B的结果直接输出 : sltu, sltiu
`define ALU_CTRL_XOR  4'b0100    // 选择异或输出 , xor , xori
`define ALU_CTRL_SRL  4'b0101    // 选择移位器输出，逻辑右移 : srl , srli
`define ALU_CTRL_SRA  4'b1101    // 选择移位器输出，算术右移 : sra , srai
`define ALU_CTRL_OR   4'b0110    // 选择逻辑或输出 , or , ori
`define ALU_CTRL_AND  4'b0111    // 选择逻辑与输出 , and, andi

// reg
`define REGISTER_X0     5'b00000








// `define INST_ADD   8'h11

// ///全局
// `define RstEnable 1'b1
// `define RstDisable 1'b0
// `define ZeroWord   64'd0

// `define WRITE_ENABLE    1'b1
// `define WriteDisable  1'b0
// `define READ_ENABLE   1'b1
// `define READ_DISABLE  1'b0
// `define AluOpBus      6:0
// `define AluSelBus     2:0
// `define InstValid     1'b1
// `define InstInvalid   1'b0
// `define Stop          1'b1
// `define NoStop        1'b0
// `define InDelaySlot 1'b1
// `define NotInDelaySlot 1'b0
// `define Branch 1'b1
// `define NotBranch 1'b0
// `define InterruptAssert 1'b1
// `define InterruptNotAssert 1'b0
// `define TrapAssert 1'b1
// `define TrapNotAssert 1'b0
// `define True_v 1'b1
// `define False_v 1'b0
// `define ChipEnable 1'b1
// `define ChipDisable 1'b0



// //指令集相关
 
// // I type inst   8条
// `define INST_TYPE_I 7'b0010011
// //32位相加
// `define INST_ADDI   3'b000
// `define INST_SLTI   3'b010
// `define INST_SLTIU  3'b011
// `define INST_XORI   3'b100
// `define INST_ORI    3'b110
// `define INST_ANDI   3'b111
//   //64BIT 最fun7--->fun6
// `define INST_SLLI   3'b001
// //包含算术和逻辑
// `define INST_SRI    3'b101


// // U type inst  7条
// `define INST_TYPE_L 7'b0000011
// `define INST_LB     3'b000
// `define INST_LH     3'b001
// `define INST_LW     3'b010
// `define INST_LBU    3'b100
// `define INST_LHU    3'b101
// //64bit inst
// `define INST_LWU    3'b110
// `define INST_LD     3'b011


// // S type inst   4条  
// `define INST_TYPE_S   7'b0100011
// `define INST_SB     3'b000
// `define INST_SH     3'b001
// `define INST_SW     3'b010
// //64 bit
// `define INST_SD     3'b011  

// // R and M type inst  16条
// `define INST_TYPE_R_M 7'b0110011
// // R type inst
// `define INST_ADD_SUB 3'b000
// `define INST_SLL    3'b001
// `define INST_SLT    3'b010
// `define INST_SLTU   3'b011
// `define INST_XOR    3'b100
// `define INST_SR     3'b101
// `define INST_OR     3'b110
// `define INST_AND    3'b111
// // M type inst  乘除法
// `define INST_MUL    3'b000
// `define INST_MULH   3'b001
// `define INST_MULHSU 3'b010
// `define INST_MULHU  3'b011
// `define INST_DIV    3'b100
// `define INST_DIVU   3'b101
// `define INST_REM    3'b110
// `define INST_REMU   3'b111

// // J type inst  4条
// `define INST_JAL    7'b1101111
// `define INST_JALR   7'b1100111
// `define INST_LUI    7'b0110111
// `define INST_AUIPC  7'b0010111

// `define INST_NOP    32'h00000001
// `define INST_NOP_OP 7'b0000001
// `define INST_MRET   32'h30200073
// //从子过程返回
// `define INST_RET    32'h00008067

// `define INST_FENCE  7'b0001111
// `define INST_ECALL  32'h73


//停止 EBREAK指令被调试器所使用，用来将控制权传送回给调试环境
`define INST_EBREAK 32'h00100073

// // B type inst 有条件跳转  6条
// `define INST_TYPE_B 7'b1100011
// `define INST_BEQ    3'b000
// `define INST_BNE    3'b001
// `define INST_BLT    3'b100
// `define INST_BGE    3'b101
// `define INST_BLTU   3'b110
// `define INST_BGEU   3'b111

// // CSR inst   7条
// `define INST_CSR    7'b1110011
// `define INST_CSRRW  3'b001
// `define INST_CSRRS  3'b010
// `define INST_CSRRC  3'b011
// `define INST_CSRRWI 3'b101
// `define INST_CSRRSI 3'b110
// `define INST_CSRRCI 3'b111
