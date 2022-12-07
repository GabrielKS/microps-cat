module quadrature_decoder(
    input logic rst, clk, in1, in2,
    output logic signed [15:0] position);
	
	logic s1, s2;
	synchronizer sy1(clk, in1, s1);
	synchronizer sy2(clk, in2, s2);
	
    logic p1, p2;  // Previous values

    always_ff @(posedge clk) begin
		if (rst) begin
			position <= 0;
			p1 <= s1;
			p2 <= s2;
		end
		else begin
			// Lookup table from https://cdn.sparkfun.com/datasheets/Robotics/How%20to%20use%20a%20quadrature%20encoder.pdf
			case ({p2, p1, s2, s1})
				4'b0001: position <= position-1;
				4'b0010: position <= position+1;
				4'b0100: position <= position+1;
				4'b0111: position <= position-1;
				4'b1000: position <= position-1;
				4'b1011: position <= position+1;
				4'b1101: position <= position+1;
				4'b1110: position <= position-1;
				default: position <= position;
			endcase
		end

        p1 <= s1;
        p2 <= s2;
    end
endmodule


module synchronizer(  // A simple synchronizer module
	input logic clk,
	input logic raw,
	output logic syncd);

	logic buff;

	always_ff @(posedge clk) begin
		buff <= raw;
		syncd <= buff;
	end
endmodule
