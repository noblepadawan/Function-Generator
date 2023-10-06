

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
uint16_t triangle_table[TABLE_SIZE];

void SystemClock_Config(void);

#define arr 0xFFFFFFFF

WaveformMode waveform = SQUARE;
uint16_t duty_cycle = 50;
uint32_t ccr1 = 567;
uint32_t count = 0;
uint32_t dac_value;
uint16_t frequency = 1;


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
  generate_triangle_table();

  int8_t keypad_val = 0;


  while(1)
  {
	  keypad_val = keypad_read();

	  // Button debounce delay
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
					  frequency = 1;
					  break;

				  case 2:
					  // 200 Hz
					  frequency = 2;
					  break;

				  case 3:
					  // 300 Hz
					  frequency = 3;
					  break;

				  case 4:
					  // 400 Hz
					  frequency = 4;
					  break;

				  case 5:
					  // 500 Hz
					  frequency = 5;
					  break;

				  case 6:
					  // Sine wave output
					  waveform = SINE;
					  break;

				  case 7:
					  // Triangle wave output
					  waveform = TRIANGLE;
					  break;

				  case 8:
					  // Sawtooth/Ramp wave output
					  waveform = RAMP;
					  break;

				  case 9:
					  // Square wave output
					  waveform = SQUARE;
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
	TIM2->SR &= ~((TIM_SR_UIF) | (TIM_SR_CC1IF));	// Clear UIF and CC1IF flag

	switch(waveform)
	{
		case SQUARE:

			if(count < (duty_cycle * ccr1) / 100)	// Check if count is less than duty cycle percentage of CCR1
			{
				DAC_write(VOLT_MAX);
			}
			else{
				DAC_write(0);
			}
			count = (count + frequency);			// Increment count based on frequency
			if(count > (uint32_t)(ccr1))
			{
				count = 0;
			}
			break;

		case SINE:
            dac_value = sine_table[count];
            DAC_write(dac_value);
            count = (count + frequency) % TABLE_SIZE;	// Increment count based on frequency, reset if over table size
			break;

		case TRIANGLE:
            dac_value = triangle_table[count];
            DAC_write(dac_value);
            count = (count + frequency) % TABLE_SIZE;	// Increment count based on frequency, reset if over table size
            break;

		case RAMP:
            DAC_write(VOLT_MAX * count / ccr1);
            count += (frequency);					// Increment count based on frequency
            if (count > ccr1)// - 3) 					// subtracting 3 is for some instrument fine tuning
            {
                count = 0;
            }
            break;

	}
	TIM2->CCR1 += ccr1;								// Increment CCR1 by previous CCR1
}



void TIM2_init(void)
{
    // Configure Timer 2
    RCC->APB1ENR1 	|= 	RCC_APB1ENR1_TIM2EN;
    TIM2->DIER 		|= 	(TIM_DIER_UIE | TIM_DIER_CC1IE);	// enable update event and compare/capture 1 interrupt
    TIM2->SR 		&=  ~(TIM_SR_UIF | TIM_SR_CC1IF);		// clear update event and compare/capture 1 flag
    TIM2->ARR 		= 	arr - 1;
    TIM2->CCR1 		= 	ccr1 - 1;
    TIM2->CCMR1 	&=  ~(TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC1S_1);	// set output compare mode to Toggle
    TIM2->CCER 		|= 	TIM_CCER_CC1E;						// enable capture/compare channel 1 output
    TIM2->CR1 		|= 	TIM_CR1_ARPE;						// enable auto-reload preload
    TIM2->CR1 		|= 	TIM_CR1_CEN;						// start timer

    // Enable interrupt in NVIC
    NVIC->ISER[0] = (1 << (TIM2_IRQn & 0x1F));
}


void generate_sine_table(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
    	sine_table[i] = (uint16_t) (VOLT_MAX / 2.0 * sin(2 * M_PI * i / TABLE_SIZE) + VOLT_MAX / 2.0);
    }
}

void generate_triangle_table(void) {
  uint16_t half_size = TABLE_SIZE / 2;
  float slope = (float)VOLT_MAX / half_size;

  // Generate first half of the triangle wave
  for (uint16_t i = 0; i < half_size; i++) {
    triangle_table[i] = (uint16_t)(slope * i);
  }

  // Generate second half of the triangle wave
  for (uint16_t i = half_size; i < TABLE_SIZE; i++) {
    triangle_table[i] = (uint16_t)(VOLT_MAX - slope * (i - half_size));
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
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10; // Set frequency to 32 MHz
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



