// Top-level module for a demonstration of a real PID control loop using a quadrature encoder to send the motor to a DIP switch-determined position
module imu_motor_demo(
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
	
	// ENCODER READ
    logic signed [15:0] position;
    quadrature_decoder qd(rst, clk, encoder1, encoder2, position);
	
	// SPI RECEIVE
    logic [15:0] setpoint;
    get_roll sb(rst, sck, sdi, sdo, nss, setpoint);
	
	// PID CONTROLLER
	logic signed[15:0] kp_n, kp_ds, ki_n, ki_ds, kd_n, kd_ds, max_integral_step;
	logic signed[31:0] max_integral, derivative_downsample;
	assign kp_n = 800;
	assign kp_ds = 0;
	assign ki_n = 2;
	assign ki_ds = 14;
	assign kd_n = 7500;
	assign kd_ds = 0;
	assign max_integral = 20000000;
	assign max_integral_step = 5;
	assign derivative_downsample = 120000;
	logic signed[15:0] out_frac;
	pid_16 pid(clk, rst, kp_n, kp_ds, ki_n, ki_ds, kd_n, kd_ds, max_integral, max_integral_step, derivative_downsample, setpoint, position, out_frac);
	
	// PWM
	logic motor1_raw, motor2_raw;
	pwm16 o2p(clk, rst, out_frac, motor1_raw, motor2_raw);
	assign motor1 = ~motor1_raw;
	assign motor2 = ~motor2_raw;
endmodule
