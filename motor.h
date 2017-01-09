/*
 * motor.h
 *
 * Created: 12/16/2016 3:30:10 PM
 * Author : eYRC#638
 */

/* 
  Motion control Connection:
  L-1---->PA0;		L-2---->PA1;
  R-1---->PA2;		R-2---->PA3;
  PL3 (OC5A) ----> PWM left; 	PL4 (OC5B) ----> PWM right; 
 */


#define DISTANCE_PRESCALER 5.338
#define ANGLE_PRESCALER 4.090

//=========================Global Variable Declearation=============================
//Debugging variable
unsigned int motor_debug = 0; //to keep track of debugging

//Shaft count variables
unsigned long int req_shaft_count = 0; //to keep track of required shaft count
volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder

//Distance variables
unsigned long int req_mm = 0; //to keep track of required distance
volatile unsigned long int left_mm = 0; //to keep track of left distance
volatile unsigned long int right_mm = 0; //to keep track of right distance

//Degree variables
unsigned long int req_deg = 0; //to keep track of required degree
volatile unsigned long int left_deg = 0; //to keep track of left degree
volatile unsigned long int right_deg = 0; //to keep track of right degree
//===================================================================================

//===========================Global Function Declearation============================
//Main Function for initialization of motors, encoders, pwm and encoder interrupts
void motor_init(void);

//L293D Motor Driver and Encoders Initialization
void init_motor_controller(void);
void init_encoders(void);

//timer5 initialization for PWM
void timer5_int(void);

//Interrupt initialization for left and right encoders
void left_position_encoder_interrupt_init (void);
void right_position_encoder_interrupt_init (void);

//Distance and Angle calculation upon encoders interrupt
void cal_distance(unsigned long int req_distance);
void cal_angle(unsigned long int req_angle);

//Speed control and motion data passing
void speed_control(int left_motor_speed, int right_motor_speed);
void motion_direction(unsigned char direction);

//Linear Motion Functions
void forward_mm(unsigned int speed, unsigned int distance);
void backward_mm(unsigned int speed, unsigned int distance);

//Circular Motion Functions
void circular_forward(unsigned int l_speed,unsigned int r_speed);
void circular_backward(unsigned int l_speed, unsigned int r_speed);

//Angular Forward Motion Functions
void left(unsigned int speed, unsigned int angle);
void right(unsigned int speed, unsigned int angle);

//Angular Soft Forward Motion Functions
void forward_soft_left(unsigned int speed, unsigned int angle);
void forward_soft_right(unsigned int speed, unsigned int angle);

//Angular Soft Backward Motion Functions
void backward_soft_left(unsigned int speed, unsigned int angle);
void backward_soft_right(unsigned int speed, unsigned int angle);

//Stop Bot Function
void stop(void);
//===================================================================================

//============================================Motor Initialization===========================================
void init_motor_controller(void)
{
	DDRL |= 0b00011000; //Enable/PWM pin PORTL L=3, R=4
	PORTL &= 0b00000000;
	DDRA |= 0b00001111; //Output pin PORTA L= [0,1}, R=[2,3]
	PORTA &= 0b00000000;
}
//===========================================================================================================

//==========================================Encoders Initialization=========================================
void init_encoders(void)
{
	//Initialization of Left & Right encoders
	DDRE |= 0b11001111; //Initialize PORTE pin [4,5] as input
	PORTE &= 0b00110000; //Turn on internal pull up resistors in PORTE pin [4,5]
}
//===========================================================================================================

//=========================================Interrupt Initialization========================================
//Timer5 Initialization
void timer5_int(void)
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

//Interrupt 4 enable
void left_position_encoder_interrupt_init (void)
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
	sei();   // Enables the global interrupt
}

//Interrupt 5 enable
void right_position_encoder_interrupt_init (void)
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();   // Enables the global interrupt
}

//ISR for left position encoder
ISR(INT4_vect)
{
	ShaftCountLeft++;  //increment left shaft position count
}

//ISR for right position encoder
ISR(INT5_vect)
{
	ShaftCountRight++;  //increment right shaft position count
}
//==========================================================================================================

//======================================Measuring Distance=========================================
void cal_distance(unsigned long int req_distance)
{
	if (req_distance == 0)
		return;
	
	req_shaft_count = 0;
	ShaftCountLeft = 0;
	ShaftCountRight = 0;
	left_mm = 0;
	right_mm = 0;

	req_shaft_count = req_distance/DISTANCE_PRESCALER;
	req_mm = req_distance;
	
	
	while(1)
	{
		left_mm = (ShaftCountLeft*DISTANCE_PRESCALER);
		right_mm = (ShaftCountRight*DISTANCE_PRESCALER);

		if ((ShaftCountLeft > req_shaft_count) || (ShaftCountRight > req_shaft_count))
		{
			break;
		}
	}
	stop();
}
//=================================================================================================

//========================================Measuring Angle==========================================
void cal_angle(unsigned long int req_angle)
{
	if (req_angle == 0)
		return;
	
	req_shaft_count = 0;
	ShaftCountLeft = 0;
	ShaftCountRight = 0;
	left_deg = 0;
	right_deg = 0;

	req_shaft_count = req_angle/ANGLE_PRESCALER;
	req_deg = req_angle;

	while(1)
	{
		left_deg = (ShaftCountLeft*ANGLE_PRESCALER);
		right_deg = (ShaftCountRight*ANGLE_PRESCALER);

		if ((ShaftCountLeft > req_shaft_count) || (ShaftCountRight > req_shaft_count))
		{
			break;
		}
	}
	stop();
}
//=================================================================================================

//======================================Motor Speed Control========================================
void speed_control(int left_motor_speed, int right_motor_speed)
{
	OCR5AL = (unsigned char)left_motor_speed;
	OCR5BL = (unsigned char)right_motor_speed;
}
//=================================================================================================

//======================================Motion Data Passing========================================
//Function for passing motion data
void motion_direction(unsigned char direction)
{
	unsigned char PortARestore = 0;
	
	direction &= 0b00001111; 			// removing upper nibbel as it is not needed
	PortARestore = PORTA; 			// reading the PORTA's original status
	PortARestore &= 0b11110000; 			// setting lower direction nibbel to 0
	PortARestore |= direction; 	// adding lower nibbel for direction command and restoring the PORTA status
	PORTA = PortARestore;
}
//=================================================================================================

//=======================================Motion Control===============================================
//Function for linear forward_mm motion
void forward_mm(unsigned int speed, unsigned int distance)
{
	motion_direction(0b00000110);
	speed_control(speed,speed);
	cal_distance(distance);
	_delay_us(10);
}

//Function for linear backward_mm motion
void backward_mm(unsigned int speed, unsigned int distance)
{
	motion_direction(0b00001001);
	speed_control(speed,speed);
	cal_distance(distance);
	_delay_us(10);
}

//Function for angular forward left motion
void left(unsigned int speed, unsigned int angle)
{
	motion_direction(0b00000101);
	speed_control(speed,speed);
	cal_angle(angle);
	_delay_us(10);
}

//Function for angular forward soft left motion
void forward_soft_left(unsigned int speed, unsigned int angle)
{
	motion_direction(0b00000100);
	speed_control(0,speed);
	cal_angle(angle*2);
	_delay_us(10);
}

//Function for angular backward soft left motion
void backward_soft_left(unsigned int speed, unsigned int angle)
{
	motion_direction(0b00001000);
	speed_control(0,speed);
	cal_angle(angle*2);
	_delay_us(10);
}

//Function for angular forward right motion
void right(unsigned int speed, unsigned int angle)
{
	motion_direction(0b00001010);
	speed_control(speed,speed);
	cal_angle(angle);
	_delay_us(10);
}

//Function for angular forward soft right motion
void forward_soft_right(unsigned int speed, unsigned int angle)
{
	motion_direction(0b00000010);
	speed_control(speed,0);
	cal_angle(angle*2);
	_delay_us(10);
}

//Function for angular backward soft right motion
void backward_soft_right(unsigned int speed, unsigned int angle)
{
	motion_direction(0b00000001);
	speed_control(speed,0);
	cal_angle(angle*2);
	_delay_us(10);
}

//Function for stop
void stop(void)
{
	motion_direction(0b00000000);
	speed_control(0,0);
	_delay_us(10);
}

//Function for Circular forward motion
void circular_forward(unsigned int l_speed, unsigned int r_speed)
{
	motion_direction(0b00000110);
	speed_control(l_speed,r_speed);
	//cal_distance(distance);
	_delay_us(10);
}

//Function for Circular backward motion
void circular_backward(unsigned int l_speed, unsigned int r_speed)
{
	motion_direction(0b00001001);
	speed_control(l_speed,r_speed);
	//cal_distance(distance);
	_delay_us(10);
}
//=================================================================================================

//========================================Main Initialization======================================
//Main Function for initialization of motors, encoders, pwm and encoder interrupts
void motor_init(void)
{
	init_motor_controller();
	init_encoders();

	timer5_int();
	left_position_encoder_interrupt_init ();
	right_position_encoder_interrupt_init ();
}
//=================================================================================================