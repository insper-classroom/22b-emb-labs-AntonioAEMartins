#include "function.h"

void led_init(void){
	//Led 1
	pmc_enable_periph_clk(LED1_ID);						// Activating the clock of the LED 1
	pio_set_output(LED1_PIO, LED1_MASK, 0, 0, 0);		// Configuring the pin of the LED 1
	//Led 2
	pmc_enable_periph_clk(LED2_ID);   				
	pio_set_output(LED2_PIO, LED2_MASK, 0, 0, 0);
	//Led 3
	pmc_enable_periph_clk(LED3_ID);
	pio_set_output(LED3_PIO, LED3_MASK, 0, 0, 0);
}

void button_init(void){
	//Button 1
	pmc_enable_periph_clk(BUT1_ID);								// Activating the clock of the button 1
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_MASK, PIO_PULLUP);	// Configuring the pin of the button 1
	// Button 2
	pmc_enable_periph_clk(BUT2_ID);							
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_MASK, PIO_PULLUP);
	// Button 3
	pmc_enable_periph_clk(BUT3_ID);							
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_MASK, PIO_PULLUP);	
}

void system_settings(void){
	sysclk_init(); 				// INIT Board Clock
	WDT->WDT_MR = WDT_MR_WDDIS;	// Deactivate Watch Dog
}