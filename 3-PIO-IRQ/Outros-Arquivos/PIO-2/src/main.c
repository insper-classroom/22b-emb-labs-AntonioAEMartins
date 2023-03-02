#include "asf.h"
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)

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

void _pio_set(Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask){
	p_pio->PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable){
	if (ul_pull_up_enable){
		p_pio->PIO_PUER = ul_mask;
	} else{
		p_pio->PIO_PUDR = ul_mask;
	}
}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute){
	if (ul_attribute & _PIO_DEBOUNCE){ //Isolo um BIT, se o resultado for 1000 = 8, devo ativar o Filtro Debounce
		p_pio->PIO_IFER = ul_mask;
	}
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask,
						const uint32_t ul_default_level,
						const uint32_t ul_multidrive_enable,
						const uint32_t ul_pull_up_enable)
{
	//Configurando PIO para controlar Pino
	p_pio->PIO_PER = ul_mask;
	
	//Configurando Pino em Modo Saida
	p_pio->PIO_OER = ul_mask;
	
	//Definir Saida Padrao do Pino
	if (ul_default_level){
		p_pio->PIO_SODR = ul_mask;
	} else{
		p_pio->PIO_CODR = ul_mask;
	}
	
	//Multi Drive
		//Pull-up Resistor
	_pio_pull_up(p_pio, ul_mask, PIO_PULLUP);
	if (ul_multidrive_enable){
		p_pio->PIO_MDER = ul_mask;
	} else{
		p_pio->PIO_MDDR = ul_mask;
	}
}

uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type, const uint32_t ul_mask)
{
	uint32_t ul_ref;
	if (ul_type == PIO_OUTPUT_0){
		ul_ref = p_pio->PIO_ODSR;
		} else{
		ul_ref = p_pio->PIO_PDSR;
	}
	
	if (!(ul_ref & ul_mask)){
		return 0;
		} else{
		return 1;
	}
}

void _delay_ms(int delay)
{
	int ciclos = 200e3 * delay;
	
	for (int i = 0; i < ciclos; i++){
		asm("nop");
	}
}

volatile char but_flag1 = 0;
volatile char but_flag2 = 0;
volatile char but_flag3 = 0;
volatile int frequencia = 100;

void pisca_led(int n, int t, int led){
	if (led == 1){
		for (int i=0; i<n; i++){
			_pio_clear(LED1_PIO, LED1_MASK);
			_delay_ms(t);
			_pio_set(LED1_PIO, LED1_MASK);
			_delay_ms(t);
		}
	}
	else if (led == 2){
		for (int i=0; i<n; i++){
			_pio_clear(LED2_PIO, LED2_MASK);
			_delay_ms(t);
			_pio_set(LED2_PIO, LED2_MASK);
			_delay_ms(t);
		}
	}
	else if (led == 3){
		for (int i=0; i<n; i++){
			_pio_clear(LED3_PIO, LED3_MASK);
			_delay_ms(t);
			_pio_set(LED3_PIO, LED3_MASK);
			_delay_ms(t);
		}
	}

}

void but_callback1(void){
	but_flag1 = 1;
}

void but_callback2(void){
	but_flag2 = 1;
}

void but_callback3(void){
	but_flag3 = 1;
}

void init(void)
{
	sysclk_init();		
	
	// Initializing OLED-display
			
								// INIT Board Clock
	WDT->WDT_MR = WDT_MR_WDDIS;								// Desativa WatchDog Timer

	//Led 1
	pmc_enable_periph_clk(LED1_ID);						// Ativa o PIO utilizando ID, Modo de Entrada
	_pio_set_output(LED1_PIO, LED1_MASK, 0, 0, 0);		// Configura PIOC 8 como saida
	//Led 2
	pmc_enable_periph_clk(LED2_ID);
	_pio_set_output(LED2_PIO, LED2_MASK, 0, 0, 0);
	//Led 3
	pmc_enable_periph_clk(LED3_ID);
	_pio_set_output(LED3_PIO, LED3_MASK, 0, 0, 0);
	//Button 1
	pmc_enable_periph_clk(BUT1_ID);								// Ativando Botao 1
	_pio_set_input(BUT1_PIO, BUT1_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	// Button 2
	pmc_enable_periph_clk(BUT2_ID);
	_pio_set_input(BUT2_PIO, BUT2_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	// Button 3
	pmc_enable_periph_clk(BUT3_ID);
	_pio_set_input(BUT3_PIO, BUT3_MASK, _PIO_PULLUP | _PIO_DEBOUNCE);
	
	// Adding Interruptions - Button 1
	pio_handler_set(BUT1_PIO, BUT1_ID, BUT1_MASK, PIO_IT_FALL_EDGE, but_callback1);
	pio_enable_interrupt(BUT1_PIO, BUT1_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	NVIC_EnableIRQ(BUT1_ID);
	NVIC_SetPriority(BUT1_ID, 4);
	
	// Adding Interruptions - Button 2
	pio_handler_set(BUT2_PIO, BUT2_ID, BUT2_MASK, PIO_IT_FALL_EDGE, but_callback2);
	pio_enable_interrupt(BUT2_PIO, BUT1_MASK);
	pio_get_interrupt_status(BUT2_PIO);
	NVIC_EnableIRQ(BUT2_ID);
	NVIC_SetPriority(BUT2_ID, 4);
	// Adding Interruptions - Button 3
	pio_handler_set(BUT3_PIO, BUT3_ID, BUT3_MASK, PIO_IT_FALL_EDGE, but_callback2);
	pio_enable_interrupt(BUT3_PIO, BUT3_MASK);
	pio_get_interrupt_status(BUT3_PIO);
	NVIC_EnableIRQ(BUT3_ID);
	NVIC_SetPriority(BUT3_ID, 4);
}



int main(void)
{
	init();

	while (1){
		pisca_led(10, frequencia,2);
		if (but_flag1){
			// Add Button
			frequencia += 100;
			but_flag1 = 0;
		}
		
		if (but_flag2){
			// Stop Button
			frequencia = 0;
			but_flag2 = 0;
			
		}
		
		if (but_flag3){
			// Sub Button
			frequencia -= 50;
			but_flag3 = 0;
		}
	}
	return 0;
}
