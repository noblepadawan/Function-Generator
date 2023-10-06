
#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DELAY_TIME 250000
#define VOLT_MAX 3900
#define TABLE_SIZE 567

void SystemClock_Config(void);
void generate_sine_table(void);
void generate_triangle_table(void);

typedef enum {
	SQUARE,
	SINE,
	TRIANGLE,
	RAMP
} WaveformMode;


/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"


/* Private includes ----------------------------------------------------------*/

void Error_Handler(void);

#ifdef __cplusplus
}
#endif



#endif /* __MAIN_H */
