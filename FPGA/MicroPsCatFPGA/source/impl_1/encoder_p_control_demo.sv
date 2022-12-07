// Top-level module for a demonstration of a real P control loop using a quadrature encoder to send the motor to a DIP switch-determined position
module encoder_p_control_demo(
	input logic rst_raw,
    input logic sck,
    input logic sdi,
    output logic sdo,
    input logic nss,
    input logic encoder1, encoder2,
    output logic [6:0] seg,
	output logic motor1,
	output logic motor2);
	
	// RESET
	logic rst;
	assign rst = ~rst_raw;
	
	// CLOCK
	logic clk;
	HSOSC #(.CLKHF_DIV(2'b11))
		myclock (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(clk));
	
	// ENCODER READ AND PRINTOUT
    logic signed [15:0] position;
    quadrature_decoder qd(rst, clk, encoder1, encoder2, position);
	logic signed [3:0] num_display;
	assign num_display = position >> 4;  // Divide by 16 before displaying
    seven_seg ss(num_display, seg);
	
	// SPI RECEIVE
    logic [7:0] byte_recv;
    spi_byte sb(rst, sck, sdi, sdo, nss, byte_recv);
	
	// INPUT FABRICATION
	logic signed [15:0] setpoint;
	assign setpoint = (({{8{byte_recv[7]}}, byte_recv})<<4)+2;  // Multiply by 16 to correspond to the display, add 2~=80/16/2 to get to the middle of the range
	
	// P CONTROLLER
	logic signed[15:0] kp_n, kp_ds;  // P constant numerator and denominator-exponent
	assign kp_n = 5000;
	assign kp_ds = 2;
	logic signed[15:0] out_frac;
	p_control_16 p(kp_n, kp_ds, setpoint, position, out_frac);
	
	// PWM
	logic motor1_raw, motor2_raw;
	pwm16 o2p(clk, rst, out_frac, motor1_raw, motor2_raw);
	assign motor1 = ~motor1_raw;
	assign motor2 = ~motor2_raw;
endmodule
