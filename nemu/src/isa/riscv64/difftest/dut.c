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
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  uint32_t i;
  bool is_the_same = true;
  for(i=0; i<(sizeof(cpu.gpr)/sizeof(cpu.gpr[0])); i++)
  {
      if (ref_r->gpr[i] != cpu.gpr[i])
      {
          printf("difftest: ref pc : 0x%8lx, reg number = %d, ref is : 0x%8lx, DUT is 0x%8lx \r\n" ,pc, i, ref_r->gpr[i], cpu.gpr[i]);
          is_the_same = false;
      }

  }
  return is_the_same;
}

void isa_difftest_attach() {
}
