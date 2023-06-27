
//rvcpu-test.cpp
#include <verilated.h>     //Defines common routines     
#include <verilated_vcd_c.h>    
#include <iostream>
#include <fstream>
#include "Vrvcpu.h"   //design under test of top

#ifdef VM_TRACE                 // --trace
#include <verilated_vcd_c.h>
static VerilatedVcdC* tfp;       //to form *.vcd file
#endif


using namespace std;

static Vrvcpu* top;
static vluint64_t main_time = 0;
static const vluint64_t sim_time = 1000;

// inst.bin
// inst 0: 1 + zero = reg1 1+0=1
// inst 1: 2 + zero = reg1 2+0=2
// inst 2: 1 + reg1 = reg1 1+2=3
int inst_rom[65536];

void read_inst( char* filename)
{
  FILE *fp = fopen(filename, "rb");
  if( fp == NULL ) {
		printf( "Can not open this file!\n" );
		exit(1);
  }
  
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  size = fread(inst_rom, size, 1, fp);
  for(int i=0; i<ftell(fp)/4; i++)
  {
    printf("0x%x %x\r\n", i*4, inst_rom[i]);
  }
  fclose(fp);
}

int main(int argc, char **argv)
{
	char filename[100];
	printf("Please enter your filename~\n");
	cin >> filename;
	read_inst(filename);

  // initialization
  Verilated::commandArgs(argc, argv);

	top = new Vrvcpu;

#ifdef VM_TRACE  
    ////// !!!  ATTENTION  !!!//////
    //  Call Verilated::traceEverOn(true) first.
    //  Then create a VerilatedVcdC object.  
  Verilated::traceEverOn(true);
  printf("Enabling waves ...\n");
  tfp = new VerilatedVcdC;     //instantiating .vcd object   
  top->trace(tfp, 99);     //trace 99 levels of hierarchy
  tfp->open("top.vcd");   
  tfp->dump(0);
#endif    

	while( !Verilated::gotFinish() && main_time < sim_time )
	{
	  if( main_time % 10 == 0 ) top->clk = 0;
	  if( main_time % 10 == 5 ) top->clk = 1;
		  
	  if( main_time < 10 )
	  {
		top->rst = 1;
	  }
	  else
	  {
	    top->rst = 0;
		if( main_time % 10 == 5 )
		  top->inst = (top->inst_ena == 1) ? inst_rom[ (top->inst_addr) >> 2 ] : 0;
	  }
	  top->eval();
#ifdef VM_TRACE    
	  tfp->dump(main_time);
#endif    
	  main_time++;
	}
		
  // clean
#ifdef VM_TRACE  
  tfp->close();
  delete tfp;
#endif
  delete top;

  exit(0);
  return 0;
}
