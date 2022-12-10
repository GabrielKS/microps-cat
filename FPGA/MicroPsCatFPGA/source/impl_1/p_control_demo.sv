// Demonstration of PWM: drives the motor at a duty cycle depending on the input nybble

module p_control_demo(
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
	
	logic clk;
	HSOSC #(.CLKHF_DIV(2'b11))
		myclock (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(clk));

    logic [7:0] byte_recv;
    spi_byte sb(rst, sck, sdi, sdo, nss, byte_recv);
    seven_seg ss(byte_recv[3:0], seg);
	
	logic signed [15:0] setpoint, observed;
	logic signed [8:0] signed_byte_recv;
	assign signed_byte_recv = {1'b0, byte_recv};
	assign setpoint = (signed_byte_recv - 8) <<< 6;  // setpoint range = ([0, 15]-8)*64 encoder ticks = [-512, 448] encoder ticks = [-6.4, 5.6] rotations
	assign observed = 0;  // As we have only one input right now, we set the observed value to zero to test.
	
	logic signed[15:0] kp_n, kp_ds;  // With an integer numerator and a power of two denominator, we can get quite a lot of range for cheap math
	assign kp_n = 315;  // We set KP = 78.75 = 315>>2
	assign kp_ds = 2;
	
	logic signed[15:0] out_frac;
	p_control_16 p(kp_n, kp_ds, setpoint, observed, out_frac);
	
	logic motor1_raw, motor2_raw;
	pwm16 o2p(clk, rst, out_frac, motor1_raw, motor2_raw);
	
	assign motor1 = ~motor1_raw;  // Empirically, active low. 1 is counterclockwise drive
	assign motor2 = ~motor2_raw;  // Empirically, active low. 2 is clockwise drive
endmodule

module p_control_16(  // Proportional control with 16-bit signed integers
	input logic signed [15:0] kp_n, kp_ds,
	input logic signed [15:0] setpoint, observed,
	output logic signed [15:0] out);
	
	// out = (kp_n*(setpoint-observed))>>kp_ds, clamping to range of datatype and being perhaps more explicit than we need to be
	// We do some of the internal math at a higher precision
	logic signed[15:0] error;
	assign error = setpoint-observed;
	logic signed[31:0] temp1;
	assign temp1 = {16'b0, kp_n}*{{16{error[15]}}, error};  // Manually sign-extend to promote bitlength
	logic signed[31:0] temp2;
	assign temp2 = temp1 >>> kp_ds;
	assign out = (temp2 < -'sd32768) ? -'sd32768 : ((temp2 > 'sd32767) ? 'sd32767 : temp2);  // Clamp to avoid overflow
endmodule

module pwm16(  // PWM with 16-bit signed integer as input
	input logic clk, rst,
	input logic signed [15:0] frac,
	output logic out1,  // Produces outputs suitable for an H-bridge
	output logic out2);
	
	logic sign;
	assign sign = (frac < 0);
	
	logic [31:0] ticks_cycle;
	assign ticks_cycle = 1 << 8;  // Magnitude is out of 15 bits
	logic [31:0] ticks_on;
	assign ticks_on = (sign ? -frac : frac) >> 7;
	logic out_mag;
	blink_counter bc(clk, rst, ticks_cycle, ticks_on, out_mag);
	
	assign out1 = sign ? 0 : out_mag;
	assign out2 = sign ? out_mag : 0;
endmodule
