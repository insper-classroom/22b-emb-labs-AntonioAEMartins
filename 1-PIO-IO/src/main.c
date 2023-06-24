/*
	This file is resposable for the logic of the program. It is the main file of the project.

	As it is the first project, it is a simple program that turns on the LEDs when the buttons are pressed.
*/

#include "asf.h"
#include "function.c"

int main(void)
{
	system_settings();
	led_init();
	button_init();

	while (1)
	{
		if (!pio_get(BUT1_PIO, PIO_INPUT, BUT1_MASK)){
			for(int i = 0; i < 5; i++){
				pio_clear(LED1_PIO, LED1_MASK);
				delay_ms(100);
				pio_set(LED1_PIO, LED1_MASK);
				delay_ms(100);
			}
		}
		else
		{
			pio_set(LED1_PIO, LED1_MASK);
		}
		if (!pio_get(BUT2_PIO, PIO_INPUT, BUT2_MASK)){
			for(int i = 0; i < 5; i++){
				pio_clear(LED2_PIO, LED2_MASK);
				delay_ms(100);
				pio_set(LED2_PIO, LED2_MASK);
				delay_ms(100);
			}
		}
		else
		{
			pio_set(LED2_PIO, LED2_MASK);
		}
		if (!pio_get(BUT3_PIO, PIO_INPUT, BUT3_MASK)){
			for(int i = 0; i < 5; i++){
				pio_clear(LED3_PIO, LED3_MASK);
				delay_ms(100);
				pio_set(LED3_PIO, LED3_MASK);
				delay_ms(100);
			}
		}
		else
		{
			pio_set(LED3_PIO, LED3_MASK);
		}
	}
	return 0;
}
