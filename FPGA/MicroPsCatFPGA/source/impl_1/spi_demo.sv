
// Top-level module for a demonstration of SPI: receives a byte and displays the low nybble on a seven-segment display
module spi_demo(
	input logic rst,
    input logic sck,
    input logic sdi,
    output logic sdo,
    input logic nss,
    output logic [6:0] seg);
	
	logic clk;
	HSOSC #(.CLKHF_DIV(2'b01))
		hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(clk));

    logic [7:0] byte_recv;
    spi_byte sb(rst, sck, sdi, sdo, nss, byte_recv);
    seven_seg ss(byte_recv[3:0], seg);
endmodule

// SPI module! Currently read-only.
module spi_byte(
	input logic rst,
    input logic sck,
    input logic sdi,
    output logic sdo,
    input logic nss,
	
    output logic [7:0] byte_recv);

    always_ff @(posedge sck) begin
		if (rst == 0) byte_recv <= 0;
        else byte_recv <= {byte_recv[6:0], sdi};
    end

endmodule

// The usual seven-segment module
module seven_seg(  // Encapsulates logic to display the input nybble in hex on the output seven-segment display.
	input logic [3:0] num,
	output logic [6:0] segs);
	
	always_comb
		case(num)  // Number to segment activation lookup table (active low)
			0:  segs <= 'b0000001;
			1:  segs <= 'b1001111;
			2:  segs <= 'b0010010;
			3:  segs <= 'b0000110;
			4:  segs <= 'b1001100;
			5:  segs <= 'b0100100;
			6:  segs <= 'b0100000;
			7:  segs <= 'b0001111;
			8:  segs <= 'b0000000;
			9:  segs <= 'b0000100;
			10: segs <= 'b0001000;
			11: segs <= 'b1100000;
			12: segs <= 'b0110001;
			13: segs <= 'b1000010;
			14: segs <= 'b0110000;
			15: segs <= 'b0111000;
			default: segs <= 'b0110110;  // Dogmatic; unused in practice
		endcase
endmodule
