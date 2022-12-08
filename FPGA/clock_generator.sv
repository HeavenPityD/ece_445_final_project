module clock_generator(
    input logic clk,
    output logic low_clk
);

logic [10:0] ct = 0;

always_ff @ (posedge clk) begin
    if (ct == 200) low_clk = 0;
    if (ct == 399) begin
        low_clk = 1;
        ct = 0;
    end else ct += 1;

end

endmodule