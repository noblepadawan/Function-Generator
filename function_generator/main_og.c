/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "keypad.h"
#include "dac.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <time.h>



void TIM2_init(void);



uint16_t sine_table[TABLE_SIZE];
uint16_t triangle_table[TABLE_SIZE] = {
	0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375,
	400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725, 750, 775,
	800, 825, 850, 875, 900, 925, 950, 975, 1000, 1025, 1050, 1075, 1100, 1125, 1150, 1175,
	1200, 1225, 1250, 1275, 1300, 1325, 1350, 1375, 1400, 1425, 1450, 1475, 1500, 1525, 1550, 1575,
	1600, 1625, 1650, 1675, 1700, 1725, 1750, 1775, 1800, 1825, 1850, 1875, 1900, 1925, 1950, 1975,
	2000, 2025, 2050, 2075, 2100, 2125, 2150, 2175, 2200, 2225, 2250, 2275, 2300, 2325, 2350, 2375,
	2400, 2425, 2450, 2475, 2500, 2525, 2550, 2575, 2600, 2625, 2650, 2675, 2700, 2725, 2750, 2775,
	2800, 2825, 2850, 2875, 2900, 2925, 2950, 2975, 3000, 3025, 3050, 3075, 3100, 3125, 3150, 3175,
	3200, 3175, 3150, 3125, 3100, 3075, 3050, 3025, 3000, 2975, 2950, 2925, 2900, 2875, 2850, 2825,
	2800, 2775, 2750, 2725, 2700, 2675, 2650, 2625, 2600, 2575, 2550, 2525, 2500, 2475, 2450, 2425,
	2400, 2375, 2350, 2325, 2300, 2275, 2250, 2225, 2200, 2175, 2150, 2125, 2100, 2075, 2050, 2025,
	2000, 1975, 1950, 1925, 1900, 1875, 1850, 1825, 1800, 1775, 1750, 1725, 1700, 1675, 1650, 1625,
	1600, 1575, 1550, 1525, 1500, 1475, 1450, 1425, 1400, 1375, 1350, 1325, 1300, 1275, 1250, 1225,
	1200, 1175, 1150, 1125, 1100, 1075, 1050, 1025, 1000, 975, 950, 925, 900, 875, 850, 825,
	800, 775, 750, 725, 700, 675, 650, 625, 600, 575, 550, 525, 500, 475, 450, 425,
	400, 375, 350, 325, 300, 275, 250, 225, 200, 175, 150, 125, 100, 75, 50, 25, 0

};

void SystemClock_Config(void);


WaveformMode waveform = SQUARE;
uint16_t duty_cycle = 50;
uint32_t arr = 567;
uint32_t count = 0;
uint32_t dac_value;
uint16_t frequency = 100;


int main(void)
{

  HAL_Init();
  SystemClock_Config();

  // Set Up Clocks

  // Enable SPI1 clock
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  // Turn on the clock for GPIOA
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // clock is meant for SPI Pins
  // Turn on the clock for GPIOC
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; // clock is meant Keypad

  keypad_init();
  DAC_init();
  TIM2_init();

  __enable_irq();

  generate_sine_table();

  int8_t keypad_val = 0;


  while(1)
  {
	  keypad_val = keypad_read();
	  uint8_t debounce = keypad_val;
	  for (uint32_t i = 0; i < DELAY_TIME; i++);
	  if(keypad_val != -1)
	  {

		  if (keypad_val == debounce)
		  {
			  switch(keypad_val)
			  {
				  case 1:
					  // 100 Hz
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  frequency = 100;

					  // Set arr to value for frequency depending on wave
					  if (waveform == SQUARE) {arr = 567;}
					  else if (waveform == SINE) {arr = 1250;}
					  else if (waveform == TRIANGLE) {arr = 1250;}
					  else if (waveform == RAMP) {arr = 3150;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 2:
					  // 200 Hz
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  frequency = 200;

					  // Set arr to value for frequency depending on wave
					  if (waveform == SQUARE) {arr = 400;}
					  else if (waveform == SINE) {arr = 625;}
					  else if (waveform == TRIANGLE) {arr = 625;}
					  else if (waveform == RAMP) {arr = 1545;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 3:
					  // 300 Hz
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  frequency = 300;

					  // Set arr to value for frequency depending on wave
					  if (waveform == SQUARE) {arr = 327;}
					  else if (waveform == SINE) {arr = 417;}
					  else if (waveform == TRIANGLE) {arr = 417;}
					  else if (waveform == RAMP) {arr = 1030;}

					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 4:
					  // 400 Hz
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  frequency = 400;

					  // Set arr to value for frequency depending on wave
					  if (waveform == SQUARE) {arr = 283;}
					  else if (waveform == SINE) {arr = 313;}
					  else if (waveform == TRIANGLE) {arr = 313;}
					  else if (waveform == RAMP) {arr = 748;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 5:
					  // 500 Hz
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  frequency = 500;

					  // Set arr to value for frequency depending on wave
					  if (waveform == SQUARE) {arr = 253;}
					  else if (waveform == SINE) {arr = 251;}
					  else if (waveform == TRIANGLE) {arr = 251;}
					  else if (waveform == RAMP) {arr = 618;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 6:
					  // Sine wave output
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  waveform = SINE;

					  // Set arr to value for wave depending on frequency
					  if (frequency == 100){arr = 1250;}
					  else if (frequency == 200) {arr = 625;}
					  else if (frequency == 300) {arr = 417;}
					  else if (frequency == 400) {arr = 313;}
					  else if (frequency == 500) {arr = 251;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 7:
					  // Triangle wave output
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  waveform = TRIANGLE;

					  // Set arr to value for wave depending on frequency
					  if (frequency == 100){arr = 1250;}
					  else if (frequency == 200) {arr = 625;}
					  else if (frequency == 300) {arr = 417;}
					  else if (frequency == 400) {arr = 313;}
					  else if (frequency == 500) {arr = 251;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 8:
					  // Sawtooth/Ramp wave output
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  waveform = RAMP;

					  // Set arr to value for wave depending on frequency
					  if (frequency == 100) {arr = 3150;}
					  else if (frequency == 200) {arr = 1545;}
					  else if (frequency == 300) {arr = 1030;}
					  else if (frequency == 400) {arr = 748;}
					  else if (frequency == 500) {arr = 618;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 9:
					  // Square wave output
//					  TIM2->DIER &= ~TIM_DIER_UIE;
					  waveform = SQUARE;

					  // Set arr to value for wave depending on frequency
					  if (frequency == 100) {arr = 567;}
					  else if (frequency == 200) {arr = 400;}
					  else if (frequency == 300) {arr = 327;}
					  else if (frequency == 400) {arr = 283;}
					  else if (frequency == 500) {arr = 253;}

					  // Disable interrupt in order to update ARR
					  TIM2->DIER &= ~TIM_DIER_UIE;
					  TIM2->ARR = arr - 1;
					  for (uint32_t i = 0; i < DELAY_TIME; i++);
					  TIM2->DIER |= TIM_DIER_UIE;

					  break;

				  case 0:
					  // Reset duty_cycle to 50 %
					  duty_cycle = 50;
					  break;

				  case 10:				// * key
					  // Decrement duty_cycle by 10 %
					  if (duty_cycle > 10)
						  duty_cycle -= 10;
					  break;

				  case 12:				// # key
					  // Increment duty_cycle by 10 %
					  if (duty_cycle < 90)
						  duty_cycle += 10;
					  break;


			  }
		  }
	  }
  }

}

// Define TIM2 ISR
void TIM2_IRQHandler(void)
{
//    TIM2->DIER &= ~TIM_DIER_UIE;
//    TIM2->ARR = arr - 1;

	switch(waveform)
	{
		case SQUARE:

			if(count < (duty_cycle * arr) / 100)
			{
				DAC_write(VOLT_MAX);
			}
			else{
				DAC_write(0);
			}
			count = (count + 1);
			if(count >= (arr))
			{
				count = 0;
			}
			break;

		case SINE:
            dac_value = sine_table[count];
            DAC_write(dac_value);
            count = (count + 1) % TABLE_SIZE;
			break;

		case TRIANGLE:

            count = (count+ 1) % TABLE_SIZE;
            dac_value = triangle_table[count];
            DAC_write(dac_value);
            break;

		case RAMP:
            count += (1 *(arr / 100));
            if (count > arr) {
                count = 0;
            }
            DAC_write(VOLT_MAX * count / arr);
            break;

	}

	TIM2->SR &= ~(TIM_SR_UIF);

//    TIM2->DIER |= TIM_DIER_UIE;


}



void TIM2_init(void)
{
    // Configure Timer 2
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->SR &= ~(TIM_SR_UIF);
    TIM2->ARR = arr - 1;
    TIM2->CR1 |= TIM_CR1_CEN;

    // Enable interrupt in NVIC
    NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));
}


void generate_sine_table(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
    	sine_table[i] = (uint16_t) (VOLT_MAX / 2.0 * sin(2 * M_PI * i / TABLE_SIZE) + VOLT_MAX / 2.0);
    }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10; // before was 6
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


//// Read from keypad to form voltage
//keypad_val = keypad_read();
//
//// Delay in order to prevent bounce from filling entire buffer
//for (uint32_t i = 0; i < DELAY_TIME; i++);
//
//// Buttons such as "*", "#", "A","B", "C", "D" are the reset Buttons
//if (keypad_val >= STAR_BTN && keypad_val <= POUND_BTN)
//{
//	  voltage_buff[0] = 0;
//	  voltage_buff[1] = 0;
//	  voltage_buff[2] = 0;
//	  buffer_index = 0;
//}
//// Store the value read into the buffer or if full output voltage
//else if (keypad_val >= 0 && keypad_val < STAR_BTN)
//{
//	  // If the buffer is not full, add the input to the buffer
//	  if (buffer_index < VBUFF_SIZE)
//	  {
//		  voltage_buff[buffer_index] = keypad_val;
//		  buffer_index++;
//	  }
//	  // If the buffer is full, print its contents and reset the buffer
//	  if (buffer_index == VBUFF_SIZE)
//	  {
//		  buffer_index = 0;
//		  dac_value = DAC_volt_conv(voltage_buff);
//		  DAC_write(dac_value);
//	  }
//}
