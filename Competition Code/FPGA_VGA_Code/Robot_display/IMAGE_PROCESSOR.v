`define SCREEN_WIDTH 330
`define SCREEN_HEIGHT 330
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR (
	clk,
	valid,
	x_tl,
	y_tl,
   x_addr,
	y_addr,
	result,
	W_EN,
	north,
	east,
	south,
	west
);


//=======================================================
//  PORT declarations
//=======================================================

input 		  clk;
//input [6:0]   arduino_in; // {1 bit input, 3 bit x, 3 bit y}
input         valid;
input [11:0]   x_tl;
input [11:0]   y_tl;
output reg [14:0] x_addr = 15'd270;
output reg [14:0] y_addr = 15'd270;
output reg [1:0]  result;  // 2'b00 = BLACK, 2'b01 = BLUE, 2'b10 = ORANGE, 2'b11 = ORANGE
output reg        W_EN = 0;
input             north;
input             east;
input             south;
input             west;

localparam BLACK  = 2'b00;
localparam BLUE = 2'b01;
localparam RED = 2'b10;
localparam ORANGE = 2'b11;

localparam BLOCK_SIZE = 8'd30;

localparam hi_th = 24;
localparam lo_th = 6;

wire         update;
reg [14:0] count = 15'b0;
reg [6:0]  xpos  = 7'b0; //offset from tl corner of tile
reg [6:0]  ypos  = 7'b0; //up to 29.

reg [5:0] n_th;
reg [5:0] e_th;
reg [5:0] s_th;
reg [5:0] w_th;

always @ (posedge clk) begin

   if (valid && count > 5) begin
      x_addr <= x_tl + xpos;
		y_addr <= y_tl + ypos;
		
		// Incrementing xpos and ypos
		if (xpos >= 30) begin
		   if (ypos < 30) begin
				ypos <= ypos + 5'b1;	
		      xpos <= 5'b0;
				W_EN <= 1;	
		   end
			
			else begin
				W_EN <= 0;
				ypos <= 5'b0;
			   xpos <= 5'b0;
				count <= 0;
			end
		end
		
		else begin
			W_EN <= 1;
			ypos <= ypos;
		   xpos <= xpos + 5'b1;
		end
		
		n_th <= (north) ? lo_th : 0;
		e_th <= (east)  ? hi_th : 30;
		s_th <= (south) ? hi_th : 30;
		w_th <= (west)  ? lo_th : 0;
		result <= (xpos < w_th || xpos > e_th || ypos < n_th || ypos > s_th) ? RED : ORANGE;		
	end
	
	else if ( valid && count <= 10 ) begin
	   count <= count + 1;
	end
	
	else begin
	   W_EN   <= 0;
		x_addr <= x_addr;
		y_addr <= y_addr;
		count <= 0;
	end
end

endmodule
