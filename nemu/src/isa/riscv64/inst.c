/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include "trace.h"

#undef DBG_TAG
#undef DBG_LVL
#define DBG_TAG          "inst"
// #define DBG_LVL           DBG_LOG
#define DBG_LVL           DBG_INFO
// #define DBG_LVL           DBG_ERROR

#include <debug_log.h> 
#define R(i) gpr(i)
#define CSRs(i) csrs(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_I, TYPE_U, TYPE_S,TYPE_J,TYPE_R, TYPE_B,
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | (BITS(i, 30, 21) << 1) | (BITS(i, 20, 20) << 11) | (BITS(i, 19, 12) << 12); } while(0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | (BITS(i, 30, 25) << 5) | (BITS(i, 11, 8) << 1)   | (BITS(i, 7, 7) << 11); } while(0)

static void decode_operand(Decode *s, int *dest, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rd  = BITS(i, 11, 7);
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *dest = rd;
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_B: src1R(); src2R(); immB(); break;
  }
}

static int decode_exec(Decode *s) {
  int dest = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &dest, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = s->pc + imm);
  INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(dest) = Mr(src1 + imm, 8));
  INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));

  //addi 加立即数(Add Immediate). I-type, RV32I and RV64I. ; x[rd] = x[rs1] + sext(immediate)
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, LOG_D("addi src1 : 0x%lx, imm  0x%lx:", src1, imm);  \
                                                                R(dest) = src1 + imm ;  \
                                                                LOG_D("dest : 0x%lx", R(dest)) );  
  // addiw : x[rd] = sext((x[rs1] + sext(immediate))[31:0])
  INSTPAT("??????? ????? ????? 000 ????? 00110 11", addiw ,  I, R(dest) = SEXT( (src1 + imm), 32 ) );         

  // andi 与立即数 (And Immediate). I-type,  x[rd] = x[rs1] & sext(immediate
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(dest) = src1 & imm );  
  // xori rd, rs1, immediate x[rd] = x[rs1] ^ sext(immediate)
  // 立即数异或(Exclusive-OR Immediate). I-type, RV32I and RV64I.
  // x[rs1]和有符号扩展的 immediate 按位异或，结果写入 x[rd]
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(dest) = src1 ^ imm );  



  // seqz : sltiu rd, rs1, 1
  // sltiu : Set if Less Than Immediate, Unsigned , I-type, //x[rd] = (x[rs1] <u sext(immediate))
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu , I, R(dest) = (src1 < (word_t)imm) ? 1 : 0 ) ;

  // lw rd, offset(rs1) x[rd] = sext(M[x[rs1] + sext(offset)][31:0])
  // 字加载 (Load Word). I-type, RV32I and RV64I.
  // 从地址 x[rs1] + sign-extend(offset)读取四个字节，写入 x[rd]。对于 RV64I，结果要进行符号
  // 位扩展。
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(dest) = SEXT( Mr(src1 + imm, 4), 32) );
  // lwu rd, offset(rs1) x[rd] = M[x[rs1] + sext(offset)][31:0]
  // 无符号字加载 (Load Word, Unsigned). I-type, RV64I.
  // 从地址 x[rs1] + sign-extend(offset)读取四个字节，零扩展后写入 x[rd]
  INSTPAT("??????? ????? ????? 110 ????? 00000 11", lwu  , I, R(dest) =  (Mr(src1 + imm, 4) & BITMASK(32)) );  

  // lb rd, offset(rs1) x[rd] = sext(M[x[rs1] + sext(offset)][7:0])
  // 字节加载 (Load Byte). I-type, RV32I and RV64I.
  // 从地址 x[rs1] + sign-extend(offset)读取一个字节，经符号位扩展后写入 x[rd]。
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb   , I, R(dest) = SEXT(Mr(src1 + imm, 1), 8) );
  // lbu 无符号字节加载 (Load Byte, Unsigned). I-type ;x[rd] = M[x[rs1] + sext(offset)][7:0]
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu   , I, R(dest) = (Mr(src1 + imm, 1) & BITMASK(8)) );

  // lh rd, offset(rs1) x[rd] = sext(M[x[rs1] + sext(offset)][15:0])
  // 半字加载 (Load Halfword). I-type, RV32I and RV64I.
  // 从地址 x[rs1] + sign-extend(offset)读取两个字节，经符号位扩展后写入 x[rd]。
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh   , I, R(dest) = SEXT(Mr(src1 + imm, 2),16 ) );
  // lhu rd, offset(rs1) x[rd] = M[x[rs1] + sext(offset)][15:0]
  // 无符号半字加载 (Load Halfword, Unsigned). I-type, RV32I and RV64I.
  // 从地址 x[rs1] + sign-extend(offset)读取两个字节，经零扩展后写入 x[rd]。
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu  , I, R(dest) = Mr(src1 + imm, 2) & BITMASK(16));

  //jal 跳转并链接 (Jump and Link). J-type, x[rd] = pc+4; pc += sext(offset)
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(dest) = s->pc + 4 ; s->dnpc = s->pc + imm;   \
                                                                  IFDEF(CONFIG_FTRACE, if(dest == 1) function_trace(s->pc, s->dnpc, true) ) ); //rd ← pc + 4 ; pc ← pc + imm20
  //jalr 跳转并寄存器链接 (Jump and Link Register). I-type, t =pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(dest) = s->pc + 4 ; s->dnpc = ( (src1 +  imm ) & ~1) ; \
                                                                IFDEF(CONFIG_FTRACE, if(s->isa.inst.val == 0x8067) function_trace(s->pc, s->dnpc, false);
                                                                else function_trace(s->pc, s->dnpc, true) )); 
  // slli rd, rs1, shamt x[rd] = x[rs1] ≪ shamt
  // 立即数逻辑左移(Shift Left Logical Immediate). I-type, RV32I and RV64I.
  // 把寄存器 x[rs1]左移 shamt位，空出的位置填入 0，结果写入 x[rd]。对于 RV32I，仅当 shamt[5]=0 .时，指令才是有效的
  INSTPAT("000000 ?????? ????? 001 ????? 00100 11", slli  , I,  LOG_D("[slli] src1 : 0x%lx, imm 0x%lx:", src1,imm);  \
                                                                R(dest) = src1 << imm ;\
                                                                LOG_D("dest : 0x%lx", R(dest))  );    
  //srli rd, rs1, shamt    x[rd] = (x[rs1] ≫� shamt)
  // 立即数逻辑右移(Shift Right Logical Immediate). I-type, RV32I and RV64I.
  // 把寄存器 x[rs1]右移 shamt位，空出的位置填入 0，结果写入 x[rd]。对于 RV32I，仅当 shamt[5]=0
  // 时，指令才是有效的。
  INSTPAT("000000 ?????? ????? 101 ????? 00100 11", srli  , I, R(dest) = (src1 >> imm) & BITMASK(64-imm)); 
  //srai  立即数算术右移(Shift Right Arithmetic Immediate). I-type, x[rd] = (x[rs1] ≫� shamt)
  INSTPAT("010000 ?????? ????? 101 ????? 00100 11", srai  , I, R(dest) = (sword_t)src1 >> imm );  
  // srliw rd, rs1, shamt x[rd] = sext(x[rs1][31: 0] ≫� shamt)
  // 立即数逻辑右移字(Shift Right Logical Word Immediate). I-type, RV64I only.
  // 把寄存器 x[rs1]右移 shamt 位，空出的位置填入 0，结果截为 32 位，进行有符号扩展后写入
  // x[rd]。仅当 shamt[5]=0 时，指令才是有效的
  INSTPAT("0000000 ????? ????? 101 ????? 00110 11", srliw  , I, LOG_D("src1 is 0x%lx", src1); R(dest) = SEXT(((word_t)BITS(src1, 31, 0)) >> imm, 32); \
                                                                assert(BITS(imm, 5, 5) == 0)  ); 
  // slliw 立即数逻辑左移字(Shift Left Logical Word Immediate). I-type, x[rd] = sext((x[rs1] ≪ shamt)[31: 0])
  INSTPAT("0000000 ????? ????? 001 ????? 00110 11", slliw  , I, R(dest) = SEXT((BITS(src1, 31, 0) << imm), 32); \
                                                                assert(BITS(imm, 5, 5) == 0) ); 
  // sraiw 立即数算术右移字(Shift Right Arithmetic Word Immediate). I-type, x[rd] = sext(x[rs1][31: 0] ≫� shamt)
  INSTPAT("0100000 ????? ????? 101 ????? 00110 11", sraiw , I, R(dest) = SEXT( (int32_t)BITS( src1, 31, 0) >> imm, 32); \
                                                                assert(BITS(imm, 5, 5) == 0) ); 

  // sw rs2, offset(rs1) M[x[rs1] + sext(offset) = x[rs2][31: 0]
  // 存字(Store Word). S-type, RV32I and RV64I.
  // 将 x[rs2]的低位 4 个字节存入内存地址 x[rs1]+sign-extend(offset)
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, LOG_D("[sw] src1 : 0x%lx, src2 0x%lx:, imm 0x%lx:", src1, src2, imm);  \
                                                                Mw(src1 + imm, 4, src2));
  // sb rs2, offset(rs1) M[x[rs1] + sext(offset) = x[rs2][7: 0]
  // 存字节(Store Byte). S-type, RV32I and RV64I.
  // 将 x[rs2]的低位字节存入内存地址 x[rs1]+sign-extend(offset)。
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, LOG_D("[sb] src1 : 0x%lx, src2 0x%lx:, imm 0x%lx:", src1, src2, imm);  \
                                                                Mw(src1 + imm, 1, src2));
  //sh 存半字(Store Halfword). S-type ; M[x[rs1] + sext(offset) = x[rs2][15: 0]
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));


  // addw : x[rd] = sext((x[rs1] + x[rs2])[31:0])
  INSTPAT("0000000 ????? ????? 000 ????? 01110 11", addw   , R, R(dest) = SEXT( (src1 + src2), 32) );
  // or 取或(OR). R-type,  x[rd] = x[rs1] |  x[rs2]
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(dest) = src1 | src2 );
  //   ori rd, rs1, immediate x[rd] = x[rs1] | sext(immediate)
  // 立即数取或(OR Immediate). I-type, RV32I and RV64I.
  // 把寄存器 x[rs1]和有符号扩展的立即数 immediate 按位取或，结果写入 x[rd]。
  // 压缩形式： c.or rd, rs2
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori     , I, R(dest) = src1 | imm );

  // add  加 (Add). R-type, x[rd] = x[rs1] + x[rs2]
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(dest) = src1 + src2 );
  //sllw 逻辑左移字(Shift Left Logical Word). R-type, RV64I only. x[rd] = sext ( (x[rs1] ≪ x[rs2][4: 0]) [31: 0])
  INSTPAT("0000000 ????? ????? 001 ????? 01110 11", sllw   , R, R(dest) = SEXT(BITS(src1 << BITS (src2, 4, 0), 31, 0), 32) );
  // and 与 (And). R-type, RV32I and RV64I  //x[rd] = x[rs1] & x[rs2]
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(dest) = src1 & src2 );  

  // sub : Substract. R-type ; x[rd] = x[rs1] − x[rs2]
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub   , R, R(dest) = src1 - src2 );
  // subw rd, rs1, rs2 x[rd] = sext((x[rs1] − x[rs2])[31: 0])
  // 减去字(Substract Word). R-type, RV64I only.
  // x[rs1]减去 x[rs2]，结果截为 32 位，有符号扩展后写入 x[rd]。忽略算术溢出
  INSTPAT("0100000 ????? ????? 000 ????? 01110 11", subw   , R, R(dest) = SEXT(BITS(src1 - src2, 31, 0), 32) );  
  // rem rd, rs1, rs2 x[rd] = x[rs1] %� x[rs2]
  // 求余数(Remainder). R-type, RV32M and RV64M.
  // x[rs1]除以 x[rs2]，向 0 舍入，都视为 2 的补码，余数写入 x[rd]
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem   , R, R(dest) = src1 % src2 );  
  // mulw rd, rs1, rs2 x[rd] = sext((x[rs1] × x[rs2])[31: 0])
  // 乘字(Multiply Word). R-type, RV64M only.
  // 把寄存器 x[rs2]乘到寄存器 x[rs1]上，乘积截为 32 位，进行有符号扩展后写入 x[rd]。忽略算术溢出
  INSTPAT("0000001 ????? ????? 000 ????? 01110 11", mulw   , R, R(dest) = SEXT(BITS(src1 * src2, 31, 0), 32) );  
  // mul rd, rs1, rs2 x[rd] = x[rs1] × x[rs2]
  // 乘(Multiply). R-type, RV32M and RV64M.
  // 把寄存器 x[rs2]乘到寄存器 x[rs1]上，乘积写入 x[rd]。忽略算术溢出。
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul   , R, R(dest) = src1 * src2 );  
  // divw rd, rs1, rs2 x[rd] = sext(x[rs1][31:0] ÷s x[rs2][31:0])
  // 字除法(Divide Word). R-type, RV64M.
  // 用寄存器 x[rs1]的低 32 位除以寄存器 x[rs2]的低 32 位，向零舍入，将这些数视为二进制补
  // 码，把经符号位扩展的 32 位商写入 x[rd]。
  INSTPAT("0000001 ????? ????? 100 ????? 01110 11", divw  , R, LOG_D("[divw] src1 : 0x%lx, src2  0x%lx:", src1, src2);  \
                                                                R(dest) = SEXT( ((int32_t) BITS(src1, 31, 0)) / ((int32_t)BITS(src2, 31, 0)) ,32); \
                                                                LOG_D("dest : 0x%lx", R(dest))  );    
 
  //   divuw rd, rs1, rs2 x[rd] = sext(x[rs1][31:0] ÷u x[rs2][31:0])
  // 无符号字除法(Divide Word, Unsigned). R-type, RV64M.
  // 用寄存器 x[rs1]的低 32 位除以寄存器 x[rs2]的低 32 位，向零舍入，将这些数视为无符号数，
  // 把经符号位扩展的 32 位商写入 x[rd]
  INSTPAT("0000001 ????? ????? 101 ????? 01110 11", divuw   , R, R(dest) = SEXT( (word_t)( BITS(src1, 31, 0) / BITS(src2, 31, 0) ) ,32); );  


  //sltu  无符号小于则置位(Set if Less Than, Unsigned). R-type, RV32I and RV64I. x[rd] = (x[rs1] <� x[rs2])
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu    , R, R(dest) = ((word_t)src1 < (word_t)src2)? 1: 0; );  

  // slt rd, rs1, rs2 x[rd] = (x[rs1] <� x[rs2])
  // 小于则置位(Set if Less Than). R-type, RV32I and RV64I.
  // 比较 x[rs1]和 x[rs2]中的数，如果 x[rs1]更小，向 x[rd]写入 1，否则写入 0。
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(dest) = ((sword_t)src1 < (sword_t)src2)? 1: 0; );  
  // sraw rd, rs1, rs2 x[rd] = sext(x[rs1][31: 0] ≫� x[rs2][4: 0])
  // 算术右移字(Shift Right Arithmetic Word). R-type, RV64I only.
  // 把寄存器 x[rs1]的低 32 位右移 x[rs2]位，空位用 x[rs1][31]填充，结果进行有符号扩展后写
  // 入 x[rd]。 x[rs2]的低 5 位为移动位数，高位则被忽略
  INSTPAT("0100000 ????? ????? 101 ????? 01110 11", sraw  , R, R(dest) = SEXT( (int32_t)BITS(src1, 31, 0) >>  BITS(src2, 4, 0), 32)  ); 
  // srlw rd, rs1, rs2 x[rd] = sext(x[rs1][31: 0] ≫� x[rs2][4: 0])
  // 逻辑右移字(Shift Right Logical Word). R-type, RV64I only.
  // 把寄存器 x[rs1]的低 32 位右移 x[rs2]位，空出的位置填入 0，结果进行有符号扩展后写入
  // x[rd]。 x[rs2]的低 5 位代表移动位数，其高位则被忽略。
  INSTPAT("0000000 ????? ????? 101 ????? 01110 11", srlw  , R, R(dest) = SEXT( (uint32_t)BITS(src1, 31, 0) >>  BITS(src2, 4, 0), 32)  ); 
  // remu rd, rs1, rs2 x[rd] = x[rs1] %� x[rs2]
  // 求无符号数的余数(Remainder, Unsigned). R-type, RV32M and RV64M.
  // x[rs1]除以 x[rs2]，向 0 舍入，都视为无符号数，余数写入 x[rd]。
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu  , R, R(dest) = (word_t)src1 % (word_t)src2 ); 
  // remuw rd, rs1, rs2 x[rd] = sext(x[rs1][31: 0] %� x[rs2][31: 0])
  // 求无符号数的余数字(Remainder Word, Unsigned). R-type, RV64M only.
  // x[rs1]的低 32 位除以 x[rs2]的低 32 位，向 0 舍入，都视为无符号数，将余数的有符号扩展
  // 写入 x[rd]。
  INSTPAT("0000001 ????? ????? 111 ????? 01110 11", remuw  , R, R(dest) = SEXT( ((uint32_t)BITS(src1, 31, 0) % (uint32_t)BITS(src2, 31, 0)), 32) ); 
  // remw rd, rs1, rs2 x[rd] = sext(x[rs1][31: 0] %� x[rs2][31: 0])
  // 求余数字(Remainder Word). R-type, RV64M only.
  // x[rs1]的低 32 位除以 x[rs2]的低 32 位，向 0 舍入，都视为 2 的补码，将余数的有符号扩展
  // 写入 x[rd]。
  INSTPAT("0000001 ????? ????? 110 ????? 01110 11", remw  , R,  LOG_D("[remw] src1 : 0x%lx, src2  0x%lx:", src1, src2); \
                                                                LOG_D("[remw] src1 : %i", (int32_t)BITS(src1, 31, 0) ); \
                                                                LOG_D("[remw] src1 %% src2 : %i" , ( ((int32_t)BITS(src1, 31, 0)) % ((int32_t)BITS(src2, 31, 0)) ) ); \
                                                                R(dest) = SEXT( ( ((int32_t)BITS(src1, 31, 0)) % ((int32_t)BITS(src2, 31, 0)) ), 32) );  

  // divu rd, rs1, rs2 x[rd] = x[rs1] ÷u x[rs2]
  // 无符号除法(Divide, Unsigned). R-type, RV32M and RV64M.
  // 用寄存器 x[rs1]的值除以寄存器 x[rs2]的值，向零舍入，将这些数视为无符号数，把商写入
  // x[rd]。
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu  , R, R(dest) = (word_t)src1 / (word_t)src2 ); 

  // div rd, rs1, rs2 x[rd] = x[rs1] ÷s x[rs2]
  // 除法(Divide). R-type, RV32M and RV64M.
  // 用寄存器 x[rs1]的值除以寄存器 x[rs2]的值，向零舍入，将这些数视为二进制补码，把商写
  // 入 x[rd]
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div  , R, R(dest) = (sword_t)src1 / (sword_t)src2 ); 


  // xor rd, rs1, rs2 x[rd] = x[rs1] ^ x[rs2]
  // 异或(Exclusive-OR). R-type, RV32I and RV64I.
  // x[rs1]和 x[rs2]按位异或，结果写入 x[rd]。
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor  , R, R(dest) = src1 ^ src2 ); 
  // sll rd, rs1, rs2 x[rd] = x[rs1] ≪ x[rs2]
  // 逻辑左移(Shift Left Logical). R-type, RV32I and RV64I.
  // 把寄存器 x[rs1]左移 x[rs2]位，空出的位置填入 0，结果写入 x[rd]。 x[rs2]的低 5 位（如果是
  // RV64I 则是低 6 位）代表移动位数，其高位则被忽略。
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll  , R, R(dest) = src1 << BITS(src2, 6, 0) ); 

  // beqz : Branch if Equal to Zero = beq rs1, x0, offset.
  // beq :Branch if Equal. B-type ; if (rs1 == rs2) pc += sext(offset)
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, if(src1 == src2) s->dnpc = s->pc + imm );
  // bne : Branch if Not Equal) : B : if (rs1 ≠ rs2) pc += sext(offset)
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, LOG_D("[bne]src1 : 0x%lx, src2 : 0x%lx, imm : 0x%lx,", src1, src2, imm); \
                                                                if(src1 != src2) s->dnpc = s->pc +  + imm );
  // beg ： 大于等于时分支 (Branch if Greater Than or Equal). B-type, if (rs1 ≥s rs2) pc += sext(offset)
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", beg    , B, if((sword_t)src1 >= (sword_t)src2) s->dnpc = s->pc + imm );
  // bltu rs1, rs2, offset if (rs1 <u rs2) pc += sext(offset)
  // 无符号小于时分支 (Branch if Less Than, Unsigned). B-type, RV32I and RV64I.
  // 若寄存器 x[rs1]的值小于寄存器 x[rs2]的值（均视为无符号数），把 pc 的值设为当前值加上
  // 符号位扩展的偏移 offset
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, if((word_t)src1 < (word_t)src2) s->dnpc = s->pc + imm );
  // blt rs1, rs2, offset if (rs1 <s rs2) pc += sext(offset)
  // 小于时分支 (Branch if Less Than). B-type, RV32I and RV64I.
  // 若寄存器 x[rs1]的值小于寄存器 x[rs2]的值（均视为二进制补码），把 pc 的值设为当前值加
  // 上符号位扩展的偏移 offset。 
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if((sword_t)src1 < (sword_t)src2) s->dnpc = s->pc + imm );
  // bgeu rs1, rs2, offset if (rs1 ≥u rs2) pc += sext(offset)
  // 无符号大于等于时分支 (Branch if Greater Than or Equal, Unsigned). B-type, RV32I and RV64I.
  // 若寄存器 x[rs1]的值大于等于寄存器 x[rs2]的值（均视为无符号数），把 pc 的值设为当前值
  // 加上符号位扩展的偏移 offset。
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu    , B, if((word_t)src1 >= (word_t)src2) s->dnpc = s->pc + imm );


  // lui : Load Upper Immediate, U-type, x[rd] = sext(immediate[31:12] << 12)q
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, LOG_D("lui : imm : 0x%lx", imm) ;R(dest) = imm);

  // csrrw rd, csr, zimm[4:0] t = CSRs[csr]; CSRs[csr] = x[rs1]; x[rd] = t
  // 读后写控制状态寄存器 (Control and Status Register Read and Write). I-type, RV32I and RV64I.
  // 记控制状态寄存器 csr 中的值为 t。 把寄存器 x[rs1]的值写入 csr，再把 t 写入 x[rd]。
  INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw  , I,  R(dest) = csrs_get(imm) ; csrs_set(imm ,src1); \
                                                                  LOG_D("csrrw : csr num = 0x%lx, src1 : 0x%lx", imm, src1) );

  // csrrs rd, csr, rs1 t = CSRs[csr]; CSRs[csr] = t | x[rs1]; x[rd] = t
  // 读后置位控制状态寄存器 (Control and Status Register Read and Set). I-type, RV32I and RV64I.
  // 记控制状态寄存器 csr 中的值为 t。 把 t 和寄存器 x[rs1]按位或的结果写入 csr，再把 t 写入
  // x[rd]。
  INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs  , I, R(dest) = csrs_get(imm) ; csrs_set(imm , src1 | R(dest)) ; \
                                                                 LOG_D("csrrs : csr num = 0x%lx, src1 : 0x%lx", imm, src1) );

  // //csrrc rd, csr, rs1 t = CSRs[csr]; CSRs[csr] = t &~x[rs1]; x[rd] = t
  // // 读后清除控制状态寄存器 (Control and Status Register Read and Clear). I-type, RV32I and
  // // RV64I
  // INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrc  , I, R(dest) = (sword_t)src1 >> imm );

  // // csrrwi rd, csr, zimm[4:0] x[rd] = CSRs[csr]; CSRs[csr] = zimm
  // // 立即数读后写控制状态寄存器 (Control and Status Register Read and Write Immediate). I-type,
  // // RV32I and RV64I
  // INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrwi  , I, R(dest) = (sword_t)src1 >> imm );

  
  // INSTPAT("??????? ????? ????? 010 ????? 11100 11", cssrrsi  , I, R(dest) = (sword_t)src1 >> imm );

  // // csrrci rd, csr, zimm[4:0] t = CSRs[csr]; CSRs[csr] = t | zimm; x[rd] = t
  // // 立即数读后设置控制状态寄存器 (Control and Status Register Read and Set Immediate). I-type,
  // // RV32I and RV64I.
  // INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrci  , I, R(dest) = (sword_t)src1 >> imm );

  // mret ExceptionReturn(Machine)
  // 机器模式异常返回(Machine-mode Exception Return). R-type, RV32I and RV64I 特权架构
  // 从机器模式异常处理程序返回。将 pc 设置为 CSRs[mepc], 将特权级设置成
  // CSRs[mstatus].MPP, CSRs[mstatus].MIE 置成 CSRs[mstatus].MPIE, 并且将
  // CSRs[mstatus].MPIE 为 1;并且，如果支持用户模式，则将 CSR [mstatus].MPP 设置为 0。
  INSTPAT("0011000 00010 00000 000 00000 11100 11", mret  , R, s->dnpc = csrs_get(CSR_MEPC) ; \
                                                                LOG_D("mret : mepc = 0x%lx", csrs_get(CSR_MEPC) ); );


  INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall , I, s->dnpc = isa_raise_intr(11, s->pc) );
                                                               

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
