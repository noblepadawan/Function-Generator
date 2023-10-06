 ///*
// * dac.c
// *
// *  Created on: Apr 18, 2023
// *  Authors: Luis D. Garcia and Kieran Valino
// *  Description: MCP4922 DAC Library Implementation File that has the
// * 			  functions DAC_init, DAC_write, and DAC_volt_conv, that
// * 			  all work to setup the pins, convert a voltage to a 12-bit
// * 			  value, and write the 12-bit value to the DAC
//*/

#include "dac.h"
#include "stm32l4xx_hal.h"
#include <stdint.h>

// Function to write initialize the PA4, PA5, and PA7 Pins in SPI Mode
void DAC_init(void)
{
	// GPIO Configuration of PA4 - PA7 to Alternate Function for SPI
	GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE7);
	GPIOA->MODER |= (GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1);

	// User manual requires all alternate functions to be set to 5 for SPI Usage
	GPIOA->AFR[0] |= ((SPI_AF5 << GPIO_AFRL_AFSEL5_Pos)| (SPI_AF5 << GPIO_AFRL_AFSEL7_Pos));

	// Configure SPI1 CR to set MSTR, Baudrate: fCLK/2, SPI Enable, SSI, and SSM
	SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;

	//Configure SPI CR2 to set NSSP, TXEIE, DS = 1111
	SPI1->CR2 |= (SPI_CR2_DS_3| SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0);

	SPI1->CR1 |= SPI_CR1_SPE;
}

// Function to write a 12-bit value to the DAC
void DAC_write(uint16_t packet)
{
	//SPI1->CR1 &= ~SPI_CR1_SSI;   				 // turn off CS when writing
	GPIOA->ODR &= ~GPIO_ODR_OD4;

	while (!(SPI1->SR & SPI_SR_TXE));  	 // wait until transmit buffer is empty

//	SPI1->DR = (packet & ~(DAC_CONFIG_MASK)) | (DAC_CONFIG_MOD1 << 12);
	SPI1->DR = packet | (DAC_CONFIG_MOD1 << 12);

	while ((SPI1->SR & SPI_SR_BSY));  	 // wait until transmit buffer is empty

	// set the configurations bits and send packet
	GPIOA->ODR |= GPIO_ODR_OD4;
	//SPI1->CR1 |= SPI_CR1_SSI;      				 // turn on CS when done
}
// Function to convert inputed voltage in keypad to 12-bit value
uint16_t DAC_volt_conv(uint16_t voltage)
{
    /*
         Description of Conversion Function:
   	 The 0 is 1000's place, 1 is the 100's place, and 2 is the 10's place in the buffer.
   	 The voltage inputed into keypad gets buffered and then transformed into a 12 bit
   	 value that the DAC is able to process and output as a voltage.
    */

    uint16_t first = voltage / 100;
    uint16_t second = (voltage % 100) / 10;
    uint16_t third = voltage % 10;
	uint16_t volt_val_12_bits = first * 1000 +
   		                 	second * 100 +
   							 third * 10;

    // When the voltage inputed is higher than reference voltage, output the reference voltage
    if (volt_val_12_bits > VREF)
   	 volt_val_12_bits = VREF;


    // Utilize Determined Calibrated Equation to Account for Non-Ideal Equation
    volt_val_12_bits = CALIBRATED_MULT * volt_val_12_bits - CALIBRATED_OFFSET;


    return volt_val_12_bits;
}

