
#include "main.h"

#define COL_PORT GPIOC->ODR		// output
#define ROW_PORT GPIOC->IDR		// input


#define COL_MASK (GPIO_ODR_OD10 | GPIO_ODR_OD11 | GPIO_ODR_OD12 | GPIO_ODR_OD13)
#define COL_MASK_HIGH0 (GPIO_ODR_OD10)
#define COL_MASK_HIGH1 (GPIO_ODR_OD11)
#define COL_MASK_HIGH2 (GPIO_ODR_OD12)
#define COL_MASK_HIGH3 (GPIO_ODR_OD13)
#define COL_OFFSET 4

#define COL_0 GPIO_ODR_OD10
#define COL_1 GPIO_ODR_OD11
#define COL_2 GPIO_ODR_OD12

#define ROW_MASK (GPIO_IDR_ID0 | GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3)
#define ROW_0 GPIO_IDR_ID0;
#define ROW_1 GPIO_IDR_ID1;
#define ROW_2 GPIO_IDR_ID2;
#define ROW_3 GPIO_IDR_ID3;

#define STAR 10
#define POUND 12
#define NOKEY 0xFF

void keypad_init();
uint16_t keypad_read();

