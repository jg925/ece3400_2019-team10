`define SCREEN_WIDTH 270
`define SCREEN_HEIGHT 270
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR (
	//PIXEL_IN,
	clk,
	arduino_in,
	//VGA_PIXEL_X,
	//VGA_PIXEL_Y,
	//VGA_VSYNC_NEG,
	w_addr,
	result,
	W_EN
);


//=======================================================
//  PORT declarations
//=======================================================
//input	[7:0]	PIXEL_IN;
input 		  clk;
input [6:0]   arduino_in; // {1 bit input, 3 bit x, 3 bit y}
//input [9:0] VGA_PIXEL_X;
//input [9:0] VGA_PIXEL_Y;
//input			VGA_VSYNC_NEG;
output [14:0] w_addr;
//output [7:0]  result;
output        result;
output        W_EN;

localparam RED    = 8'b111_000_00;
localparam GREEN  = 8'b000_111_00;
localparam BLUE   = 8'b000_000_11;
localparam ORANGE = 8'b111_101_00;
localparam BLACK  = 8'b000_000_00;

localparam BLOCK_SIZE = 8'd30;

wire [6:0] arduino_in;
wire [2:0] x;
wire [2:0] y;

assign in_data = arduino_in[6];
assign x = arduino_in[5:3];
assign y = arduino_in[2:0];

reg [7:0] x_tl;
reg [7:0] y_tl;
wire      in_data;
reg       W_EN_reg;

//reg [7:0] result_reg;
reg result_reg;

reg [14:0] w_addr_reg;

//int xpos, int ypos = 0;
//any other vars
reg [4:0] xpos; //offset from tl corner of tile
reg [4:0] ypos; //up to 29. 

always @(posedge clk) begin //any time, the robot goes into a new tile. x, y, or input changes.
    //assign color to tile
	 //top left corner = 0,0
	case (x)
		3'b000 : x_tl <= 0*BLOCK_SIZE;
		3'b001 : x_tl <= 1*BLOCK_SIZE;
		3'b010 : x_tl <= 2*BLOCK_SIZE;
		3'b011 : x_tl <= 3*BLOCK_SIZE;
		3'b100 : x_tl <= 4*BLOCK_SIZE;
		3'b101 : x_tl <= 5*BLOCK_SIZE;
		3'b110 : x_tl <= 6*BLOCK_SIZE;
		3'b111 : x_tl <= 7*BLOCK_SIZE;
		default : x_tl <= 0;
	endcase
	
	case (y)
		3'b000 : y_tl <= 0*BLOCK_SIZE;
		3'b001 : y_tl <= 1*BLOCK_SIZE;
		3'b010 : y_tl <= 2*BLOCK_SIZE;
		3'b011 : y_tl <= 3*BLOCK_SIZE;
		3'b100 : y_tl <= 4*BLOCK_SIZE;
		3'b101 : y_tl <= 5*BLOCK_SIZE;
		3'b110 : y_tl <= 6*BLOCK_SIZE;
		3'b111 : y_tl <= 7*BLOCK_SIZE;
		default : y_tl <= 0;
	endcase
end

always @ (posedge clk) begin
    if (arduino_in > 0) begin
	     //result_reg <= some data that gets stored in m9k
		  //READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		  w_addr_reg <= (x_tl + xpos + (y_tl + ypos)*`SCREEN_WIDTH);
		  result_reg <= 1'b1;
		  
		  if (xpos >= 30 && ypos >= 30) begin  //reset tile
		      //update <= 1'b0; 
				xpos <= 5'b0;
				ypos <= 5'b0;
				W_EN_reg <= 0;		
		  end
		  else if (xpos >= 30) begin //switch rows
		      xpos <= 5'b0;
				ypos <= ypos + 5'b1;
				W_EN_reg <= 1;
				
		  end else begin   //stay in same row
		      xpos <= xpos + 5'b1;
				W_EN_reg <= 1;
		  end
	 end
	 else begin 
	     result_reg <= 1'b0;
//		  w_addr_reg <= (x_tl + xpos + (y_tl + ypos)*`SCREEN_WIDTH);
		  W_EN_reg <= 0;
	 end

end

assign W_EN   = W_EN_reg;
assign result = result_reg;
assign w_addr = w_addr_reg;

endmodule
