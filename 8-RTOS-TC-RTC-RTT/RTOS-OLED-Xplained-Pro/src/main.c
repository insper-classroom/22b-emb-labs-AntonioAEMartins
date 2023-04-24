#include <asf.h>
#include "conf_board.h"

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
#include <string.h>

/* Botao da placa */

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

#define BUT1_PIO PIOD
#define BUT1_ID ID_PIOD
#define BUT1_IDX 28
#define BUT1_MASK (1 << BUT1_IDX)

/** RTOS  */
#define TASK_OLED_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_OLED_STACK_PRIORITY            (tskIDLE_PRIORITY)
#define TASK_SENSOR_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_SENSOR_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

volatile int global_seconds;
volatile int global_minutes;
volatile int global_hours;

volatile int flag_tc_count;
volatile int flag_tc_button = 0;
	
typedef struct  {
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t week;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
} calendar;

QueueHandle_t xQueuePing;
QueueHandle_t xQueueEcho;
QueueHandle_t xQueueOled;

SemaphoreHandle_t xSemaphoreRTT;
SemaphoreHandle_t xSemaphoreTC;
SemaphoreHandle_t xSemaphoreRTC;

BaseType_t xHigherPriorityTaskWoken = pdTRUE;

/** prototypes */
void but_callback(void);
static void BUT_init(void);
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq);
void led_pisca(int led);
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type);

/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
void but_callback(void) {
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_oled(void *pvParameters) {
  gfx_mono_ssd1306_init();
  gfx_mono_draw_string("Exemplo RTOS", 0, 0, &sysfont);
  gfx_mono_draw_string("oii", 0, 20, &sysfont);

	uint32_t cont=0;
	for (;;)
	{
		char buf[3];
		
		cont++;
		
		sprintf(buf,"%03d",cont);
		gfx_mono_draw_string(buf, 0, 20, &sysfont);
				
		vTaskDelay(1000);
	}
}


static void task_printConsole(void *pvParameters) {
	
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT));
		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
}

void RTT_Handler(void){
	uint32_t ul_status;
	ul_status = rtt_get_status(RTT);
	
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS){
		xSemaphoreGiveFromISR(xSemaphoreRTT, &xHigherPriorityTaskWoken);
		RTT_init(4, 16, RTT_MR_ALMIEN);
	}
}

void TC7_Handler(void) {
	/**
	* Devemos indicar ao TC que a interrup��o foi satisfeita.
	* Isso � realizado pela leitura do status do perif�rico
	**/
	volatile uint32_t status = tc_get_status(TC2, 1);

	/** Muda o estado do LED (pisca) **/
	xSemaphoreGiveFromISR(xSemaphoreTC, &xHigherPriorityTaskWoken);
}

void TC4_Handler(void) {
	/**
	* Devemos indicar ao TC que a interrup��o foi satisfeita.
	* Isso � realizado pela leitura do status do perif�rico
	**/
	volatile uint32_t status = tc_get_status(TC1, 1);

	/** Muda o estado do LED (pisca) **/
	if (flag_tc_button == 1){
		flag_tc_count ++;
		if (flag_tc_count > 20){
			flag_tc_button = 0;
			flag_tc_count = 0;
			xSemaphoreGiveFromISR(xSemaphoreRTC, &xHigherPriorityTaskWoken);
		}
	}
} 

void RTC_Handler(void) {
	uint32_t ul_status = rtc_get_status(RTC);
	
	/* seccond tick */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		// o c�digo para irq de segundo vem aqui
	}
	
	/* Time or date alarm */
	if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
		xSemaphoreGiveFromISR(xSemaphoreRTC, &xHigherPriorityTaskWoken);
		// o c�digo para irq de alame vem aqui
	}

	rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Configura o PMC */
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  freq hz e interrup�c�o no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	
	/** ATIVA PMC PCK6 TIMER_CLOCK1  */
	if(ul_tcclks == 0 )
	pmc_enable_pck(PMC_PCK_6);
	
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura NVIC*/
	NVIC_SetPriority(ID_TC, 4);
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type) {
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.second);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 4);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}

void led_pisca(int led){
	if (led == 1){
		pio_clear(LED1_PIO, LED1_MASK);
		vTaskDelay(50);
		pio_set(LED1_PIO, LED1_MASK);
	}
	if (led == 2){
		pio_clear(LED2_PIO, LED2_MASK);
		vTaskDelay(50);
		pio_set(LED2_PIO, LED2_MASK);
	}
	if (led == 3){
		pio_clear(LED3_PIO, LED3_MASK);
		vTaskDelay(50);
		pio_set(LED3_PIO, LED3_MASK);
	}
}

static void task_oled(void *pvParameters){
	gfx_mono_ssd1306_init();
	char char_minutes[3] = "";
	char char_seconds[3] = "";
	char char_hours[12] = "";
	
	for(;;){
		rtc_get_time(RTC, &global_hours, &global_minutes, &global_seconds);
		sprintf(char_minutes, "%d", global_minutes);
		sprintf(char_seconds, "%d", global_seconds);
		sprintf(char_hours, "%d", global_hours);
		strcat(char_hours, ":");
		strcat(char_hours, char_minutes);
		strcat(char_hours, ":");
		strcat(char_hours, char_seconds);
		gfx_mono_draw_string(char_hours, 0, 0, &sysfont);
		vTaskDelay(500);
	}
}

static void task_sensor(void *pvParameters){
	for (;;)  {
		vTaskDelay(1000);
	}
}



void RTC_Handler(void) {
	uint32_t ul_status = rtc_get_status(RTC);
	
	/* seccond tick */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		// o c�digo para irq de segundo vem aqui
	}
	
	/* Time or date alarm */
	if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
		xSemaphoreGiveFromISR(xSemaphoreRTC, &xHigherPriorityTaskWoken);
		// o c�digo para irq de alame vem aqui
	}

	rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Configura o PMC */
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  freq hz e interrup�c�o no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	
	/** ATIVA PMC PCK6 TIMER_CLOCK1  */
	if(ul_tcclks == 0 )
	pmc_enable_pck(PMC_PCK_6);
	
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura NVIC*/
	NVIC_SetPriority(ID_TC, 4);
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type) {
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.second);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 4);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}

void led_pisca(int led){
	if (led == 1){
		pio_clear(LED1_PIO, LED1_MASK);
		vTaskDelay(50);
		pio_set(LED1_PIO, LED1_MASK);
	}
	if (led == 2){
		pio_clear(LED2_PIO, LED2_MASK);
		vTaskDelay(50);
		pio_set(LED2_PIO, LED2_MASK);
	}
	if (led == 3){
		pio_clear(LED3_PIO, LED3_MASK);
		vTaskDelay(50);
		pio_set(LED3_PIO, LED3_MASK);
	}
}

static void task_oled(void *pvParameters){
	gfx_mono_ssd1306_init();
	char char_minutes[3] = "";
	char char_seconds[3] = "";
	char char_hours[12] = "";
	
	for(;;){
		rtc_get_time(RTC, &global_hours, &global_minutes, &global_seconds);
		sprintf(char_minutes, "%d", global_minutes);
		sprintf(char_seconds, "%d", global_seconds);
		sprintf(char_hours, "%d", global_hours);
		strcat(char_hours, ":");
		strcat(char_hours, char_minutes);
		strcat(char_hours, ":");
		strcat(char_hours, char_seconds);
		gfx_mono_draw_string(char_hours, 0, 0, &sysfont);
		pmc_sleep(SLEEPMGR_SLEEP_WFI);
	}
}

static void task_sensor(void *pvParameters){
	for (;;)  {
		pmc_sleep(SLEEPMGR_SLEEP_WFI);
	}
}

static void task_rtt(void *pvParameters){
	for(;;){
		if (xSemaphoreTake(xSemaphoreRTT, 100)){
			led_pisca(2);
			RTT_init(4, 16, RTT_MR_ALMIEN);
		}
		pmc_sleep(SLEEPMGR_SLEEP_WFI);
	}
}

static void task_tc(void *pvParameters){
	for(;;){
		if (xSemaphoreTake(xSemaphoreTC, 100)){
			led_pisca(1);
		}
		pmc_sleep(SLEEPMGR_SLEEP_WFI);
	}
}	

static void task_rtc(void *pvParameters){
	for(;;){
		if (xSemaphoreTake(xSemaphoreRTC, 100)){
			led_pisca(3);
		}
	}
		pmc_sleep(SLEEPMGR_SLEEP_WFI);
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

void led_init(){
	pmc_enable_periph_clk(LED1_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_MASK, PIO_DEFAULT | PIO_DEBOUNCE);
	
	pmc_enable_periph_clk(LED2_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_MASK, PIO_DEFAULT | PIO_DEBOUNCE);
	
	pmc_enable_periph_clk(LED3_ID);
	pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_MASK, PIO_DEFAULT | PIO_DEBOUNCE);
	
	pio_set(LED1_PIO, LED1_MASK);
	pio_set(LED2_PIO, LED2_MASK);
	pio_set(LED3_PIO, LED3_MASK);
}

void but_callback1(void){
	flag_tc_button = 1;
	flag_tc_count = 0;
}

void button_init(void){
	pmc_enable_periph_clk(BUT1_ID);					
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_handler_set(BUT1_PIO, BUT1_ID, BUT1_MASK, PIO_IT_FALL_EDGE, &but_callback1);
	pio_enable_interrupt(BUT1_PIO, BUT1_MASK);
	pio_get_interrupt_status(BUT1_PIO);
	NVIC_EnableIRQ(BUT1_ID);
	NVIC_SetPriority(BUT1_ID, 4);
	
}

int main(void) {
	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	led_init();
	button_init();
	/* Initialize the console uart */
	configure_console();
	
	WDT->WDT_MR = WDT_MR_WDDIS;  
	
	xSemaphoreRTT = xSemaphoreCreateBinary();
	xSemaphoreRTC = xSemaphoreCreateBinary();
	xSemaphoreTC = xSemaphoreCreateBinary();
	
	/* Create task to control oled */
	if (xTaskCreate(task_oled, "oled", TASK_OLED_STACK_SIZE, NULL, TASK_OLED_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create oled task\r\n");
	}
	
	if (xTaskCreate(task_sensor, "sensor", TASK_SENSOR_STACK_SIZE, NULL, TASK_SENSOR_STACK_PRIORITY, NULL) != pdPASS){
		printf("Failed to create sensor task\r\n");
	}
	
	if (xTaskCreate(task_rtt, "rtt", TASK_SENSOR_STACK_SIZE, NULL, TASK_SENSOR_STACK_PRIORITY, NULL) != pdPASS){
		printf("Failed to create rtt task\r\n");
	}
	
	if (xTaskCreate(task_tc, "tc", TASK_SENSOR_STACK_SIZE, NULL, TASK_SENSOR_STACK_PRIORITY, NULL) != pdPASS){
		printf("Failed to create tc task\r\n");
	}
	
	if (xTaskCreate(task_rtc, "rtc", TASK_SENSOR_STACK_SIZE, NULL, TASK_SENSOR_STACK_PRIORITY, NULL) != pdPASS){
		printf("Failed to create rtc task\r\n");
	}
	
	if (xSemaphoreRTT == NULL){
		printf("Failed to create semaphore RTT\n");
	}
	if (xSemaphoreTC == NULL){
		printf("Failed to create semaphore TC\n");
	}
	if (xSemaphoreRTC == NULL){

		printf("Failed to create semaphore RTC\n");
	}

	TC_init(TC2, ID_TC7, 1, 4);
	tc_start(TC2, 1);
	
	TC_init(TC1, ID_TC4, 1, 1);
	tc_start(TC1, 1);
	
	RTT_init(4, 16, RTT_MR_ALMIEN);
	calendar rtc_initial = {2023, 4, 24, 17, 16, 15 ,1};
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_SECEN);
	/* Start the scheduler. */
	vTaskStartScheduler();
	/* RTOS n�o deve chegar aqui !! */
	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
