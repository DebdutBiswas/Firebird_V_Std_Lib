/*
 * pir.h
 *
 * Created: 10-Oct-19 06:10:28 PM
 * Modified: 23-Oct-19 08:29:05 PM
 * Author: Debdut
 * Modifier: Debdut
 */ 

#ifndef PIR_H_
#define PIR_H_

#define PIR_PORT PORTL //SENSOR PORT REG
#define PIR_PD DDRL //SENSOR PORT DATA DIRECTION REG
#define PIR_PIN PINL7 //SENSOR PIN
#define PIR_PDR PINL //SENSOR INPUT DATA REG

void pir_port_config(void);
void pir_init();
bool pirReading();

//Function to config sensor port
void pir_port_config(void)
{
	PIR_PD &= ~(1 << PIR_PIN); //set sensor pin io direction to input
	//PIR_PORT &= ~(1 << PIR_PIN); //write low to pin
}

//Function to initialize sensor
void pir_init()
{
	pir_port_config();
}

//Function to get sensor digital data
bool pirReading()
{
	if((PIR_PDR & (1 << PIR_PIN)) >> PIR_PIN) return true; //check if pin input level is high
	return false;
}

#endif
