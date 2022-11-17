// Demonstration of PWM: drives the motor at a duty cycle depending on the input nybble
module pwm_demo(
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
	
	logic pwm_out;
	nybble_to_pwm n2p(clk, rst, 5'd10, byte_recv[3:0], pwm_out);
	
	assign motor1 = ~pwm_out;  // Empirically, active low. 1 is counterclockwise drive
	assign motor2 = 1;  // Empirically, active low. 2 is clockwise drive
endmodule

module nybble_to_pwm(
	input logic clk, rst,
	input logic [4:0] t_pow,  // Period will be 2^t_pow*15 clock cycles (so if t_pow=0 and nyb is 1 then output will be 1 tick on, 14 ticks off)
	input logic [3:0] nyb,
	output logic out);
	
	logic [31:0] ticks_cycle;
	assign ticks_cycle = 'b1111 << t_pow;
	logic [31:0] ticks_on;
	assign ticks_on = nyb << t_pow;
	blink_counter bc(clk, rst, ticks_cycle, ticks_on, out);
endmodule

module blink_counter(  // Blink an output at a configurable speed and duty cycle, now without its own clock (edited from previous)
	input logic clk, rst,
	input logic [31:0] ticks_cycle, ticks_on,  // Clock ticks per full cycle; number of those for which the output should be on
	output logic out);
	
	logic [31:0] counter;

	always_ff @(posedge clk, posedge rst) begin
		if(rst) counter <= 0;  // Manual reset
		else if (counter < 0) counter <= 0;  // Automatic reset -- ensures that counter is always between 0 and ticks_cycle
		else if(counter > ticks_cycle) counter <= 0;  // Back to 0 after a full cycle
		else counter <= counter + 1;
	end
	
	assign out = counter <= ticks_on;
endmodule
