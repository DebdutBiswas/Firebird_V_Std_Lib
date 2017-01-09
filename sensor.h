/*
 * sensor.h
 *
 * Created: 12/16/2016 3:30:10 PM
 * Author : eYRC#638
 */

/*
   ADC Connection:
   ACD CH.	PORT	Sensor
   0		PF0		Battery Voltage
   1		PF1		White line sensor 3
   2		PF2		White line sensor 2
   3		PF3		White line sensor 1
   4		PF4		IR Proximity analog sensor 1**
   5		PF5		IR Proximity analog sensor 2**
   6		PF6		IR Proximity analog sensor 3**
   7		PF7		IR Proximity analog sensor 4**
   8		PK0		IR Proximity analog sensor 5
   9		PK1		Sharp IR range sensor 1 (Generally sensor not attached to bot)
   10		PK2		Sharp IR range sensor 2 (Generally sensor not attached to bot)
   11		PK3		Sharp IR range sensor 3
   12		PK4		Sharp IR range sensor 4
   13		PK5		Sharp IR range sensor 5 (Generally sensor not attached to bot)
   14		PK6		Servo Pod 1
   15		PK7		Servo Pod 2

   ** For using Analog IR proximity (1, 2, 3 and 4) sensors short the jumper J2.
   To use JTAG via expansion slot of the micro controller socket remove these jumpers.
 */

//=========================Global Variable Declearation=============================
//Initialization of battery voltage sensor variable
unsigned int battery_voltage = 0;

//Initialization of white line sensor variables
unsigned int Left_white_line = 0;
unsigned int Center_white_line = 0;
unsigned int Right_white_line = 0;

//Initialization of ir sensor variables
unsigned int ir_sensor1 = 0;
unsigned int ir_sensor2 = 0;
unsigned int ir_sensor3 = 0;
unsigned int ir_sensor4 = 0;
unsigned int ir_sensor5 = 0;

//Initialization of sharp sensor variables
unsigned int sr_sensor1 = 0;
unsigned int sr_sensor2 = 0;
unsigned int sr_sensor3 = 0;
unsigned int sr_sensor4 = 0;
unsigned int sr_sensor5 = 0;
//===================================================================================

//===========================Global Function Declearation============================
//Main Function for initialization of sensors and ADC interrupt
void sensors_init(void);

//Sensor ports and ACD Initialization
void init_sensor_ports(void);
void init_adc();

//Function for ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch);

//Functions for update sensors data
void update_btv_sensor();
void update_wl_sensors();
void update_ir_sensors();
void update_sr_sensors();
void update_all_sensors();
//===================================================================================

//Function for initialization of sensor PORTs [F,K]
void init_sensor_ports(void)
{
	DDRF |= 0b00000000; //Initialize PORTF as input
	PORTF &= 0b00000000; //Set low output to PORTF
	DDRK |= 0b00000000; //Initialize PORTK as input
	PORTK &= 0b00000000; //Set low output to PORTK
}

//ADC converter initialization
void init_adc()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//Function for ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;
	ADMUX= 0x20| Ch;
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

//Function for getting data from battery voltage sensor
void update_btv_sensor()
{
	battery_voltage = ADC_Conversion(0);
}

//Function for getting data from white line sensors
void update_wl_sensors()
{
	Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
	Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor
}

//Function for getting data from ir proximity sensors
void update_ir_sensors()
{
	ir_sensor1 = ADC_Conversion(4);
	ir_sensor2 = ADC_Conversion(5);
	ir_sensor3 = ADC_Conversion(6);
	ir_sensor4 = ADC_Conversion(7);
	ir_sensor5 = ADC_Conversion(8);
}

//Function for getting data from sharp range sensors
void update_sr_sensors()
{
	//sr_sensor1 = ADC_Conversion(9); //Sensor not attached to bot
	//sr_sensor2 = ADC_Conversion(10); //Sensor not attached to bot
	sr_sensor3 = ADC_Conversion(11);
	sr_sensor4 = ADC_Conversion(12);
	//sr_sensor5 = ADC_Conversion(13); //Sensor not attached to bot
}

//Function for getting data from all sensors(wl,ir,sr)
void update_all_sensors()
{
	update_btv_sensor();
	update_wl_sensors();
	update_ir_sensors();
	update_sr_sensors();
}

//Main Function for initialization of sensors and ADC interrupt
void sensors_init(void)
{
	init_sensor_ports();
	init_adc();
}