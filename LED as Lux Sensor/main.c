//using diode reverse current as ambient light sensor
//
//v0.1: 5/10/2018
//
//
//connection:
//
//
//
//                                |-------------------------|
//                                |                         |
//           |---------|--------->| LUX_PIN (floating       |
//           |         |          |          input pin)     |
//           |         |          |                         |
//   light  ---       ---         |                         |
//   -----> / \LED                |                         |
//          ---       ---         |                         |
//           |         |          |                         |
//           |         |          |                         |
//           |         |          |                         |
//          ---       ---         |                         |
//          GND       GND         |     PIC32MX250F128B     |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |
//                                |                         |>--------TX----> PC
//                                |                         |
//                                |                         |
//                                |-------------------------|
//
//
#include "config.h"						//configuration words
#include "gpio.h"
#include "delay.h"						//we use software delays
#include "uart1.h"						//we use uart1isr

//hardware configuration
#define LED_PORT			LATB
#define LED_DDR				TRISB
#define LED_PIN				(1<<7)

#define LUX_PORT			LATB
#define LUX_PORTIN			PORTB
#define LUX_DDR				TRISB
#define LUX_PIN				(1<<5)		//lux sensing pin connected to an LED (notice the polarity)
//end hardware configuration

//global defines

//global variables
char uRAM[40]="uart0 = \n\r";

//initialize the diode sensor
void lux_init(void) {
	IO_SET(LUX_PORT, LUX_PIN);			//pin idles high - capacitor charged
	IO_OUT(LUX_DDR, LUX_PIN);			//pin as output
}

//read the diode ambient sensor	
uint32_t lux_read(void) {
	uint32_t tmp;
	uint32_t cnt=0;						//cycle counter
	
	//reset the ambient sensor - this charges up the capacitor
	lux_init();
	while (IO_GET(LUX_PORTIN, LUX_PIN)==0) continue;	//wait until the capacitor is fully charged
	
	//turn TS to floating input
	IO_IN(LUX_DDR, LUX_PIN); //IO_CLR(LUX_PORT, LUX_PIN);
	//capacitor is now being discharged by the diode's dark current
	
	while (IO_GET(LUX_PORTIN, LUX_PIN)) cnt+=1;	//increment cnt until LUX goes high / capacitor charged up
		
	return cnt;
}	


int main(void) {
	uint16_t i=0;
	uint32_t tmp, cnt=0;

	mcu_init();						//reset the mcu
	IO_OUT(LED_DDR, LED_PIN);		//led as output

	uart1_init(9600);				//initialize uart - for output
	
	//initilize ambient light sensor
	lux_init();
	
	//ei();							//enable interrupts
	while (1) {
		IO_FLP(LED_PORT, LED_PIN);	//blink the led
		
		//display tmp
		tmp = lux_read();			//read the ambient light sensor
		//convert tmp to uRAM
		uRAM[20]=0, uRAM[19]='\n',  uRAM[18]='\r';
		uRAM[17]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[16]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[15]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[14]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[13]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[12]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[11]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[10]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[ 9]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[ 8]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[ 7]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[ 6]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[ 5]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[ 4]=(tmp % 10) + '0'; tmp /= 10;
		uRAM[ 3]='=';
		uRAM[ 2]='p';
		uRAM[ 1]='m';
		uRAM[ 0]='t';
		//display u RAM
		if (!uart1_busy()) uart1_puts(uRAM);

		delay_ms(100);				//just waste sometime
	}
}
