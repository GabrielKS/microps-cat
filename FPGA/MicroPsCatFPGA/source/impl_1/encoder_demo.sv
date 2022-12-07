// Top-level module for a demonstration of reading a quadrature encoder: displays the count mod 16 as a nybble on the seven-segment display
module encoder_demo(
	input logic rst_raw,
    input logic encoder1, encoder2,
    output logic [6:0] seg);
	
	logic rst;  // reset button is active low
	assign rst = ~rst_raw;
	
	logic clk;
	HSOSC #(.CLKHF_DIV(2'b11))
		myclock (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(clk));

    logic signed [15:0] position;
    quadrature_decoder qd(rst, clk, encoder1, encoder2, position);
    seven_seg ss(position[3:0], seg);
endmodule
