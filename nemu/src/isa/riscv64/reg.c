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

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  uint32_t i = 0;

  printf("PC = 0x%lx,", cpu.pc);
  for(i=0; i<sizeof(regs)/sizeof(regs[0]); i++)
  {
      if (i % 4  == 0)
         printf("\r\n");
      printf("%5s = 0x%016lx,", regs[i], cpu.gpr[i]);      
  }
  printf("\r\n");
}

word_t isa_reg_str2val(const char *s, bool *success) {
  uint32_t i = 0;
  bool is_find = false;

  printf("s : %s\r\n", s);
  for(i=0; i<sizeof(regs)/sizeof(regs[0]); i++)
  {
      if (strcmp(regs[i], s) == 0)
      {
          is_find = true;
          break;
      }
  }

  if (is_find == true)
  {
     printf("0x%lx \r\n", cpu.gpr[i]);
     *success = true;
  }
  else if (strcmp("pc", s) == 0)
  {
      *success = true;
  }
  else 
  {
    printf("This register was not found!\r\n");
    *success = false;
  }

  return 0;
}

const char* isa_reg_name_display(uint32_t no) 
{
    assert(no < sizeof(regs)/sizeof(regs[0]));
    return regs[no];
}

void csrs_set(uint32_t idx, word_t value)
{
  switch(idx)
  {
    case CSR_MSTATUS: cpu.csr_mstatus = value; break;
    case CSR_MEPC: cpu.csr_mepc = value; break;
    case CSR_MCAUSE: cpu.csr_mcause = value;  break;
    case CSR_MTVEC: cpu.csr_mtvec = value;  break;
    default: assert(0); break;
  }
}

word_t csrs_get(uint32_t idx)
{
  word_t s_csr ;
  switch(idx)
  {
    case CSR_MSTATUS: s_csr = cpu.csr_mstatus; break;
    case CSR_MEPC: s_csr = cpu.csr_mepc ; break;
    case CSR_MCAUSE: s_csr = cpu.csr_mcause;  break;
    case CSR_MTVEC:  s_csr = cpu.csr_mtvec;  break;
    default: assert(0); break;
  }

  return s_csr;
}