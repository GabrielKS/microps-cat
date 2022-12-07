// Top-level module for a demonstration of a real PID control loop using a quadrature encoder to send the motor to a DIP switch-determined position
module encoder_pid_demo(
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
	assign num_display = position >> 0;  // Divide by 16 before displaying
    seven_seg ss(position, seg);
	
	// SPI RECEIVE
    logic [7:0] byte_recv;
    spi_byte sb(rst, sck, sdi, sdo, nss, byte_recv);
	
	// INPUT FABRICATION
	logic signed [15:0] setpoint;
	assign setpoint = (({{8{byte_recv[7]}}, byte_recv}));  // Multiply by 16 to correspond to the display, add 2~=80/16/2 to get to the middle of the range
	
	// P CONTROLLER
	logic signed[15:0] kp_n, kp_ds, ki_n, ki_ds, kd_n, kd_ds, max_integral_step;
	logic signed[31:0] max_integral, derivative_downsample;
	assign kp_n = 8000;
	assign kp_ds = 2;
	assign ki_n = 3;
	assign ki_ds = 12;
	assign kd_n = 0;
	assign kd_ds = 2;
	assign max_integral = 20000000;
	assign max_integral_step = 5;
	assign derivative_downsample = 8;
	logic signed[15:0] out_frac;
	pid_16 pid(clk, rst, kp_n, kp_ds, ki_n, ki_ds, kd_n, kd_ds, max_integral, max_integral_step, derivative_downsample, setpoint, position, out_frac);
	
	// PWM
	logic motor1_raw, motor2_raw;
	pwm16 o2p(clk, rst, out_frac, motor1_raw, motor2_raw);
	assign motor1 = ~motor1_raw;
	assign motor2 = ~motor2_raw;
endmodule
