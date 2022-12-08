module led_driver_test();

timeunit 10ns;	// Half clock cycle at 50 MHz
			// This is the amount of time represented by #1 
timeprecision 1ns;

logic clk = 0;
logic rst = 0;
logic [31:0] balance = 32'd123456;
logic cclr_neg;
logic [3:0] num;
logic clk_out;
logic next_num;
led_driver driver(.*);

always begin : CLOCK_GENERATION
#1 clk = ~clk;
end

initial begin: TEST_VECTORS
#2 rst = 1;
#2 rst = 0;

#20 next_num = 1;

end

endmodule