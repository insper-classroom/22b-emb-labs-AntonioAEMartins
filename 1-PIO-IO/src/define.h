// this file is resposable for all define's used in this project

#ifndef DEFINE_H_
#define DEFINE_H_

// define's for the buttons
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

// define's for the leds

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

#endif /* DEFINE_H_ */