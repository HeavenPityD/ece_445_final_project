module top(
    input logic rst,
    input logic clk,
    input logic next_msg,
    output logic signal,
    output logic cclr_neg,
    output logic clk_out,
    output logic [3:0] num,
    output logic msg_enable,
    output logic msg_type
);
logic [167:0] outputMsg;
logic [167:0] msg;
logic [31:0] balance;
sequencer seq(.clk(clk), .msgOut(outputMsg), .msg(msg));
processor process(.clk(clk), .msg(outputMsg), .low_clk(clk_out), .*); 
led_driver driver(.*);

parameter [167:0] msg_buf[5] = {
    {24'd000000, 32'd1, 32'd96, 32'd0, 8'd0, 32'd0, 8'd0},
    {24'd000000, 32'd1, 32'd104, 32'd0, 8'd1, 32'd1, 8'd0},
    {24'd000000, 32'd1, 32'd103, 32'd0, 8'd0, 32'd2, 8'd0},
    
    {24'd000000, 32'd1, 32'd97, 32'd0, 8'd1, 32'd3, 8'd0},
    {24'd000000, 32'd1, 32'd97, 32'd0, 8'd1, 32'd3, 8'd0}
};
logic [1:0] state = 0;
logic [3:0] ct = 0;

assign msg = msg_buf[ct];
always_ff @ (posedge clk) begin
    if (state == 0 && next_msg == 1) begin
        ct += 1;
        state = 1;
    end else if (state == 1 && next_msg == 0) begin
        state = 0;
    end
end

endmodule