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

#ifndef __ISA_RISCV64_H__
#define __ISA_RISCV64_H__

#include <common.h>

#define CSR_MSTATUS 0x300
#define CSR_MEPC    0x341
#define CSR_MCAUSE  0x342

#define CSR_MTVEC   0x305

typedef struct {
  word_t gpr[32];
  vaddr_t pc;
  word_t csr_mepc;    //存放触发异常的PC
  word_t csr_mstatus; //存放处理器的状态
  word_t csr_mcause;  //存放触发异常的原因
  word_t csr_mtvec;   //异常时，处理器跳转的地址
  //word_t csrs[4096];
} riscv64_CPU_state;

// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv64_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
