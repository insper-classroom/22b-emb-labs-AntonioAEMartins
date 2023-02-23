#include "asf.h"

#define BUT1_PIO PIOD
#define BUT1_ID ID_PIOD
#define BUT1_IDX 28
#define BUT1_MASK (1 << BUT1_IDX)

#define BUT2_PIO PIOC
#define BUT2_ID ID_PIOC
#define BUT2_IDX 31
#define BUT2_MASK (1 << BUT2_IDX)

#define BUT3_PIO PIOA
#define BUT3_ID ID_PIOA
#define BUT3_IDX 19
#define BUT3_MASK (1 << BUT3_IDX)

#define LED1_PIO PIOA
#define LED1_ID ID_PIOA
#define LED1_IDX 0
#define LED1_MASK (1 << LED1_IDX)

#define LED2_PIO PIOC
#define LED2_ID ID_PIOC
#define LED2_IDX 30
#define LED2_MASK (1 << LED2_IDX)

#define LED3_PIO PIOB
#define LED3_ID ID_PIOB
#define LED3_IDX 2
#define LED3_MASK (1 << LED3_IDX)

void init(void)
{
	sysclk_init();											// INIT Board Clock
	WDT->WDT_MR = WDT_MR_WDDIS;								// Desativa WatchDog Timer

	//Led 1
	pmc_enable_periph_clk(LED1_ID);						// Ativa o PIO utilizando ID, Modo de Entrada
	pio_set_output(LED1_PIO, LED1_MASK, 0, 0, 0);		// Configura PIOC 8 como saida
	//Led 2
	pmc_enable_periph_clk(LED2_ID);
	pio_set_output(LED2_PIO, LED2_MASK, 0, 0, 0);
	//Led 3
	pmc_enable_periph_clk(LED3_ID);
	pio_set_output(LED3_PIO, LED3_MASK, 0, 0, 0);
	//Button 1
	pmc_enable_periph_clk(BUT1_ID);								// Ativando Botao 1
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_MASK, PIO_PULLUP);	// Configurando Botao 1
	// Button 2
	pmc_enable_periph_clk(BUT2_ID);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_MASK, PIO_PULLUP);
	// Button 3
	pmc_enable_periph_clk(BUT3_ID);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_MASK, PIO_PULLUP);
}

int main(void)
{
	init();

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
