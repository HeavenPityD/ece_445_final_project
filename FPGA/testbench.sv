module testbench();

timeunit 10ns;	// Half clock cycle at 50 MHz
			// This is the amount of time represented by #1 
timeprecision 1ns;

logic clk = 0;
logic next_msg = 0;
logic signal = 0;
logic rst = 0;
logic cclr_neg, clk_out, msg_enable, msg_type;
logic [3:0] num;
top topModule(.*);


always begin : CLOCK_GENERATION
#1 clk = ~clk;
end

initial begin: TEST_VECTORS
#2 rst = 1;
#2 rst = 0;
#10 next_msg = 1;
#10 next_msg = 0;

#10 next_msg = 1;
#10 next_msg = 0;

#10 next_msg = 1;
#10 next_msg = 0;

#10 next_msg = 1;
#10 next_msg = 0;
end

endmodule