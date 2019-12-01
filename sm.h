/*
 * sm.h
 *
 * Created: 19-Oct-19 03:46:26 PM
 * Modified: 23-Oct-19 09:49:10 PM
 * Author: Debdut
 * Modifier: Debdut
 */ 

#ifndef SM_H_
#define SM_H_

#define SM_PORT PORTF //SENSOR PORT REG
#define SM_PD DDRF //SENSOR PORT DATA DIRECTION REG
#define SM_PIN PINF5 //SENSOR PIN
#define SM_PDR PINF //SENSOR INPUT DATA REG

#define SM_MIN_CAL 300 //FC-28 MIN ADJUST
#define SM_MAX_CAL 10 //FC-28 MAX ADJUST

void sm_port_config(void);
void sm_init();

void adc_init();

uint16_t adc_read(uint8_t);
uint16_t update_sm_sensor();
uint8_t moistureReading();

long map(long ,long, long, long, long);

//Function to config sensor port
void sm_port_config(void)
{
	SM_PD &= ~(1 << SM_PIN);
	/* Remove J2 jumper for IR proximity analog sensor 2
	   to use PF5 (Pin No: 9 @ ATMEGA2560 Micro controller Board Expansion Socket)
	   as FC-28 sensor output to ADC channel 5 input */
}

//Function to initialize sensor
void sm_init()
{
	sm_port_config();
}

//Function to initialize 10bit-16ch ADC
void adc_init()
{
	ADMUX = 0x00; //{ REFS1=0,REFS0=0 } ADC REF VOLTAGE SELECTION TO AREF, { ADLAR=0 } ADC DATA RIGHT ADJUST, { MUX4:0=00000 } MUX INPUT SELECT ADC CHANNEL A0
	//ADMUX = 0x40; //{ REFS1=0,REFS0=1 } ADC REF VOLTAGE SELECTION TO AVCC (SET IT WHEN NOT USING AREF)
	ADCSRA = 0x86; //{ ADEN=1 } ADC ENABLE, { ADSC=0 } ADC START CONVERSION TO NO, { ADATE=0 } ADC AUTO TRIGGER DISABLE, { ADIF=0 } ADC INTERRUPT FLAG NOT SET, { ADIE=0 } ADC INTERRUPT DISABLE, { ADPS2:0=110 } ADC PRESCALER SET 64
	ADCSRB = ~(1 << MUX5); //{ MUX5=0 } MUX INPUT SELECT FIRST 8-BITS OF ADC CHANNEL { ADC7:0 }
	
	ACSR = (1 << ACD); //ADC COMPARATOR OFF (POWER SAVING)
	DIDR0 = 0xFF; //DIGITAL INPUT DISABLE ON { ADC7:0 } (POWER SAVING)
	DIDR2 = 0xFF; //DIGITAL INPUT DISABLE ON { ADC15:8 } (POWER SAVING)
}

//Function to read 10bit ADC value with dynamic channel selection using polling
uint16_t adc_read(uint8_t channel)
{
	uint16_t adc_data;
	if(channel > 0x07) ADCSRB |= (1 << MUX5); //CHECK AND SELECT CHANNEL FROM FIRST OR SECOND 8-BITS OF ADC CHANNEL
	channel &= 0x07; //ROLL OVER LAST 3-BITS WHEN CHANNEL GOES GREATER THAN 7 { 0, 1, ..., 7, 0, 1, ..., 7 } SINCE MUX5 IS THE MSB OF ADC15:0 4-BIT MUX SELECTION
	ADMUX = 0x00 | channel; //CHANNEL OVERRIDE
	//ADMUX = 0x40 | channel; //CHANNEL OVERRIDE { REFS1=0,REFS0=1 } ADC REF VOLTAGE SELECTION TO AVCC (SET IT WHEN NOT USING AREF)
	ADCSRA |= (1 << ADSC); //START CONVERSION
	while((ADCSRA & (1 << ADIF)) == 0); //WAIT FOR CONVERSION
	adc_data = ADCL | (ADCH << 8); //COPY DATA FROM ADCL & ADCH DATA REG
	
	ADCSRA |= (1 << ADIF); //CLEAR ADC INTERRUPT FLAG
	ADCSRB |= ~(1 << MUX5); //RESET MUX5
	return adc_data;
}

//Function to read FC-28 sensor ADC value
uint16_t update_sm_sensor()
{
	return adc_read(SM_PIN);
}

//Function to return moisture reading in percentage with calibration data
uint8_t moistureReading()
{
	return(map(update_sm_sensor(), SM_MIN_CAL, 1023, 100, 0));
}

//Function to return mathematical range mapping
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
