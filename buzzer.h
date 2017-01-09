/*
 * buzzer.h
 *
 * Created: 12/17/2016 1:12:32 PM
 * Author : eYRC#638
 */

/* 
  Buzzer Connection:
  Buzzer---->PC3;
 */

//Declaration of buzzer functions
void buzzer_init(void);//Function for buzzer initialization
void buzzer_pin_config (void);//Function for buzzer port configuration
void buzzer_on(void);//Function for buzzer on
void buzzer_off(void);//Function for buzzer off

//Function for buzzer port configuration
void buzzer_pin_config (void)
{
	DDRC |= 0b00001000;//0x08; 		//Setting PORTC 3 as output
	PORTC &= 0b11110111;//0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

//Function for buzzer initialization
void buzzer_init(void)
{
	cli(); //Clear global interrupt
	buzzer_pin_config(); //Set buzzer pins
	sei(); //Set global interrupt
}

//Function for buzzer on
void buzzer_on(void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore | 0b00001000;
	PORTC = port_restore;
}

//Function for buzzer off
void buzzer_off(void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore & 0b11110111;
	PORTC = port_restore;
}