/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include <asf.h>
#include <string.h>
#include "ili9341.h"
#include "lvgl.h"
#include "touch/touch.h"

/************************************************************************/
/* LCD / LVGL                                                           */
/************************************************************************/

LV_FONT_DECLARE(dseg70);
LV_FONT_DECLARE(dseg55);
LV_FONT_DECLARE(dseg35);

#define LV_HOR_RES_MAX          (320)
#define LV_VER_RES_MAX          (240)

/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
static lv_indev_drv_t indev_drv;

volatile lv_obj_t *labelpower;
volatile lv_obj_t * labelFloor;
volatile lv_obj_t * labelFloor2;
volatile lv_obj_t * labelFloor3;
volatile lv_obj_t * labelFloor4;
volatile int minutes = 0;
volatile int hours = 0;

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_LCD_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}


static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
/************************************************************************/
/* lvgl                                                                 */
/************************************************************************/

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
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

void RTT_Handler(void) {
	uint32_t ul_status;
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		minutes++;
		if (minutes >= 60){
			minutes = 0;
			hours ++;
		}
		printf("%d", minutes);
		RTT_init(4, 4, RTT_MR_ALMIEN);
	}
}



static void event_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		printf("Clicou");
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void up_handler(lv_event_t *e){
	lv_event_code_t code = lv_event_get_code(e);
	char *c;
	int temp;
	if (code == LV_EVENT_CLICKED) {
		c = lv_label_get_text(labelFloor2);
		temp = atoi(c);
		lv_label_set_text_fmt(labelFloor2, "%02d", temp + 1);
	}
}

static void down_handler(lv_event_t *e){
	lv_event_code_t code = lv_event_get_code(e);
	char *c;
	int temp;
	if (code == LV_EVENT_CLICKED) {
		c = lv_label_get_text(labelFloor2);
		temp = atoi(c);
		lv_label_set_text_fmt(labelFloor2, "%02d", temp - 1);
	}
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

void lv_termostato(void){
	//Criando Termo1
	labelFloor = lv_label_create(lv_scr_act());
	lv_obj_align(labelFloor, LV_ALIGN_LEFT_MID, 35 , -45);
	lv_obj_set_style_text_font(labelFloor, &dseg70, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelFloor, lv_color_white(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelFloor, "%02d", 23);
	
	//Criando Termo2 | labelSetValue
	labelFloor2 = lv_label_create(lv_scr_act());
	lv_obj_align(labelFloor2, LV_ALIGN_RIGHT_MID, 0 , -45);
	lv_obj_set_style_text_font(labelFloor2, &dseg55, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelFloor2, lv_color_white(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelFloor2, "%02d", 22);
	
	//Criando minutos
	labelFloor3 = lv_label_create(lv_scr_act());
	lv_obj_align(labelFloor3, LV_ALIGN_TOP_RIGHT, 0 , 0);
	lv_obj_set_style_text_font(labelFloor3, &dseg35, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelFloor3, lv_color_white(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelFloor3, "%02d", 49);
	
	//Criando horas
	labelFloor4 = lv_label_create(lv_scr_act());
	lv_obj_align_to(labelFloor4, labelFloor3, LV_ALIGN_LEFT_MID,-55,-3);
	lv_obj_set_style_text_font(labelFloor4, &dseg35, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelFloor4, lv_color_white(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelFloor4, "%02d", 17);
	
	//Criando Estilo
	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_black());
	lv_style_set_border_color(&style, lv_color_black());
	lv_style_set_border_width(&style, 5);
	
	//Power Button
	lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
	
	lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, 0, -40);
	labelpower = lv_label_create(btn1);
	lv_label_set_text(labelpower, "[  " LV_SYMBOL_POWER);
	lv_obj_center(labelpower);
	lv_obj_add_style(btn1, &style, 0);
	lv_obj_set_width(btn1, 60);
	lv_obj_set_height(btn1, 60);
	
	//M Button
	lv_obj_t *labelBtn2;
	lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
	
	lv_obj_align_to(btn2, btn1, LV_ALIGN_RIGHT_MID, 40, -10);
	labelBtn2 = lv_label_create(btn2);
	lv_label_set_text(labelBtn2, "|   M   |");
	lv_obj_center(labelBtn2);
	lv_obj_add_style(btn2, &style, 0);
	
	//Clock Button
	lv_obj_t *labelBtn3;
	lv_obj_t *btn3 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn3, event_handler, LV_EVENT_ALL, NULL);
	
	lv_obj_align_to(btn3, btn2, LV_ALIGN_RIGHT_MID, 40, -13);
	labelBtn3 = lv_label_create(btn3);
	lv_label_set_text(labelBtn3, LV_SYMBOL_REFRESH "  ]");
	lv_obj_center(labelBtn3);
	lv_obj_add_style(btn3, &style, 0);
	
	//Up Arrow Button
	lv_obj_t *labelBtn4;
	lv_obj_t *btn4 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn4, up_handler, LV_EVENT_ALL, NULL);
	
	lv_obj_align(btn4, LV_ALIGN_BOTTOM_RIGHT, 0, -37);
	labelBtn4 = lv_label_create(btn4);
	lv_label_set_text(labelBtn4, "  ^  ]");
	lv_obj_center(labelBtn4);
	lv_obj_add_style(btn4, &style, 0);
	lv_obj_set_width(btn4, 60);
	lv_obj_set_height(btn4, 60);
	//Down Arrow Button
	lv_obj_t *labelBtn5;
	lv_obj_t *btn5 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn5, down_handler, LV_EVENT_ALL, NULL);
	
	lv_obj_align_to(btn5, btn4, LV_ALIGN_LEFT_MID, -70, -23);
	labelBtn5 = lv_label_create(btn5);
	lv_label_set_text(labelBtn5, "[  v  |");
	lv_obj_center(labelBtn5);
	lv_obj_add_style(btn5, &style, 0);
	lv_obj_set_width(btn5, 60);
	lv_obj_set_height(btn5, 60);
}

static void task_lcd(void *pvParameters) {
	int px, py;

	lv_termostato();

	for (;;)  {
		lv_tick_inc(50);
		lv_task_handler();
		vTaskDelay(50);
	}
}

static void task_rtt(void *pvParameters){
	for(;;){
		lv_label_set_text_fmt(labelFloor4, "%02d", hours);
		lv_label_set_text_fmt(labelFloor3, "%02d", minutes);
		vTaskDelay(1000);
	}
}

/************************************************************************/
/* configs                                                              */
/************************************************************************/

static void configure_lcd(void) {
	/**LCD pin configure on SPI*/
	pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);  //
	pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
	pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
	pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
	pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
	pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
	
	ili9341_init();
	ili9341_backlight_on();
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
	};

	/* Configure console UART. */
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

/************************************************************************/
/* port lvgl                                                            */
/************************************************************************/

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
	ili9341_set_top_left_limit(area->x1, area->y1);   ili9341_set_bottom_right_limit(area->x2, area->y2);
	ili9341_copy_pixels_to_screen(color_p,  (area->x2 + 1 - area->x1) * (area->y2 + 1 - area->y1));
	
	/* IMPORTANT!!!
	* Inform the graphics library that you are ready with the flushing*/
	lv_disp_flush_ready(disp_drv);
}

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
	int px, py, pressed;
	
	if (readPoint(&px, &py))
		data->state = LV_INDEV_STATE_PRESSED;
	else
		data->state = LV_INDEV_STATE_RELEASED; 
	
	data->point.x = px;
	data->point.y = py;
}



void configure_lvgl(void) {
	lv_init();
	lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);
	
	lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;      /*Set the horizontal resolution in pixels*/
	disp_drv.ver_res = LV_VER_RES_MAX;      /*Set the vertical resolution in pixels*/

	lv_disp_t * disp;
	disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
	
	/* Init input on LVGL */
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_input_read;
	lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/
int main(void) {
	/* board and sys init */
	board_init();
	sysclk_init();
	configure_console();

	/* LCd, touch and lvgl init*/
	configure_lcd();
	configure_touch();
	configure_lvgl();
	
	RTT_init(4, 4, RTT_MR_ALMIEN);  // inicializa rtt com alarme

	/* Create task to control oled */
	if (xTaskCreate(task_lcd, "LCD", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}
	
	if (xTaskCreate(task_rtt, "rtt", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create rtt task\r\n");
	}
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){ }
}
