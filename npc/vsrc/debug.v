

`define assert(cond) \
        if (cond !== 1'b1) begin \
            $display("ASSERTION FAILED in %m."); \
            $finish; \
        end
