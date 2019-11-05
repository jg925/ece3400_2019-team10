`define SCREEN_WIDTH  270 // 176: original value
`define SCREEN_HEIGHT 270 // 144: original value

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
//localparam RED    = 8'b111_000_00;
//localparam GREEN  = 8'b000_111_00;
localparam BLUE   = 8'b000_000_11;
localparam ORANGE = 8'b111_101_00;
localparam BLACK  = 8'b000_000_00;

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
reg  [14:0] X_ADDR;
reg  [14:0] Y_ADDR;
wire [14:0] WRITE_ADDRESS;
reg  [14:0] READ_ADDRESS; 

//assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);
//assign WRITE_ADDRESS = x_data + y_data*(`SCREEN_WIDTH);

///// VGA INPUTS/OUTPUTS /////
wire 		VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire		VGA_VSYNC_NEG;
wire		VGA_HSYNC_NEG;
reg		VGA_READ_MEM_EN;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
wire [8:0] RESULT;

/* WRITE ENABLE */
reg W_EN;

///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////
wire c0; // 24 Mhz
wire c1; // 25 Mhz
wire c2; // 50 Mhz

// Pins for data coming from Arduino
wire input_data_1;
wire [1:0] x_data;
wire [1:0] y_data;
assign input_data_1 = GPIO_1_D[33];
assign x_data       = {GPIO_1_D[31],GPIO_1_D[27]};
assign y_data       = {GPIO_1_D[29],GPIO_1_D[25]};

// wire to set the color on monitor
//reg [7:0] pixel_color;

// counters to help drawing shapes
//reg [9:0]  counter;
//reg        counter_2;

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
	.clk_R( c0 ), // DO WE NEED TO READ SLOWER THAN WRITE??
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
    .RESET(VGA_RESET),
    .CLOCK( c1 ), // 25 Mhz
    .PIXEL_COLOR_IN( VGA_READ_MEM_EN ? MEM_OUTPUT : BLUE ),
    .PIXEL_X(VGA_PIXEL_X),
    .PIXEL_Y(VGA_PIXEL_Y),
    .PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
    .H_SYNC_NEG(GPIO_0_D[7]),
    .V_SYNC_NEG(VGA_VSYNC_NEG) // GPIO[5]
);

///////* Image Processor *///////
IMAGE_PROCESSOR proc(
    //.PIXEL_IN(MEM_OUTPUT),
    .clk( c1 ), // 25 Mhz
	 .arduino_in({x_data, y_data, input_data_1}),
    //.VGA_PIXEL_X(VGA_PIXEL_X),
    //.VGA_PIXEL_Y(VGA_PIXEL_Y),
   // .VGA_VSYNC_NEG(VGA_VSYNC_NEG),
	 .w_addr(WRITE_ADDRESS),
    .result(RESULT)
);

// Update Write Address
// Memory maps exactly 1:1 with screen.
/*   I think eventually we just want to write to the top left corner.
always @ (x_data, y_data) begin
    //x_data = {0,1,2,3}
    //y_data = {0,1,2,3}
    /* 
    
//    */
//    if (x_data == 2'd0) begin
//        X_ADDR = x_data;
//    end
//    else if (x_data == 2'd1) begin
//         
//    end
//    else if (x_data == 2'd2) begin
//
//    end
//    else if (x_data == 2'd3) begin
//
//    end
//    X_ADDR = x_data;
//    Y_ADDR = y_data;
//end


// Setting pixel color
/*
always @ (posedge c2) begin
//	if (VGA_PIXEL_X > 100) begin
//		pixel_color <= ORANGE;
//	end
//	
//	else begin
//		pixel_color <= BLUE;
//	end
    if (MEM_OUTPUT) begin
        pixel_color <= GREEN;
    end
	
    else begin
        pixel_color <= BLUE;
    end
end*/

//reg [1:0] inc_x;
//reg [1:0] inc_y;
//assign inc = 2'd0;
//assign inc = 2'd0;
///////* Update Read Address *///////
always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
    W_EN = 1;
    /*
	 if (inc_x <= 2'd3) begin //keep same read. Basically paints a 4x4 box given the top left corner(?)
	     READ_ADDRESS = READ_ADDRESS;
        inc_x <= inc_x + 2'd1;
    end else if (inc_y <= 2'd3) begin
        READ_ADDRESS = READ_ADDRESS;
        inc_y <= inc_y + 2'd1;
    end else begin
        READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
        inc_x <= 2'd0;
        inc_y <= 2'd0;
    end*/
    //READ_ADDRESS = WRITE_ADDRESS;
	 READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
    if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
        VGA_READ_MEM_EN = 1'b0;
    end
    else begin
        VGA_READ_MEM_EN = 1'b1;
    end
end

endmodule
