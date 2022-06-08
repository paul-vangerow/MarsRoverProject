//cd C:/Users/Abdal/Desktop/EEE2Rover-master/DE10_LITE_D8M_VIP_16/software/D8M_Camera_Test

//nios2-download D8M_Camera_Test.elf -c 1 -g

module EEE_IMGPROC(
	// global clock & reset
	clk,
	reset_n,
	
	// mm slave
	s_chipselect,
	s_read,
	s_write,
	s_readdata,
	s_writedata,
	s_address,

	// stream sink
	sink_data,
	sink_valid,
	sink_ready,
	sink_sop,
	sink_eop,
	
	// streaming source
	source_data,
	source_valid,
	source_ready,
	source_sop,
	source_eop,
	
	// conduit
	mode,
	outbuffer,
	received_data_spi,
	received_data_byte_received
);


// global clock & reset
input	clk;
input	reset_n;

// mm slave
input							s_chipselect;
input							s_read;
input							s_write;
output	reg	[31:0]	s_readdata;
input	[31:0]				s_writedata;
input	[2:0]					s_address;


// streaming sink
input	[23:0]            	sink_data;
input								sink_valid;
output							sink_ready;
input								sink_sop;
input								sink_eop;

// streaming source
output	[23:0]			  	   source_data;
output								source_valid;
input									source_ready;
output								source_sop;
output								source_eop;

// conduit export
input                         mode;
input		[15:0]					received_data_spi;
output	[15:0]						outbuffer;
input									received_data_byte_received;
////////////////////////////////////////////////////////////////////////
//
parameter IMAGE_W = 11'd640;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT = 24'h00ff00;


wire [7:0]   red, green, blue, grey;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
////////////////////////////////////////////////////////////////////////
///Declaring variables for HSV conversion
wire[7:0] hue ;
wire[7:0] saturation, value, min;

reg red_f, orange_f, green_f, pink_f;

wire [14:0] value_spi;
wire	data_received;

assign value_spi = received_data_spi[14:0];
assign data_received = received_data_spi[15];

initial begin
	red_f <= 0;
	orange_f <= 0;
	green_f <= 0;
	pink_f <= 0;
end


always @(posedge clk)begin
	if(value_spi == 1 && data_received)begin
		red_f = 1;
	end
	if(value_spi == 4 && data_received)begin
		orange_f = 1;
	end
	if(value_spi == 2 && data_received)begin
		green_f = 1;
	end
	if(value_spi == 3 && data_received)begin
		pink_f = 1;
	end

end

/*
// Detect red areas
wire red_detect;
assign red_detect = red[7] & ~green[7] & ~blue[7];
*/
// Find boundary of cursor box


///Conversion from RGB to HSV
assign value = (red > green) ? ((red > blue) ? red[7:0] : blue[7:0]) : (green > blue) ? green[7:0] : blue[7:0];						
assign min = (red < green)? ((red<blue) ? red[7:0] : blue[7:0]) : (green < blue) ? green [7:0] : blue[7:0];
assign saturation = (value - min)* 255 / value;
assign hue = (red == green && red == blue) ? 0 :((value != red)? (value != green) ? (((240*((value - min))+ (60* (red - green)))/(value-min))>>1):
                ((120*(value-min)+60*(blue - red))/(value - min)>>1): 
                (blue < green) ? ((60*(green - blue)/(value - min))>>1): (((360*(value-min) +(60*(green - blue)))/(value - min))>>1));

///Detect Ping Pong balls
reg prev_detect_high_red, prev_high_red, prev_high_red2;
reg prev_detect_high_orange, prev_high_orange, prev_high_orange2;
reg prev_detect_high_teal, prev_high_teal, prev_high_teal2;
reg prev_detect_high_pink, prev_high_pink, prev_high_pink2;

wire red_ball_detect, pink_ball_detect, teal_ball_detect, orange_ball_detect;	
wire building_detect;
assign pink_ball_detect = //((((hue >= 150 && hue <= 180)||(hue <= 6 && hue >= 0)) && (saturation > 84 && value > 245))||
//(hue <= 6 && hue >= 0 && ((value > 229 && saturation > 17 && saturation < 155)||(value > 210 && saturation > 130)))
//|| ((hue >= 160 && hue <= 180) && ((saturation >= 76 && value >= 249) || (saturation >= 102 && value >= 140)))
//|| (((hue >= 160 && hue <= 180)||(hue >= 0 && hue <= 4)) && (saturation > 140 && saturation <= 179 && value >= 89 && value <= 106)) ||
 (((hue >= 175 && hue <= 180)||(hue >= 0 && hue <= 22)) && ((saturation > 83 && saturation < 190)) && ((value >  205 && value < 256)));

assign red_ball_detect = (//(((hue >= 160 && hue <= 180)||(hue <= 10 && hue >= 3)) && (saturation > 60 && value > 245))||
(hue <= 17 && hue >= 5 && (value <= 256 &&(value > 112 && saturation > 195 && saturation <= 256))) 
//||(((hue >= 172 && hue <= 180)||(hue >= 3 && hue <= 10)) && ((value >  60 && saturation > 80) || (saturation > 60 && value > 80)))
); //sat > 102

assign orange_ball_detect = (((hue >= 16 && hue <=25) && (saturation > 133 && value > 78)) 
|| ((hue >= 23 && hue <= 30) && ((value > 155 && saturation > 127)||(saturation >= 153 && value > 252)||(value > 41 && saturation > 247))));

assign teal_ball_detect = (((hue >= 60 && hue <= 85) && (saturation > 100 && saturation < 200) && (value > 110 && value < 245)));
//test orange
//assign orange_ball_detect = (((hue >= 14 && hue <=25) && (saturation > 160 && value > 128)) || ((hue >= 23 && hue <= 30) && ((value > 155 && saturation > 135)||(saturation >= 153 && value > 252)||(value > 109 && saturation > 247))));


//blue ball in the dark environment
//assign blue_ball_detect = (hue >= 55 && hue <= 85 && saturation >= 51 && saturation <= 89 && value >= 76 && value <= 240);




//assign pink_ball_detect = (((hue >= 0 && hue <= 7)||(hue >= 170 && hue <= 180)) && value > 111 && saturation > 102); //sat > 102
//assign teal_ball_detect = (hue >= 45 && hue <= 80 && value > 90 && saturation > 116);
//assign orange_ball_detect = (hue >= 15 && hue <= 32 && value > 130 && satura tion > 112);



initial begin
	prev_detect_high_red <= 0;
	prev_high_red <= 0;
	prev_high_red2 <= 0;
	prev_detect_high_orange <= 0;
	prev_high_orange <= 0;
	prev_high_orange2 <= 0;
	prev_detect_high_teal <= 0;
	prev_high_teal <= 0;
	prev_high_teal2 <= 0;
	prev_detect_high_pink <= 0;
	prev_high_pink <= 0;
	prev_high_pink2 <= 0;
end
//r = red
//y = orange
//g = teal
//b = pink
always@(negedge clk) begin
	prev_high_red2 = prev_high_red;
	prev_high_red = prev_detect_high_red;
	prev_detect_high_red = (red_ball_detect);
	
	prev_high_orange2 = prev_high_orange;
	prev_high_orange = prev_detect_high_orange;
	prev_detect_high_orange = (orange_ball_detect);
	
	prev_high_teal2 = prev_high_teal;
	prev_high_teal = prev_detect_high_teal;
	prev_detect_high_teal = (teal_ball_detect);
	
	prev_high_pink2 = prev_high_pink;
	prev_high_pink = prev_detect_high_pink;
	prev_detect_high_pink = (pink_ball_detect);
	
end

// Highlight detected areas
wire [23:0] color_high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign color_high  =  (red_ball_detect && prev_detect_high_red && prev_high_red && prev_high_red2) ? {8'hff,8'h10,8'h0} 
	: ((teal_ball_detect && prev_detect_high_teal && prev_high_teal)? {8'h04,8'hbd,8'h42} 
	: ((orange_ball_detect && prev_detect_high_orange && prev_high_orange && prev_high_orange2)? {8'hea,8'h9e,8'h1b} 
	: ((pink_ball_detect && prev_detect_high_pink && prev_high_pink && prev_high_pink2) ? {8'hdf,8'h55,8'he2}
	: {grey,grey,grey})) ) ;

// Show bounding box
wire [23:0] new_image_red;
wire bb_active_red;
assign bb_active_red = (x == left_red && left_red != IMAGE_W-11'h1) | (x == right_red && right_red != 0);
assign new_image_red = bb_active_red ? {24'hff0000} : color_high;

wire [23:0] new_image_orange;
wire bb_active_orange;
assign bb_active_orange = (x == left_orange && left_orange != IMAGE_W-11'h1) | (x == right_orange && right_orange != 0);
assign new_image_orange = bb_active_orange ? {24'hffff00} : new_image_red;

wire [23:0] new_image_teal;
wire bb_active_teal;
assign bb_active_teal = (x == left_teal && left_teal != IMAGE_W-11'h1) | (x == right_teal && right_teal != 0);
assign new_image_teal = bb_active_teal ? {24'h00ff00} : new_image_orange;

wire [23:0] new_image_pink;
wire bb_active_pink;
assign bb_active_pink = (x == left_pink && left_pink != IMAGE_W-11'h1) | (x == right_pink && right_pink != 0);
assign new_image_pink = bb_active_pink ? {24'h0000ff} : new_image_teal;

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? new_image_pink : {red,green,blue};

//Count valid pixels to get the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video;
always@(posedge clk) begin
	if (sop) begin
		x <= 11'h0;
		y <= 11'h0;
		packet_video <= (blue[3:0] == 3'h0);
	end
	else if (in_valid) begin
		if (x == IMAGE_W-1) begin
			x <= 11'h0;
			y <= y + 11'h1;
		end
		else begin
			x <= x + 11'h1;
		end
	end
end

//Find first and last red pixels
reg [10:0] x_min_red, x_max_red, x_min_orange, x_max_orange, x_min_teal, x_max_teal, x_min_pink, x_max_pink;
wire [10:0] x_dist_red, x_dist_orange, x_dist_teal, x_dist_pink;

reg	data_drive_mux;

initial begin
	data_drive_mux = 0;
end

reg left_or_right;

reg[15:0] drive_instr;
reg[15:0] data_value;

assign outbuffer = (data_drive_mux) ? data_value : drive_instr;


always@(posedge clk)begin
	if (distance_red == 0 && distance_orange == 0 && distance_teal == 0 && distance_pink == 0 && msg_state == 3)begin //if no balls on the screen rotate right
		//rotating right
		data_drive_mux <= 0;
		drive_instr <= {16'b0000100000000000}; //rotate the drone right 
	end
	else begin
		if ((distance_red != 0) && ((distance_red < distance_orange) || (distance_orange == 0)) && 
		((distance_red < distance_teal)|| (distance_teal == 0)) && 
		((distance_red < distance_pink) || (distance_pink == 0)) && 
		msg_state == 3) begin //red ball nearest
		
			if(x_min_red >  600)begin //case: if x_min is greater than the middle pixel
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				
				data_drive_mux <= 0;
			end
			else if(x_max_red < 40) begin//case if x_max is smaller than the middle pixel
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			else if (distance_red > 30) begin //if the red ping pong ball is too far
				drive_instr <= {16'b0100000000000000}; //move the drone forward
				data_drive_mux <= 0;
			end
			else if (distance_red < 26) begin //if the red ping pong ball is too near
				drive_instr <= {16'b0010000000000000}; //move the drone backward
				data_drive_mux <= 0;
			end
			/*
			else if (((320-x_min_red) > (x_max_red - 320)) && (((320-x_min_red) - (x_max_red - 320)) > 130))begin //if the image is more to the left 
				
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				data_drive_mux <= 0;
			end
			else if (((x_max_red - 320) > (320 - x_min_red)) && (((x_max_red - 320) - (320-x_min_red)) > 130))begin //if the image is more to the right
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			*/
			else begin //send distance measurement as long as it's within 20cm to 25cm
				data_value <= {1'b1,distance_red[4:0],3'b000,7'h0}; //send the distance + ball colour
				data_drive_mux <= 1;
			end
		end
		else if ((distance_orange != 0) &&((distance_orange < distance_red) || (distance_red == 0)) && ((distance_orange < distance_teal) || (distance_teal == 0)) && ((distance_orange < distance_pink) || (distance_pink == 0)) && msg_state == 3) begin //orange ball nearest
			if(x_min_orange > 600)begin //case: if x_min is greater than the middle pixel
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				
				data_drive_mux <= 0;
			end
			else if(x_max_orange < 40) begin//case if x_max is smaller than the middle pixel
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			else if (distance_orange > 30) begin //if the red ping pong ball is too far
				drive_instr <= {16'b0100000000000000}; //move the drone forward
				data_drive_mux <= 0;
			end
			else if (distance_orange < 26) begin //if the red ping pong ball is too near
				drive_instr <= {16'b0010000000000000}; //move the drone backward
				data_drive_mux <= 0;
			end
			/*
			else if (((320-x_min_orange) > (x_max_orange)) && (((320-x_min_orange) - (x_max_orange - 320)) > 130))begin //if the image is more to the right
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				data_drive_mux <= 0;
			end
			else if (((x_max_orange - 320) > (320 - x_min_orange)) && (((x_max_orange - 320) - (320-x_min_orange)) > 130))begin //if the image is more to the left
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			*/
			else begin //send distance measurement as long as it's within 20cm to 25cm
				data_value <= {1'b1,distance_orange[4:0],3'b001,7'h0}; //send the distance + ball colour
				data_drive_mux <= 1;
			end
			
		end
		
		else if ((distance_teal != 0) &&((distance_teal < distance_red) || (distance_red == 0)) && ((distance_teal < distance_orange) || (distance_orange == 0)) && ((distance_teal < distance_pink) || (distance_pink == 0))  && msg_state == 3)begin //green ball nearests
			if(x_min_teal > 600)begin //case: if x_min is greater than the pixel on the far right
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				
				
				data_drive_mux <= 0;
			end
			else if(x_max_teal < 40) begin//case if x_max is smaller than the middle pixel
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			else if (distance_teal > 30) begin //if the red ping pong ball is too far
				drive_instr <= {16'b0100000000000000}; //move the drone forward
				data_drive_mux <= 0;
			end
			else if (distance_teal < 26) begin //if the red ping pong ball is too near
				drive_instr <= {16'b0010000000000000}; //move the drone backward
				data_drive_mux <= 0;
			end
			/*
			else if (((320-x_min_teal) > (x_max_teal - 320)) && (((320-x_min_teal) - (x_max_teal - 320)) > 130))begin //if the image is more to the left 
				
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				data_drive_mux <= 0;
			end
			else if (((x_max_teal - 320) > (320 - x_min_teal)) && (((x_max_teal - 320) - (320-x_min_teal)) > 130))begin //if the image is more to the right
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			*/
			else begin //send distance measurement as long as it's within 20cm to 25cm
				data_value <= {1'b1,distance_teal[4:0],3'b010,7'h0}; //send the distance + ball colour
				data_drive_mux <= 1;
			end
		end
		else if ((distance_pink != 0) &&((distance_pink < distance_red) || (distance_red == 0)) && ((distance_pink < distance_orange) || (distance_orange == 0)) && ((distance_pink < distance_teal) || (distance_teal == 0)) && msg_state == 3)begin //green ball nearests
			if(x_min_pink > 600)begin //case: if x_min is greater than the middle pixel
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				data_drive_mux <= 0;
			end
			else if(x_max_pink < 40) begin//case if x_max is smaller than the middle pixel
				
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			else if (distance_pink > 30) begin //if the red ping pong ball is too far
				drive_instr <= {16'b0100000000000000}; //move the drone forward
				data_drive_mux <= 0;
			end
			else if (distance_pink < 26) begin //if the red ping pong ball is too near
				drive_instr <= {16'b0010000000000000}; //move the drone backward
				data_drive_mux <= 0;
			end
			/*
			else if (((320-x_min_pink) > (x_max_pink - 320)) && (((320-x_min_pink) - (x_max_pink - 320)) > 130))begin //if the image is more to the left 
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				data_drive_mux <= 0;
			end
			else if (((x_max_pink - 320) > (320 - x_min_pink)) && (((x_max_pink - 320) - (320-x_min_pink)) > 130))begin //if the image is more to the right
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			*/
			else begin //send distance measurement as long as it's within 20cm to 25cm
				data_value <= {1'b1,distance_pink[4:0],3'b011,7'h0}; //send the distance + ball colour
				data_drive_mux <= 1;
			end
		end
		
			/*
			else if (((320-x_min_pink) > (x_max_pink - 320)) && (((320-x_min_pink) - (x_max_pink - 320)) > 130))begin //if the image is more to the left 
				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				data_drive_mux <= 0;
			end
			else if (((x_max_pink - 320) > (320 - x_min_pink)) && (((x_max_pink - 320) - (320-x_min_pink)) > 130))begin //if the image is more to the right
				drive_instr <= {16'b0001000000000000}; //rotate the drone left
				data_drive_mux <= 0;
			end
			*/
	end
		
		/*
		else if ((distance_pink < distance_red) && (distance_pink < distance_teal) && (distance_pink < distance_orange) && msg_state == 3) begin //blue ball nearest
		else if ((distance_teal != 0) &&((distance_teal < distance_red) || (distance_red == 0)) && ((distance_teal < distance_orange) || (distance_orange == 0)) && ((distance_teal < distance_pink) || (distance_pink == 0)) && msg_state == 3)begin
		end
		*/
		
end


assign x_dist_red = (x_min_red > x_max_red) ? 0 : (x_max_red-x_min_red);
assign x_dist_orange = (x_min_orange > x_max_orange) ? 0 : (x_max_orange-x_min_orange);
assign x_dist_teal = (x_min_teal > x_max_teal) ? 0 : (x_max_teal-x_min_teal);
assign x_dist_pink = (x_min_pink > x_max_pink) ? 0 : (x_max_pink-x_min_pink);


initial begin
	x_min_red <= 0;
	x_max_red <= 0;
	x_min_orange <= 0;
	x_max_orange <= 0;
	x_min_teal <= 0;
	x_max_teal <= 0;
	x_min_pink <= 0;
	x_max_pink <= 0;

end

always@(posedge clk) begin
	//Update bounds when the pixel is red
	if ((red_ball_detect && prev_detect_high_red && prev_high_red && prev_high_red2) & in_valid) begin
		if (x < x_min_red) x_min_red <= x;
		if (x > x_max_red) x_max_red <= x;
	end
	if ((orange_ball_detect && prev_detect_high_orange && prev_high_orange && prev_high_orange2) & in_valid) begin
		if (x < x_min_orange) x_min_orange <= x;
		if (x > x_max_orange) x_max_orange <= x;
	end
	if ((teal_ball_detect && prev_detect_high_teal && prev_high_teal && prev_high_teal2) & in_valid) begin
		if (x < x_min_teal) x_min_teal <= x;
		if (x > x_max_teal) x_max_teal <= x;
	end
	if ((pink_ball_detect && prev_detect_high_pink && prev_high_pink && prev_high_pink2) & in_valid) begin
		if (x < x_min_pink) x_min_pink <= x;
		if (x > x_max_pink) x_max_pink <= x;
	end

	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_red <= IMAGE_W-11'h1;
		x_max_red <= 0;
		x_min_orange <= IMAGE_W-11'h1;
		x_max_orange <= 0;
		x_min_teal <= IMAGE_W-11'h1;
		x_max_teal <= 0;
		x_min_pink <= IMAGE_W-11'h1;
		x_max_pink <= 0;

		
	end
end

//Process bounding box at the end of the frame.
reg [1:0] msg_state;
reg [10:0] left_red, right_red, left_orange, right_orange, left_teal, right_teal, left_pink, right_pink;
reg [7:0] frame_count;
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
		
		//Latch edges for display overlay on next frame
		left_red <= x_min_red;
		right_red <= x_max_red;
		left_orange <= x_min_orange;
		right_orange <= x_max_orange;
		left_teal <= x_min_teal;
		right_teal <= x_max_teal;
		left_pink <= x_min_pink;
		right_pink <= x_max_pink;
		
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 2'b01;
			frame_count <= MSG_INTERVAL-1;
		end
	end
	
	//Cycle through message writer states once started
	if (msg_state != 2'b00) msg_state <= msg_state + 2'b01;

end
	
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;
reg [31:0] distance_red, distance_orange, distance_teal, distance_pink;
`define RED_BOX_MSG_ID "RBB"

wire[6:0] ratio1,ratio2;
assign ratio1 = 16'd79;
assign ratio2 = 16'd20;

wire [12:0] constan;
assign constan = 16'd7443;



always @(posedge clk)begin
	if(x_min_red != IMAGE_W-11'h1 && x_max_red != 0 && !red_f) begin 
		distance_red = (x_dist_red < 97) ? ((constan * ratio1)/ratio2/x_dist_red) / 10 : ((((constan - (((x_dist_red - 97) * 5)/16))* ratio1)/ratio2)/x_dist_red) / 10;
	end
	else begin
		distance_red = 0;
	end
	if (x_min_orange != IMAGE_W-11'h1 && x_max_orange != 0 && !orange_f) begin
		distance_orange = (x_dist_orange < 97) ? ((constan * ratio1)/ratio2/x_dist_orange) / 10 : ((((constan - (((x_dist_orange - 97) * 5)/16))* ratio1)/ratio2)/x_dist_orange) / 10;
	end
	else begin
		distance_orange = 0;
	end
	if (x_min_teal != IMAGE_W-11'h1 && x_max_teal != 0 && !green_f)begin
		distance_teal = (x_dist_teal < 97) ? ((constan * ratio1)/ratio2/x_dist_teal) / 10: ((((constan - (((x_dist_teal - 97) * 5)/16))* ratio1)/ratio2)/x_dist_teal) / 10;
	end
	else begin
		distance_teal = 0;
	end
	if (x_min_pink != IMAGE_W-11'h1 && x_max_pink != 0 && !pink_f) begin
		distance_pink = (x_dist_pink < 97) ? ((constan * ratio1)/ratio2/x_dist_pink) / 10: ((((constan - (((x_dist_pink - 97) * 5)/16))* ratio1)/ratio2)/x_dist_pink) / 10;
	end
	else begin
		distance_pink = 0;
	end


end
//((732 * (79/20))/147) =19.66 ish 19
 // -> 14.9
reg [15:0] outt_red, outt_orange;

//79/20 = 3.95 -> 3

always@(*) begin	//Write words to FIFO as state machine advances
	case(msg_state)
		2'b00: begin
			msg_buf_in = 32'd0; //Bottom right coordinate
			msg_buf_wr = 1'b0;
		end
		2'b01: begin
			msg_buf_in = `RED_BOX_MSG_ID;	//Message ID
			msg_buf_wr = 1'b1;
		end
		2'b10: begin
			//msg_buf_in = {5'b0, x_min, 5'b0, y_min};	//Top left coordinate
			outt_red = distance_orange[15:0];
			outt_orange = distance_red[15:0];
			/*for (i = 0; i < 16; i = i+1)begin
				out = out >> 1;
			end
			*/
			msg_buf_in = distance_teal; //Bottom right coordinate
			msg_buf_wr = 1'b1; //changed!!!!!!!!!!
		end
		2'b11: begin
			//msg_buf_in = {5'b0, x_max, 5'b0, y_max};	//Top left coordinate
			msg_buf_in = distance_red; //Bottom right coordinate
			msg_buf_wr = 1'b1;  //REPLACED WITH DISTANCE
		end
	endcase
end


//Output message FIFO
MSG_FIFO	MSG_FIFO_inst (
	.clock (clk),
	.data (msg_buf_in),
	.rdreq (msg_buf_rd),
	.sclr (~reset_n | msg_buf_flush),
	.wrreq (msg_buf_wr),
	.q (msg_buf_out),
	.usedw (msg_buf_size),
	.empty (msg_buf_empty)
	);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(sink_ready),
	.valid_out(in_valid),
	.data_out({red,green,blue,sop,eop}),
	.ready_in(out_ready),
	.valid_in(sink_valid),
	.data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(out_ready),
	.valid_out(source_valid),
	.data_out({source_data,source_sop,source_eop}),
	.ready_in(source_ready),
	.valid_in(in_valid),
	.data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port		 /////
/////////////////////////////////

// Addresses
`define REG_STATUS    			0
`define READ_MSG    				1
`define READ_ID    				2
`define REG_BBCOL					3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write

reg  [7:0]   reg_status;
reg	[23:0]	bb_col;

always @ (posedge clk)
begin
	if (~reset_n)
	begin
		reg_status <= 8'b0;
		bb_col <= BB_COL_DEFAULT;
	end
	else begin
		if(s_chipselect & s_write) begin
		   if      (s_address == `REG_STATUS)	reg_status <= s_writedata[7:0];
		   if      (s_address == `REG_BBCOL)	bb_col <= s_writedata[23:0];
		end
	end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);


// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
	   s_readdata <= {32'b0};
		read_d <= 1'b0;
	end
	
	else if (s_chipselect & s_read) begin
		if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};
		if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
		if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
		if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
	end
	
	read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);
						


endmodule
