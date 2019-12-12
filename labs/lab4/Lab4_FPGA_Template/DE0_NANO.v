`define SCREEN_WIDTH  330 // 176: original value
`define SCREEN_HEIGHT 330 // 144: original value

///////* DON'T CHANGE THIS PART *///////
module DE0_NANO(
	CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY
);

//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED    = 8'b111_000_00;
localparam GREEN  = 8'b000_111_00;
localparam BLUE   = 8'b000_000_11;
localparam ORANGE = 8'b111_101_00;
localparam BLACK  = 8'b000_000_00;

// each tile is 30 pixels x 30 pixels
localparam BLOCK_SIZE = 8'd30;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK - DON'T NEED TO CHANGE THIS //////////
input 		          		CLOCK_50;

//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:0]		GPIO_1_D;
input 		     [1:0]		KEY;

///// PIXEL DATA /////
//reg [7:0]	pixel_data_RGB332 = 8'd0;

///// READ/WRITE ADDRESS /////
wire [14:0] X_ADDR;
wire [14:0] Y_ADDR;
wire [17:0] WRITE_ADDRESS;
reg  [17:0] READ_ADDRESS; 

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);
//assign WRITE_ADDRESS = x_data + y_data*(`SCREEN_WIDTH);

///// VGA INPUTS/OUTPUTS /////
wire 		  VGA_RESET;
wire [7:0] VGA_COLOR_IN;
wire [9:0] VGA_PIXEL_X;
wire [9:0] VGA_PIXEL_Y;
wire [1:0] MEM_OUTPUT;
wire		  VGA_VSYNC_NEG;
wire		  VGA_HSYNC_NEG;
reg		  VGA_READ_MEM_EN;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
//wire [8:0] RESULT;
wire [1:0] RESULT;

/* WRITE ENABLE */
wire W_EN;

///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////
wire c0; // 24 Mhz
wire c1; // 25 Mhz
wire c2; // 50 Mhz

// Pins for data coming from Arduino
wire        valid;
wire [11:0] x_data;
wire [11:0] y_data;
wire        north;
wire        east;
wire        south;
wire        west;
//wire        robot_north;
//wire        robot_east;
//wire        robot_south;
//wire        robot_west;
//wire        other_valid;

assign valid  = GPIO_1_D[33];
//assign x_data = ( valid == 1'b1 ) ? ( {GPIO_1_D[31],GPIO_1_D[29],GPIO_1_D[27], GPIO_1_D[11]} * BLOCK_SIZE ) : 0;
//assign y_data = ( valid == 1'b1 ) ? ( {GPIO_1_D[25],GPIO_1_D[23],GPIO_1_D[21], GPIO_1_D[9]} * BLOCK_SIZE ) : 0;

// flipped x and y
assign x_data = ( valid == 1'b1 ) ? ( ({GPIO_1_D[25],GPIO_1_D[23],GPIO_1_D[21], GPIO_1_D[9]} * BLOCK_SIZE) + 30 ) : 0;
assign y_data = ( valid == 1'b1 ) ? ( ({GPIO_1_D[31],GPIO_1_D[29],GPIO_1_D[27], GPIO_1_D[11]} * BLOCK_SIZE) + 30 ) : 0;
//assign north  = ( valid == 1'b1 ) ? GPIO_1_D[19] : 0;
//assign east   = ( valid == 1'b1 ) ? GPIO_1_D[17] : 0;
//assign south  = ( valid == 1'b1 ) ? GPIO_1_D[15] : 0;
//assign west   = ( valid == 1'b1 ) ? GPIO_1_D[13] : 0;

assign east  = GPIO_1_D[19];
assign south = GPIO_1_D[17];
assign west  = GPIO_1_D[15];
assign north = GPIO_1_D[13];

//assign robot_north  = ( valid == 1'b0 ) ? GPIO_1_D[19] : 0;
//assign robot_east   = ( valid == 1'b0 ) ? GPIO_1_D[17] : 0;
//assign robot_south  = ( valid == 1'b0 ) ? GPIO_1_D[15] : 0;
//assign robot_west   = ( valid == 1'b0 ) ? GPIO_1_D[13] : 0;

//assign other_valid = GPIO_1_D[7];

// wire to set the color on monitor
reg [7:0] pixel_color;


///////* INSTANTIATE YOUR PLL HERE *///////
lab4_pll	lab4_pll_inst (
	.inclk0 ( CLOCK_50 ),
	.c0 ( c0 ), // 24 Mhz
	.c1 ( c1 ), // 25 Mhz
	.c2 ( c2 )  // 50 Mhz
	);


///////* M9K Module *///////
Dual_Port_RAM_M9K mem(
	.input_data(RESULT),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W( c2 ),
	.clk_R( c1 ), // DO WE NEED TO READ SLOWER THAN WRITE??
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
    .RESET(VGA_RESET),
    .CLOCK( c1 ), // 25 Mhz
    .PIXEL_COLOR_IN( VGA_READ_MEM_EN ? 
								((MEM_OUTPUT == 2'b00) ? BLACK :
							    (MEM_OUTPUT == 2'b01) ? BLUE :
								 (MEM_OUTPUT == 2'b10) ? RED : ORANGE): BLACK),   // 2'b00 = ORANGE, 2'b01 = BLUE, 2'b10 = GREEN, 2'b11 = RED
    .PIXEL_X(VGA_PIXEL_X),
    .PIXEL_Y(VGA_PIXEL_Y),
    .PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
    .H_SYNC_NEG(GPIO_0_D[7]),
    .V_SYNC_NEG(VGA_VSYNC_NEG) // GPIO[5]
);

///////* Image Processor *///////
IMAGE_PROCESSOR proc(
    .clk( c2 ), // 50 Mhz
	 .valid(valid),
	 .x_tl(x_data),
	 .y_tl(y_data),
	 .x_addr(X_ADDR),
	 .y_addr(Y_ADDR),
    .result(RESULT),
	 .W_EN(W_EN),
	 .north(north),
	 .east(east),
	 .south(south),
	 .west(west)
);

//always @ (posedge c1) begin
//	if (MEM_OUTPUT == 2'b00) begin
//		pixel_color <= GREEN;
//	end
//	else if (MEM_OUTPUT == 2'b01) begin
//		pixel_color <= BLUE;
//	end
//	else if (MEM_OUTPUT == 2'b10) begin
//		pixel_color <= RED;
//	end
//	else begin
//		pixel_color <= ORANGE;
//	end
//
//end

///////* Update Read Address *///////
always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
	 READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
    if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
        VGA_READ_MEM_EN = 1'b0;
    end
    else begin
        VGA_READ_MEM_EN = 1'b1;
    end
end

endmodule
