module pid_16(  // PID control with 16-bit signed integers
	input logic clk, rst,
	input logic signed [15:0] kp_n, kp_ds,
	input logic signed [15:0] ki_n, ki_ds,
	input logic signed [15:0] kd_n, kd_ds,
	input logic signed [15:0] max_integral, derivative_downsample;
	input logic signed [15:0] setpoint, observed,
	output logic signed [15:0] out);

	logic signed[15:0] error, integral, derivative, integral_unclamped, integral_clamped, derivative_counter, old_error;
	assign error = setpoint-observed;
	assign integral_unclamped = integral+error;
	16_clamp iclamp(integral_unclamped, -max_integral, max_integral);

	always_ff @(posedge clk) begin
		if (rst) begin
			integral <= 0;
			derivative_counter <= 0;
			old_error <= error;
		end
		else begin
			// Integral takes new state
			integral <= integral_clamped;
			
			// Is it time to update the derivative?
			if (derivative_counter == derivative_downsample) begin
				derivative = error-old_error;  // Derivative takes new state
				old_error <= error;
				derivative_counter <= 0;
			end
			else derivative_counter <= derivative_counter+1;
		end
	end

	always_ff @(posedge sck) begin
		if (rst) byte_recv <= 0;
        else byte_recv <= {byte_recv[6:0], sdi};  // Shift a new bit into the byte
    end
	
	logic signed[15:0] p_term, i_term, d_term;
	multiply_clamp mp(error, kp_n, kp_d, -'sd32768, 'sd32767, p_term);
	multiply_clamp mp(integral, ki_n, ki_d, -'sd32768, 'sd32767, i_term);
	multiply_clamp mp(derivative, kd_n, kd_d, -'sd32768, 'sd32767, d_term);
	logic signed[31:0] big_sum;
	assign big_sum = p_term+i_term+d_term;
	32_clamp c(big_sum, -'sd32768, 'sd32767, out);
endmodule

module multiply_clamp(
	input logic signed [15:0] m1, numer, denom_pow,
	input logic signed [15:0] min, max,
	output logic signed [15:0] out);

	logic signed[31:0] step1, step2;
	assign step1 = {{16{m1[15]}}, m1}*{{16{numer[15]}}, numer};  // Manually sign-extend to promote bitlength
	assign step2 = step1 >>> denom_pow;
	32_clamp c(step2, min, max, out);
endmodule

module 32_clamp(
	input logic signed[31:0] n, min, max
	output logic signed[15:0] out);
	assign out = (n < min) ? min : ((n > max) ? max : n);
endmodule

module 16_clamp(
	input logic signed[15:0] n, min, max
	output logic signed[15:0] out);
	assign out = (n < min) ? min : ((n > max) ? max : n);
endmodule
