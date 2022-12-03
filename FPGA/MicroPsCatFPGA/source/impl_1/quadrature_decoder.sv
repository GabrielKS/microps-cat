module quadrature_decoder(
    input logic s1, s2;
    output signed [15:0] position);

    logic p1, p2;  // Previous values

    always_ff @(posedge s1, posedge s2, negedge s1, negedge s2) begin
        // Lookup table from https://cdn.sparkfun.com/datasheets/Robotics/How%20to%20use%20a%20quadrature%20encoder.pdf
        case ({p1, p2, s1, s2})
            'b0010: position <= position-1;
            'b0001: position <= position+1;
            'b0100: position <= position+1;
            'b0111: position <= position-1;
            'b1000: position <= position-1;
            'b1011: position <= position+1;
            'b1110: position <= position+1;
            'b1101: position <= position-1;
        endcase

        p1 <= s1;
        p2 <= s2;
    end
endmodule
