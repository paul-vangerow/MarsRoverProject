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

reg red_f, yellow_f, teal_f, pink_f, blue_f, green_f, black_f;

wire [14:0] value_spi;
wire	data_received;

assign value_spi = received_data_spi[14:0];
assign data_received = received_data_spi[15];

initial begin
	red_f <= 0;
	yellow_f <= 0;
	teal_f <= 0;
	pink_f <= 0;
	blue_f <= 0;
	green_f <= 0;
	black_f <= 0;
end


always @(posedge clk)begin
	if(value_spi == 1 && data_received)begin
		red_f = 1;
	end
	if(value_spi == 4 && data_received)begin
		yellow_f = 1;
	end
	if(value_spi == 2 && data_received)begin
		teal_f = 1;
	end
	if(value_spi == 3 && data_received)begin
		pink_f = 1;
	end
	if(value_spi == 4 && data_received)begin
		blue_f = 1;
	end
	if(value_spi == 5 && data_received)begin
		green_f = 1;
	end
	if(value_spi == 6 && data_received)begin
		black_f = 1;
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
reg prev_detect_high_red, prev_high_red, prev_high_red2, prev_high_red3, prev_high_red4, prev_high_red5;
reg prev_detect_high_yellow, prev_high_yellow, prev_high_yellow2, prev_high_yellow3, prev_high_yellow4, prev_high_yellow5; 
reg prev_detect_high_teal, prev_high_teal, prev_high_teal2, prev_high_teal3, prev_high_teal4, prev_high_teal5;
reg prev_detect_high_pink, prev_high_pink, prev_high_pink2, prev_high_pink3, prev_high_pink4, prev_high_pink5;
reg prev_detect_high_blue, prev_high_blue, prev_high_blue2, prev_high_blue3, prev_high_blue4, prev_high_blue5;
reg prev_detect_high_green, prev_high_green, prev_high_green2, prev_high_green3, prev_high_green4, prev_high_green5;
reg prev_detect_high_black, prev_high_black, prev_high_black2, prev_high_black3, prev_high_black4, prev_high_black5;


wire red_ball_detect, pink_ball_detect, teal_ball_detect, yellow_ball_detect, blue_ball_detect, green_ball_detect, black_ball_detect;	
wire building_detect;
assign pink_ball_detect = //((((hue >= 150 && hue <= 180)||(hue <= 6 && hue >= 0)) && (saturation > 84 && value > 245))||
//(hue <= 6 && hue >= 0 && ((value > 229 && saturation > 17 && saturation < 155)||(value > 210 && saturation > 130)))
//|| ((hue >= 160 && hue <= 180) && ((saturation >= 76 && value >= 249) || (saturation >= 102 && value >= 140)))
//|| (((hue >= 160 && hue <= 180)||(hue >= 0 && hue <= 4)) && (saturation > 140 && saturation <= 179 && value >= 89 && value <= 106)) ||
 (((hue >= 175 && hue <= 181)||(hue >= 0 && hue <= 22)) && ((saturation > 83 && saturation < 190)) && ((value >  205 && value < 256)));

assign red_ball_detect = ((((hue >= 150 && hue <= 180)||(hue <= 6 && hue >= 0)) && (saturation > 84 && value > 245))||
(hue <= 6 && hue >= 0 && ((value > 229 && saturation > 17 && saturation < 155)||(value > 210 && saturation > 130)))
|| ((hue >= 160 && hue <= 180) && ((saturation >= 76 && value >= 249) || (saturation >= 102 && value >= 140)))
|| (((hue >= 160 && hue <= 180)||(hue >= 0 && hue <= 4)) && (saturation > 140 && saturation <= 179 && value >= 89 && value <= 106)) 
||(((hue >= 172 && hue <= 180)||(hue >= 0 && hue <= 6)) && ((value >  105 && saturation > 102) || (saturation > 82 && value > 168)))); //sat > 102

/*(//(((hue >= 160 && hue <= 180)||(hue <= 10 && hue >= 3)) && (saturation > 60 && value > 245))||
(hue <= 17 && hue >= 5 && value <= 256 &&value > 112 && saturation > 195 && saturation <= 256) 
||(hue <= 17 && hue >= 8 && value > 248 && saturation > 223 && saturation <= 240)
||(hue <= 13 && hue >= 3 && value > 249 && saturation > 224 && saturation <= 230)
//||(((hue >= 172 && hue <= 180)||(hue >= 3 && hue <= 10)) && ((value >  60 && saturation > 80) || (saturation > 60 && value > 80)))
); //sat > 102
*/
assign yellow_ball_detect = (((hue >= 16 && hue <=25) && (saturation > 133 && value > 124)) || ((hue >= 23 && hue <= 30) && ((value > 155 && saturation > 127)||(saturation >= 153 && value > 252)||(value > 100 && saturation > 247))));

//(((hue >= 16 && hue <=25) && (saturation > 133 && value > 78)) 
//|| ((hue >= 23 && hue <= 30) && ((value > 155 && saturation > 127)||(saturation >= 153 && value > 252)||(value > 41 && saturation > 247))));

assign teal_ball_detect = (((hue >= 60 && hue <= 85) && (saturation > 100 && saturation < 200) && (value > 110 && value < 245))
||((hue >= 53 && hue <= 80) && (saturation > 80 && saturation < 114) && (value > 60 && value <= 256)));
//test yellow
//assign yellow_ball_detect = (((hue >= 14 && hue <=25) && (saturation > 160 && value > 128)) || ((hue >= 23 && hue <= 30) && ((value > 155 && saturation > 135)||(saturation >= 153 && value > 252)||(value > 109 && saturation > 247))));


//assign blue_ball_detect = (hue >= 55 && hue <= 85 && saturation >= 51 && saturation <= 89 && value >= 76 && value <= 240);


assign blue_ball_detect = (hue >= 75 && hue <= 95 && ((saturation >= 63 && saturation <= 112 && value >= 130)||(saturation >= 63 && saturation <= 140 && value >= 58 && value <= 125)))
|| ((hue >= 87 && hue <= 104) && ((saturation >= 90 && saturation <= 146 && value >= 91 && value <= 170) || (saturation >= 127 && saturation <= 178 && value >= 63 && value <= 89)))
|| ((hue >= 62 && hue <= 75 && saturation >= 40 && saturation <= 89 && value <= 102 && value >= 114));


/*
assign blue_ball_detect = (hue >= 75 && hue <= 95 && ((saturation >= 63 && saturation <= 112 && value >= 130)||(saturation >= 63 && saturation <= 140 && value >= 58 && value <= 135)))
|| ((hue >= 84 && hue <= 104) && ((saturation >= 43 && saturation <= 94 && value >= 105 && value <=127) || (saturation >= 90 && saturation <= 146 && value >= 91 && value <= 170) || (saturation >= 127 && saturation <= 178 && value >= 63 && value <= 89)))
|| ((hue >= 62 && hue <= 86 && saturation >= 35 && saturation <= 100 && value <= 104 && value >= 112));
*/

assign green_ball_detect = (((hue >= 50 && hue <= 75) && (saturation > 105 && value >= 75)) || ((hue >= 50 && hue <= 75) && ((saturation > 127 && value > 173))));




//assign pink_ball_detect = (((hue >= 0 && hue <= 7)||(hue >= 170 && hue <= 180)) && value > 111 && saturation > 102); //sat > 102
//assign teal_ball_detect = (hue >= 45 && hue <= 80 && value > 90 && saturation > 116);
//assign yellow_ball_detect = (hue >= 15 && hue <= 32 && value > 130 && satura tion > 112);

assign black_ball_detect = (value <= 37 && x > 10 && x < IMAGE_W-10 && y > 10 && y < IMAGE_H - 10);

//Eliminating noise

initial begin
	prev_detect_high_red <= 0;
	prev_high_red <= 0;
	prev_high_red2 <= 0;
	prev_high_red3 <= 0;
	prev_high_red4 <= 0;
	prev_high_red5 <= 0;
	prev_detect_high_yellow <= 0;
	prev_high_yellow <= 0;
	prev_high_yellow2 <= 0;
	prev_high_yellow3 <= 0;
	prev_high_yellow4 <= 0;
	prev_high_yellow5 <= 0;
	prev_detect_high_teal <= 0;
	prev_high_teal <= 0;
	prev_high_teal2 <= 0;
	prev_high_teal3 <= 0;
	prev_high_teal4 <= 0;
	prev_high_teal5 <= 0;
	prev_detect_high_pink <= 0;
	prev_high_pink <= 0;
	prev_high_pink2 <= 0;
	prev_high_pink3 <= 0;
	prev_high_pink4 <= 0;
	prev_high_pink5 <= 0;
	prev_detect_high_blue <= 0;
	prev_high_blue <= 0;
	prev_high_blue2 <= 0;
	prev_high_blue3 <= 0;
	prev_high_blue4 <= 0;
	prev_high_blue5 <= 0;
	prev_detect_high_green <= 0;
	prev_high_green <= 0;
	prev_high_green2 <= 0;
	prev_high_green3 <= 0;
	prev_high_green4 <= 0;
	prev_high_green5 <= 0;
	prev_detect_high_black <= 0;
	prev_high_black <= 0;
	prev_high_black2 <= 0;
	prev_high_black3 <= 0;
	prev_high_black4 <= 0;
	prev_high_black5 <= 0;
end
//r = red
//y = yellow
//g = teal
//b = pink
always@(negedge clk) begin
	prev_high_red5 = prev_high_red4;
	prev_high_red4 = prev_high_red3;
	prev_high_red3 = prev_high_red2;
	prev_high_red2 = prev_high_red;
	prev_high_red = prev_detect_high_red;
	prev_detect_high_red = (red_ball_detect);
	
	prev_high_yellow5 = prev_high_yellow4;
	prev_high_yellow4 = prev_high_yellow3;
	prev_high_yellow3 = prev_high_yellow2;
	prev_high_yellow2 = prev_high_yellow;
	prev_high_yellow = prev_detect_high_yellow;
	prev_detect_high_yellow = (yellow_ball_detect);
	
	prev_high_teal5 = prev_high_teal4;
	prev_high_teal4 = prev_high_teal3;
	prev_high_teal3 = prev_high_teal2;
	prev_high_teal2 = prev_high_teal;
	prev_high_teal = prev_detect_high_teal;
	prev_detect_high_teal = (teal_ball_detect);
	
	prev_high_pink5 = prev_high_pink4;
	prev_high_pink4 = prev_high_pink3;
	prev_high_pink3 = prev_high_pink2;
	prev_high_pink2 = prev_high_pink;
	prev_high_pink = prev_detect_high_pink;
	prev_detect_high_pink = (pink_ball_detect);

	prev_high_blue5 = prev_high_blue4;
	prev_high_blue4 = prev_high_blue3;
	prev_high_blue3 = prev_high_blue2;
	prev_high_blue2 = prev_high_blue;
	prev_high_blue = prev_detect_high_blue;
	prev_detect_high_blue = (blue_ball_detect);
	
	prev_high_green5 = prev_high_green4;
	prev_high_green4 = prev_high_green3;
	prev_high_green3 = prev_high_green2;
	prev_high_green2 = prev_high_green;
	prev_high_green = prev_detect_high_green;
	prev_detect_high_green = (green_ball_detect);

	prev_high_black5 = prev_high_black4;
	prev_high_black4 = prev_high_black3;
	prev_high_black3 = prev_high_black2;
	prev_high_black2 = prev_high_black;
	prev_high_black = prev_detect_high_black;
	prev_detect_high_black = (black_ball_detect);
	
end

// Colour in detected areas
wire [23:0] color_high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign color_high  =  (red_ball_detect && prev_detect_high_red && prev_high_red && prev_high_red2 && prev_high_red3 && prev_high_red4 && prev_high_red5) ? {8'hff,8'h10,8'h0} 
	: ((teal_ball_detect && prev_detect_high_teal && prev_high_teal && prev_high_teal2 && prev_high_teal3 && prev_high_teal4 && prev_high_teal5) ? {8'h00,8'h80,8'h80} 
	: ((yellow_ball_detect && prev_detect_high_yellow && prev_high_yellow && prev_high_yellow2 && prev_high_yellow3 && prev_high_yellow4 && prev_high_yellow5) ? {8'hff,8'hff,8'h00} 
	: ((pink_ball_detect && prev_detect_high_pink && prev_high_pink && prev_high_pink2 && prev_high_pink3 && prev_high_pink4 && prev_high_pink5) ? {8'hdf,8'h55,8'he2}
	: ((blue_ball_detect && prev_detect_high_blue && prev_high_blue && prev_high_blue2 && prev_high_blue3 && prev_high_blue4 && prev_high_blue5) ? {8'h00,8'h00,8'h8b}
	: ((green_ball_detect && prev_detect_high_green && prev_high_green && prev_high_green2 && prev_high_green3 && prev_high_green4 && prev_high_green5) ? {8'h90,8'hee,8'h90}
	: ((black_ball_detect && prev_detect_high_black && prev_high_black && prev_high_black2 && prev_high_black3 && prev_high_black4 && prev_high_black5) ? {8'h87,8'hce,8'heb}
	: {grey,grey,grey}) ) ) ) ) ) ;

// Show bounding box
wire [23:0] new_image_red;
wire bb_active_red;
assign bb_active_red = (x == left_red && left_red != IMAGE_W-11'h1) | (x == right_red && right_red != 0);
assign new_image_red = bb_active_red ? {24'hff0000} : color_high;

wire [23:0] new_image_yellow;
wire bb_active_yellow;
assign bb_active_yellow = (x == left_yellow && left_yellow != IMAGE_W-11'h1) | (x == right_yellow && right_yellow != 0);
assign new_image_yellow = bb_active_yellow ? {24'hffff00} : new_image_red;

wire [23:0] new_image_teal;
wire bb_active_teal;
assign bb_active_teal = (x == left_teal && left_teal != IMAGE_W-11'h1) | (x == right_teal && right_teal != 0);
assign new_image_teal = bb_active_teal ? {24'h008080} : new_image_yellow;

wire [23:0] new_image_pink;
wire bb_active_pink;
assign bb_active_pink = (x == left_pink && left_pink != IMAGE_W-11'h1) | (x == right_pink && right_pink != 0);
assign new_image_pink = bb_active_pink ? {24'hffc0cb} : new_image_teal;

wire [23:0] new_image_blue;
wire bb_active_blue;
assign bb_active_blue = (x == left_blue && left_blue != IMAGE_W-11'h1) | (x == right_blue && right_blue != 0);
assign new_image_blue = bb_active_blue ? {24'h0000ab} : new_image_pink;

wire [23:0] new_image_green;
wire bb_active_green;
assign bb_active_green = (x == left_green && left_green != IMAGE_W-11'h1) | (x == right_green && right_green != 0);
assign new_image_green = bb_active_green ? {24'h90ee90} : new_image_blue;

wire [23:0] new_image_black;
wire bb_active_black;
assign bb_active_black = (x == left_black && left_black != IMAGE_W-11'h1) | (x == right_black && right_black != 0);
assign new_image_black = bb_active_black ? {24'h000000} : new_image_green;


// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? new_image_black : {red,green,blue};

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

//Find first and last coloured pixels
reg [10:0] x_min_red, x_max_red, x_min_yellow, x_max_yellow, x_min_teal, x_max_teal, x_min_pink, x_max_pink, 
x_min_blue, x_max_blue, x_min_green, x_max_green, x_min_black, x_max_black;
wire [10:0] x_dist_red, x_dist_yellow, x_dist_teal, x_dist_pink, x_dist_blue, x_dist_green, x_dist_black;

reg	data_drive_mux;

initial begin
	data_drive_mux = 0;
end

reg left_or_right;

reg[15:0] drive_instr;
reg[15:0] data_value;

assign outbuffer = (data_drive_mux) ? data_value : drive_instr;
//HERE DRIVE START

// always@(posedge clk)begin
// 	if (distance_red == 0 && distance_yellow == 0 && distance_teal == 0 && distance_pink == 0 && msg_state == 3)begin //if no balls on the screen rotate right
// 		//rotating right
// 		data_drive_mux <= 0;
// 		drive_instr <= {16'b0000100000000000}; //rotate the drone right 
// 	end
// 	else begin
// 		if ((distance_red != 0) && ((distance_red < distance_yellow) || (distance_yellow == 0)) && 
// 		((distance_red < distance_teal)|| (distance_teal == 0)) && 
// 		((distance_red < distance_pink) || (distance_pink == 0)) && 
// 		msg_state == 3) begin //red ball nearest
		
// 			if(x_min_red >  600)begin //case: if x_min is greater than the middle pixel
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				
// 				data_drive_mux <= 0;
// 			end
// 			else if(x_max_red < 40) begin//case if x_max is smaller than the middle pixel
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_red > 30) begin //if the red ping pong ball is too far
// 				drive_instr <= {16'b0100000000000000}; //move the drone forward
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_red < 26) begin //if the red ping pong ball is too near
// 				drive_instr <= {16'b0010000000000000}; //move the drone backward
// 				data_drive_mux <= 0;
// 			end
// 			/*
// 			else if (((320-x_min_red) > (x_max_red - 320)) && (((320-x_min_red) - (x_max_red - 320)) > 130))begin //if the image is more to the left 
				
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
// 				data_drive_mux <= 0;
// 			end
// 			else if (((x_max_red - 320) > (320 - x_min_red)) && (((x_max_red - 320) - (320-x_min_red)) > 130))begin //if the image is more to the right
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			*/
// 			else begin //send distance measurement as long as it's within 20cm to 25cm
// 				data_value <= {1'b1,distance_red[4:0],3'b000,7'h0}; //send the distance + ball colour
// 				data_drive_mux <= 1;
// 			end
// 		end
// 		else if ((distance_yellow != 0) &&((distance_yellow < distance_red) || (distance_red == 0)) && ((distance_yellow < distance_teal) || (distance_teal == 0)) && ((distance_yellow < distance_pink) || (distance_pink == 0)) && msg_state == 3) begin //yellow ball nearest
// 			if(x_min_yellow > 600)begin //case: if x_min is greater than the middle pixel
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				
// 				data_drive_mux <= 0;
// 			end
// 			else if(x_max_yellow < 40) begin//case if x_max is smaller than the middle pixel
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_yellow > 30) begin //if the red ping pong ball is too far
// 				drive_instr <= {16'b0100000000000000}; //move the drone forward
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_yellow < 26) begin //if the red ping pong ball is too near
// 				drive_instr <= {16'b0010000000000000}; //move the drone backward
// 				data_drive_mux <= 0;
// 			end
// 			/*
// 			else if (((320-x_min_yellow) > (x_max_yellow)) && (((320-x_min_yellow) - (x_max_yellow - 320)) > 130))begin //if the image is more to the right
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
// 				data_drive_mux <= 0;
// 			end
// 			else if (((x_max_yellow - 320) > (320 - x_min_yellow)) && (((x_max_yellow - 320) - (320-x_min_yellow)) > 130))begin //if the image is more to the left
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			*/
// 			else begin //send distance measurement as long as it's within 20cm to 25cm
// 				data_value <= {1'b1,distance_yellow[4:0],3'b001,7'h0}; //send the distance + ball colour
// 				data_drive_mux <= 1;
// 			end
			
// 		end
		
// 		else if ((distance_teal != 0) &&((distance_teal < distance_red) || (distance_red == 0)) && ((distance_teal < distance_yellow) || (distance_yellow == 0)) && ((distance_teal < distance_pink) || (distance_pink == 0))  && msg_state == 3)begin //green ball nearests
// 			if(x_min_teal > 600)begin //case: if x_min is greater than the pixel on the far right
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
				
				
// 				data_drive_mux <= 0;
// 			end
// 			else if(x_max_teal < 40) begin//case if x_max is smaller than the middle pixel
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_teal > 30) begin //if the red ping pong ball is too far
// 				drive_instr <= {16'b0100000000000000}; //move the drone forward
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_teal < 26) begin //if the red ping pong ball is too near
// 				drive_instr <= {16'b0010000000000000}; //move the drone backward
// 				data_drive_mux <= 0;
// 			end
// 			/*
// 			else if (((320-x_min_teal) > (x_max_teal - 320)) && (((320-x_min_teal) - (x_max_teal - 320)) > 130))begin //if the image is more to the left 
				
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
// 				data_drive_mux <= 0;
// 			end
// 			else if (((x_max_teal - 320) > (320 - x_min_teal)) && (((x_max_teal - 320) - (320-x_min_teal)) > 130))begin //if the image is more to the right
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			*/
// 			else begin //send distance measurement as long as it's within 20cm to 25cm
// 				data_value <= {1'b1,distance_teal[4:0],3'b010,7'h0}; //send the distance + ball colour
// 				data_drive_mux <= 1;
// 			end
// 		end
// 		else if ((distance_pink != 0) &&((distance_pink < distance_red) || (distance_red == 0)) && ((distance_pink < distance_yellow) || (distance_yellow == 0)) && ((distance_pink < distance_teal) || (distance_teal == 0)) && msg_state == 3)begin //green ball nearests
// 			if(x_min_pink > 600)begin //case: if x_min is greater than the middle pixel
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
// 				data_drive_mux <= 0;
// 			end
// 			else if(x_max_pink < 40) begin//case if x_max is smaller than the middle pixel
				
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_pink > 30) begin //if the red ping pong ball is too far
// 				drive_instr <= {16'b0100000000000000}; //move the drone forward
// 				data_drive_mux <= 0;
// 			end
// 			else if (distance_pink < 26) begin //if the red ping pong ball is too near
// 				drive_instr <= {16'b0010000000000000}; //move the drone backward
// 				data_drive_mux <= 0;
// 			end
// 			/*
// 			else if (((320-x_min_pink) > (x_max_pink - 320)) && (((320-x_min_pink) - (x_max_pink - 320)) > 130))begin //if the image is more to the left 
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
// 				data_drive_mux <= 0;
// 			end
// 			else if (((x_max_pink - 320) > (320 - x_min_pink)) && (((x_max_pink - 320) - (320-x_min_pink)) > 130))begin //if the image is more to the right
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			*/
// 			else begin //send distance measurement as long as it's within 20cm to 25cm
// 				data_value <= {1'b1,distance_pink[4:0],3'b011,7'h0}; //send the distance + ball colour
// 				data_drive_mux <= 1;
// 			end
// 		end
		
// 			/*
// 			else if (((320-x_min_pink) > (x_max_pink - 320)) && (((320-x_min_pink) - (x_max_pink - 320)) > 130))begin //if the image is more to the left 
// 				drive_instr <= {16'b0000100000000000}; //rotate the drone right
// 				data_drive_mux <= 0;
// 			end
// 			else if (((x_max_pink - 320) > (320 - x_min_pink)) && (((x_max_pink - 320) - (320-x_min_pink)) > 130))begin //if the image is more to the right
// 				drive_instr <= {16'b0001000000000000}; //rotate the drone left
// 				data_drive_mux <= 0;
// 			end
// 			*/
// 	end
		
// 		/*
// 		else if ((distance_pink < distance_red) && (distance_pink < distance_teal) && (distance_pink < distance_yellow) && msg_state == 3) begin //blue ball nearest
// 		else if ((distance_teal != 0) &&((distance_teal < distance_red) || (distance_red == 0)) && ((distance_teal < distance_yellow) || (distance_yellow == 0)) && ((distance_teal < distance_pink) || (distance_pink == 0)) && msg_state == 3)begin
// 		end
// 		*/
		
// end
// //HERE DRIVE END

//Here we do the horizon checking and exclusion
assign x_dist_red = (x_min_red > x_max_red) ? 0 : (x_max_red-x_min_red);
assign x_dist_yellow = (x_min_yellow > x_max_yellow) ? 0 : (x_max_yellow-x_min_yellow);
assign x_dist_teal = (x_min_teal > x_max_teal) ? 0 : (x_max_teal-x_min_teal);
assign x_dist_pink = (x_min_pink > x_max_pink) ? 0 : (x_max_pink-x_min_pink);
assign x_dist_blue = (x_min_blue > x_max_blue) ? 0 : (x_max_blue-x_min_blue);
assign x_dist_green = (x_min_pink > x_max_green) ? 0 : (x_max_green-x_min_green);
assign x_dist_black = (x_min_black > x_max_black) ? 0 : (x_max_pink-x_min_black);


initial begin
	x_min_red <= 0;
	x_max_red <= 0;
	x_min_yellow <= 0;
	x_max_yellow <= 0;
	x_min_teal <= 0;
	x_max_teal <= 0;
	x_min_pink <= 0;
	x_max_pink <= 0;
	x_min_blue <= 0;
	x_max_blue <= 0;
	x_min_green <= 0;
	x_max_green <= 0;
	x_min_black <= 0;
	x_max_black <= 0;


end

always@(posedge clk) begin
	//Update bounds when the pixel is certain colour
	if ((red_ball_detect && prev_detect_high_red && prev_high_red && prev_high_red2 && prev_high_red3 && prev_high_red4 && prev_high_red5) & in_valid & y > 280) begin
		if (x < x_min_red) x_min_red <= x;
		if (x > x_max_red) x_max_red <= x;
	end
	if ((yellow_ball_detect && prev_detect_high_yellow && prev_high_yellow && prev_high_yellow2 && prev_high_yellow3 && prev_high_yellow4 && prev_high_yellow5) & in_valid & y > 280) begin
		if (x < x_min_yellow) x_min_yellow <= x;
		if (x > x_max_yellow) x_max_yellow <= x;
	end
	if ((teal_ball_detect && prev_detect_high_teal && prev_high_teal && prev_high_teal2 && prev_high_teal3 && prev_high_teal4 && prev_high_teal5) & in_valid & y > 280) begin
		if (x < x_min_teal) x_min_teal <= x;
		if (x > x_max_teal) x_max_teal <= x;
	end
	if ((pink_ball_detect && prev_detect_high_pink && prev_high_pink && prev_high_pink2 && prev_high_pink3 && prev_high_pink4 && prev_high_pink5) & in_valid & y > 280) begin
		if (x < x_min_pink) x_min_pink <= x;
		if (x > x_max_pink) x_max_pink <= x;
	end
	if ((blue_ball_detect && prev_detect_high_blue && prev_high_blue && prev_high_blue2 && prev_high_blue3 && prev_high_blue4 && prev_high_blue5) & in_valid & y > 280) begin
		if (x < x_min_blue) x_min_blue <= x;
		if (x > x_max_blue) x_max_blue <= x;
	end
	if ((green_ball_detect && prev_detect_high_green && prev_high_green && prev_high_green2 && prev_high_green3 && prev_high_green4 && prev_high_green5) & in_valid & y > 280) begin
		if (x < x_min_green) x_min_green <= x;
		if (x > x_max_green) x_max_green <= x;
	end
	if ((black_ball_detect && prev_detect_high_black && prev_high_black && prev_high_black2 && prev_high_black3 && prev_high_black4 && prev_high_black5) & in_valid & y > 280) begin
		if (x < x_min_black) x_min_black <= x;
		if (x > x_max_black) x_max_black <= x;
	end

	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_red <= IMAGE_W-11'h1;
		x_max_red <= 0;
		x_min_yellow <= IMAGE_W-11'h1;
		x_max_yellow <= 0;
		x_min_teal <= IMAGE_W-11'h1;
		x_max_teal <= 0;
		x_min_pink <= IMAGE_W-11'h1;
		x_max_pink <= 0;
		x_min_blue <= IMAGE_W-11'h1;
		x_max_blue <= 0;
		x_min_green <= IMAGE_W-11'h1;
		x_max_green <= 0;
		x_min_black <= IMAGE_W-11'h1;
		x_max_black <= 0;

		
	end
end

//Process bounding box at the end of the frame.
reg [1:0] msg_state;
reg [10:0] left_red, right_red, left_yellow, right_yellow, left_teal, right_teal, left_pink, right_pink, 
left_blue, right_blue, left_green, right_green, left_black, right_black;
reg [7:0] frame_count;
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
		
		//Latch edges for display overlay on next frame
		left_red <= x_min_red;
		right_red <= x_max_red;
		left_yellow <= x_min_yellow;
		right_yellow <= x_max_yellow;
		left_teal <= x_min_teal;
		right_teal <= x_max_teal;
		left_pink <= x_min_pink;
		right_pink <= x_max_pink;
		left_blue <= x_min_blue;
		right_blue <= x_max_blue;
		left_green <= x_min_green;
		right_green <= x_max_green;
		left_black <= x_min_black;
		right_black <= x_max_black;
		
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
reg [31:0] distance_red, distance_yellow, distance_teal, distance_pink, distance_blue, distance_green, distance_black;
`define RED_BOX_MSG_ID "RBB"

wire[6:0] ratio1,ratio2;
assign ratio1 = 16'd79;
assign ratio2 = 16'd20;

wire [12:0] constant;
assign constant = 16'd7443;


//distance calculations based on width of coloured pixels
always @(posedge clk)begin
	if(x_min_red != IMAGE_W-11'h1 && x_max_red != 0 && !red_f) begin 
		distance_red = (x_dist_red < 97) ? ((constant * ratio1)/ratio2/x_dist_red) / 10 : ((((constant - (((x_dist_red - 97) * 5)/16))* ratio1)/ratio2)/x_dist_red) / 10;
	end
	else begin
		distance_red = 0;
	end

	if (x_min_yellow != IMAGE_W-11'h1 && x_max_yellow != 0 && !yellow_f) begin
		distance_yellow = (x_dist_yellow < 97) ? ((constant * ratio1)/ratio2/x_dist_yellow) / 10 : ((((constant - (((x_dist_yellow - 97) * 5)/16))* ratio1)/ratio2)/x_dist_yellow) / 10;
	end
	else begin
		distance_yellow = 0;
	end

	if (x_min_teal != IMAGE_W-11'h1 && x_max_teal != 0 && !teal_f)begin
		distance_teal = (x_dist_teal < 97) ? ((constant * ratio1)/ratio2/x_dist_teal) / 10: ((((constant - (((x_dist_teal - 97) * 5)/16))* ratio1)/ratio2)/x_dist_teal) / 10;
	end
	else begin
		distance_teal = 0;
	end

	if (x_min_pink != IMAGE_W-11'h1 && x_max_pink != 0 && !pink_f) begin
		distance_pink = (x_dist_pink < 97) ? ((constant * ratio1)/ratio2/x_dist_pink) / 10: ((((constant - (((x_dist_pink - 97) * 5)/16))* ratio1)/ratio2)/x_dist_pink) / 10;
	end
	else begin
		distance_pink = 0;
	end

	if (x_min_green != IMAGE_W-11'h1 && x_max_green != 0 && !green_f) begin
		distance_green = (x_dist_green < 97) ? ((constant * ratio1)/ratio2/x_dist_green) / 10: ((((constant - (((x_dist_green - 97) * 5)/16))* ratio1)/ratio2)/x_dist_green) / 10;
	end
	else begin
		distance_green = 0;
	end

	if (x_min_blue != IMAGE_W-11'h1 && x_max_blue != 0 && !blue_f) begin
		distance_blue = (x_dist_blue < 97) ? ((constant * ratio1)/ratio2/x_dist_blue) / 10: ((((constant - (((x_dist_blue - 97) * 5)/16))* ratio1)/ratio2)/x_dist_blue) / 10;
	end
	else begin
		distance_blue = 0;
	end

	if (x_min_black != IMAGE_W-11'h1 && x_max_black != 0 && !black_f) begin
		distance_black = (x_dist_black < 97) ? ((constant * ratio1)/ratio2/x_dist_black) / 10: ((((constant - (((x_dist_black - 97) * 5)/16))* ratio1)/ratio2)/x_dist_black) / 10;
	end
	else begin
		distance_black = 0;
	end


end
//((732 * (79/20))/147) =19.66 ish 19
 // -> 14.9
reg [15:0] outt_red, outt_yellow;
//add out for other colours

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
			outt_red = distance_yellow[15:0];
			outt_yellow = distance_red[15:0];
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
