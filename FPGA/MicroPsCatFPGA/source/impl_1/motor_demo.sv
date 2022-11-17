// Demonstration of basic motor control: drives an H-bridge with two of the incoming SPI bits
module motor_demo(
	input logic rst_raw,
    input logic sck,
    input logic sdi,
    output logic sdo,
    input logic nss,
    output logic [6:0] seg,
	output logic motor1,
	output logic motor2);
	
	logic rst;
	assign rst = ~rst_raw;
	
    logic [7:0] byte_recv;
    spi_byte sb(rst, sck, sdi, sdo, nss, byte_recv);
    seven_seg ss(byte_recv[3:0], seg);
	
	assign motor1 = ~byte_recv[0];  // Empirically, active low. 1 is counterclockwise drive
	assign motor2 = ~byte_recv[1];  // Empirically, active low. 2 is clockwise drive
endmodule

