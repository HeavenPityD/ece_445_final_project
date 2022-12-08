module sequencer(
    input logic clk,
    input logic [167:0] msg,
    output logic [167:0] msgOut
);
logic [168:0] mem[50];
logic [31:0] seq = 0;

always_ff @(posedge clk) begin
    if (msg[39:8] >= seq)
        mem[msg[39:8]%50] = {1'b1, msg};
end

always_ff @(posedge clk) begin
    if (mem[seq%50][168] == 1'b1) begin
        msgOut = mem[seq%50][167:0];
        //mem[seq%50][168] = 0;
        seq += 1;
    end
end

endmodule