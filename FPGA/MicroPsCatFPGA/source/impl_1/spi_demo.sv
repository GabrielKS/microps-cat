// Top-level module for a demonstration of SPI: receives a byte and displays the low nybble on a seven-segment display
module spi_demo(
	input logic rst_raw,
    input logic sck,
    input logic sdi,
    output logic sdo,
    input logic nss,
    output logic [6:0] seg);
	
	logic rst;  // reset button is active low
	assign rst = ~rst_raw;
    logic [15:0] roll;
    get_roll sb(rst, sck, sdi, sdo, nss, roll);
    seven_seg ss(~{roll[9:8], roll[1:0]}, seg);
endmodule

// Old SPI module for backwards compatibility
module spi_byte(
	input logic rst,
    input logic sck,
    input logic sdi,
    output logic sdo,
    input logic nss,
	
    output logic [7:0] byte_recv);
	
	logic [7:0] bufr;

    always_ff @(posedge sck) begin
		if (rst) bufr <= 0;
        else bufr <= {bufr[6:0], sdi};  // Shift a new bit into the byte
    end
	
	always_ff @(negedge nss) begin
		if (rst) byte_recv <= 0;
		byte_recv <= bufr;
	end
	//assign byte_recv = bufr;
endmodule


// New, two-byte SPI module
module get_roll(
	input logic rst,
    input logic sck,
    input logic sdi,
    output logic sdo,
    input logic nss,
    output logic [15:0] roll);
	
	logic [15:0] bufr;

    always_ff @(posedge sck) begin
		if (rst) bufr <= 0;
        else bufr <= {bufr[14:0], sdi};  // Shift a new bit into the byte
    end
	
	always_ff @(negedge nss) begin
		if (rst) roll <= 0;
		roll <= bufr;
	end
endmodule


module seven_seg(	input logic [3:0] switch,
			output logic [6:0] ssd);
		always_comb
			case(switch) // switch is OFF at 1
				4'b0000 : ssd = 7'b0001110;  // display F
				4'b0001 : ssd = 7'b0000110;  // display E
				4'b0010 : ssd = 7'b0100001;  // display d
				4'b0011 : ssd = 7'b1000110;  // display C
				4'b0100 : ssd = 7'b0000011;  // display b
				4'b0101 : ssd = 7'b0001000;  // display A
				4'b0110 : ssd = 7'b0011000;  // display 9
				4'b0111 : ssd = 7'b0000000;  // display 8
				4'b1000 : ssd = 7'b1111000;  // display 7
				4'b1001 : ssd = 7'b0000010;  // display 6 
				4'b1010 : ssd = 7'b0010010;  // display 5
				4'b1011 : ssd = 7'b0011001;  // display 4
				4'b1100 : ssd = 7'b0110000;  // display 3
				4'b1101 : ssd = 7'b0100100;  // display 2
				4'b1110 : ssd = 7'b1111001;  // display 1
				4'b1111 : ssd = 7'b1000000;  // display 0
			endcase
endmodule
