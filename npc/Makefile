# Input files for Verilator

# VSRCS = $(shell find $(./vsrc ) -name "*.v")


# INCLUDE = ./vsrc

# sim:
# 	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
# 	@echo $(VSRCS)
# 	verilator --trace --cc --exe --build \
#     --top-module rvcpu \
#     -I$(INCLUDE) ./csrc/main.cpp $(VSRCS) \
# 	-LDFLAGS -"lreadline"
 
######################################################################
# Set up variables

VERILATOR = verilator


# Generate C++ in executable form
VERILATOR_FLAGS += -cc --exe
# Generate makefile dependencies (not shown as complicates the Makefile)
#VERILATOR_FLAGS += -MMD
# Optimize
# VERILATOR_FLAGS += -x-assign fast
# Warn abount lint issues; may not want this on less solid designs
VERILATOR_FLAGS += -Wall
# Make waveforms
VERILATOR_FLAGS += --trace
# Check SystemVerilog assertions
# VERILATOR_FLAGS += --assert
# Generate coverage analysis
# VERILATOR_FLAGS += --coverage
# Run Verilator in debug mode
#VERILATOR_FLAGS += --debug
# Add this trace to get a backtrace in gdb
#VERILATOR_FLAGS += --gdbbt
VERILATOR_FLAGS += --build
# 关闭 警告 unused  
VERILATOR_FLAGS += -Wno-UNUSED 
VERILATOR_FLAGS += -Wno-PINCONNECTEMPTY
VERILATOR_FLAGS += -Wno-UNDRIVEN

VERILATOR_FLAGS += -I./vsrc  -y ./vsrc 
VERILATOR_FLAGS += --top-module rvcpu

# Input files for Verilator
VERILATOR_INPUT += $(shell find $(./vsrc ) -name "*.v")
# VERILATOR_INPUT +=  ./csrc/main.cpp
VERILATOR_INPUT += $(shell find $(./csrc ) -name "*.cpp")  


default: build

build:
	@echo
	@echo "-- new process of riscv"

	@echo
	@echo "-- VERILATE ----------------"
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILATOR_INPUT)

	@echo
	@echo "-- BUILD -------------------"

run:
	@echo
	@echo "-- RUN ---------------------"
	@rm -rf logs
	@mkdir -p logs
	obj_dir/Vrvcpu

	@echo
	@echo "-- DONE --------------------"
	@echo "To see waveforms, open top.vcd in a waveform viewer"
	gtkwave top.vcd
	@echo

sim: 
	gtkwave top.vcd
	
 
count:
	find . -name "main.cpp" -or -name "*.[vc]" | xargs wc -l
 
show-config:
	$(VERILATOR) -V

clean:
	rm -rf obj_dir



include ../Makefile
