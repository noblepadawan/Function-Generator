
#include "keypad.h"
#include <stdint.h>
#include <stdlib.h>



void keypad_init()
{

	// columns Set Up
	// clear GPIOC PB10-PB13
	GPIOC->MODER		&=	~(GPIO_MODER_MODE10
		  	  	  	  	  	  | GPIO_MODER_MODE11
							  | GPIO_MODER_MODE12
							  | GPIO_MODER_MODE13);

	// set MODE = 01 (output)
	GPIOC->MODER		|=	( (1 << GPIO_MODER_MODE10_Pos)
		  	  	  	  	  	  | (1 << GPIO_MODER_MODE11_Pos)
							  | (1 << GPIO_MODER_MODE12_Pos)
							  | (1 << GPIO_MODER_MODE13_Pos));

	// set to push-pull output
	GPIOC->OTYPER		&=	~(GPIO_OTYPER_OT10
		  	  	  	  	  	  | GPIO_OTYPER_OT11
							  | GPIO_OTYPER_OT12
							  | GPIO_OTYPER_OT13);

	// no pull-up or pull-down resistors
	GPIOC->PUPDR		&=	~(GPIO_PUPDR_PUPD10
		  	  	  	  	  	  | GPIO_PUPDR_PUPD11
							  | GPIO_PUPDR_PUPD12
							  | GPIO_PUPDR_PUPD13);

	// low speed
	GPIOC->OSPEEDR		&=	~(GPIO_OSPEEDR_OSPEED10
		  	  	  	  	  	  | GPIO_OSPEEDR_OSPEED11
							  | GPIO_OSPEEDR_OSPEED12
							  | GPIO_OSPEEDR_OSPEED13);

	// set GPIOAumns to high
	GPIOC->ODR	|= (GPIO_ODR_OD10 | GPIO_ODR_OD11 | GPIO_ODR_OD12| GPIO_ODR_OD13);

	// Rows Set Up
	// clear GPIOC PB0-PB3 and set to input
	GPIOC->MODER		&=	~(GPIO_MODER_MODE0
		  	  	  	  	  	  | GPIO_MODER_MODE1
							  | GPIO_MODER_MODE2
							  | GPIO_MODER_MODE3);
	// set MODE = 01 (output)
	GPIOC->MODER		|=	( (0 << GPIO_MODER_MODE0_Pos)
							  | (0 << GPIO_MODER_MODE1_Pos)
							  | (0 << GPIO_MODER_MODE2_Pos)
							  | (0 << GPIO_MODER_MODE3_Pos));
	// clear pupdr
	GPIOC->PUPDR		&=	~(GPIO_PUPDR_PUPD0_1
		  	  	  	  	  	  | GPIO_PUPDR_PUPD1_1
							  | GPIO_PUPDR_PUPD2_1
							  | GPIO_PUPDR_PUPD3_1);
	// set pupdr to pull-down
	GPIOC->PUPDR		|=	(2 << GPIO_PUPDR_PUPD0_Pos
		  	  	  	  	  	  | 2 << GPIO_PUPDR_PUPD1_Pos
							  | 2 << GPIO_PUPDR_PUPD2_Pos
							  | 2 << GPIO_PUPDR_PUPD3_Pos);
}

uint16_t keypad_read(void)
{
	uint8_t rows;
//	uint8_t cols;
	uint16_t bits;
	COL_PORT |= COL_MASK;

	uint8_t row_debug = ROW_PORT;

 	rows = ROW_PORT & (ROW_MASK);

	if(rows != 0){
		bits = 0;
		// set output
		COL_PORT &= ~COL_MASK;
		COL_PORT |= COL_MASK_HIGH0;
		// read
		rows = ROW_PORT & ROW_MASK;
		// scan rows and return first found value
		if(rows != 0){
			if (rows == 8){
				return STAR;
			}
			for (int i = 0; i < 3; i++){
				if ((rows & 1) == 0){
					rows = rows >> 1;
				} else {
					return 1 + 3 * i;
				}

			}
		}
		// set output
		COL_PORT &= ~COL_MASK;
		COL_PORT |= COL_MASK_HIGH1;
		// read
		rows = ROW_PORT & ROW_MASK;
		// if valid, reset and return
		// scan rows and return first found value
		if(rows != 0){
			if (rows == 8){
				return 0;
			}
			for (int i = 0; i < 3; i++){
				if ((rows & 1) == 0){
					rows = rows >> 1;
				} else {
					return 2 + 3 * i;
				}

			}
		}
		// set output
		COL_PORT &= ~COL_MASK;
		COL_PORT |= COL_MASK_HIGH2;
		// read
		rows = ROW_PORT & ROW_MASK;
		// add and shift
		// scan rows and return first found value
		if(rows != 0){
			if (rows == 8){
				return POUND;
			}
			for (int i = 0; i < 3; i++){
				if ((rows & 1) == 0){
					rows = rows >> 1;
				} else {
					return 3 + 3 * i;
				}
			}
		}
	}
	return NOKEY;
}














