module processor(
    input logic rst,
    input logic clk,
    input logic low_clk,
    input logic [167:0] msg,
    output logic signal,
    output logic [31:0] balance,
    output logic msg_enable,
    output logic msg_type
);
logic [31:0] bidLevel[100];
logic [31:0] askLevel[100];
logic [31:0] bestBid = 0;
logic [31:0] myBid = 1;
logic [31:0] bestAsk = -1;
logic [31:0] myAsk = -2;
logic [31:0] seq = 0;
logic [1:0] state = 0;
logic [31:0] stock = 0;
logic [31:0] prev_ct;
logic [31:0] cur_ct;

always_ff @ (posedge low_clk) begin
    if (rst) begin
        msg_enable = 0;
    end else if (cur_ct != prev_ct) begin
        prev_ct = cur_ct;
        msg_enable = 1;
    end else begin
        msg_enable = 0;
    end
end

always_ff @ (posedge clk) begin
    if (rst) begin
        balance = 100000;
        cur_ct = 0;
        prev_ct = 0;
    end
    if (state == 0) begin
        signal = 0;
        if (seq == msg[39:8]) begin
            if (msg[7:0] == 0) begin
                if (msg[47:40] == 0) begin
                    if (msg[111:80] >= myAsk) begin
                        state = 1;
                        stock -= msg[143:112];
                        balance += msg[111:80] * msg[143:112];
                        cur_ct += 1;
                        msg_type = 1;
                    end
                    else begin
                        bidLevel[msg[111:80]-50] += msg[143:112];
                        if (msg[111:80] > bestBid) begin
                            bestBid = msg[111:80];
                            myBid = msg[111:80] + 1;
                        end
                    end
                end else begin
                    if (msg[111:80] <= myBid) begin
                        state = 1;
                        stock += msg[143:112];
                        balance -= msg[111:80] * msg[143:112];
                        cur_ct += 1;
                        msg_type = 0;
                    end
                    else begin
                        askLevel[msg[111:80]-50] += msg[143:112];
                        if (msg[111:80] < bestAsk) begin
                            bestAsk = msg[111:80];
                            myAsk = msg[111:80] - 1;
                        end
                    end
                end
            seq += 1;
            end
        end
    end else if (state == 1) begin
        state = 0;
        signal = 1;
    end
end

endmodule