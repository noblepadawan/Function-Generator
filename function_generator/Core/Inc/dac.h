/*
 * dac.h
 *
 * Created on: Apr 18, 2023
 * Authors: Luis D. Garcia and Kieran Valino
 * Description: MCP4922 DAC Library Header File that has the
 * functions DAC_init, DAC_write, and DAC_volt_conv.
*/

#ifndef SRC_DAC_H_
#define SRC_DAC_H_

#include "stm32l4xx_hal.h"
#include <stdint.h>

#define VOLTAGE_BUF_SIZE 	3 				// Voltage Buffer Size is 3
#define DAC_CONFIG_MASK 	0xF000 			// Mask to Clear Configuration Bits
#define DAC_CONFIG_MOD1 	3 				// Mode enables 12-bit DAC to output Voltage
#define VREF 				4096 			// 3.30 V to 3300 mV
#define TOTAL_BITS 			12 				// DAC Requires 12 Bits
#define SPI_AF5 			5 				// Permits use of pins for SPI CS, POCI, SCK
#define CALIBRATED_MULT 	1.251587463 	// Calculated Multiplier for Calibration Equation
#define CALIBRATED_OFFSET 	12 				// Calculated Offset for Calibration Equation

void DAC_init(void);
void DAC_write(uint16_t dac_value);
uint16_t DAC_volt_conv(uint16_t voltage); //_buff[VOLTAGE_BUF_SIZE]);

//uint16_t DAC_volt_conv(uint8_t voltage_buff[VOLTAGE_BUF_SIZE]);



#endif /* SRC_DAC_H_ */


//#ifndef SRC_DAC_H_
//#define SRC_DAC_H_
//
//void DAC_init(void);
//void DAC_write(uint16_t);
//void DAC_volt_conv(uint16_t);
//
//
//#define SHDN 0x2000 // Bit 12
//#define GAIN 0x1000 // Bit 13
//#define BUFF 0x4000 // Bit 14
//#define SELL 0x8000 // Bit 15
//#define DAC_PORT GPIOA
//#define DAC_RES 4096
//#define VREF 3.325
//#endif /* SRC_DAC_H_ */

//#ifndef SRC_DAC_H_
//#define SRC_DAC_H_
//
//void DAC_init(void);
//void DAC_write(uint16_t);
//uint16_t DAC_volt_conv(uint16_t);
//
//
//#define SHDN 0x2000 // Bit 12
//#define GAIN 0x1000 // Bit 13
//#define BUFF 0x4000 // Bit 14
//#define SELL 0x8000 // Bit 15
//#define DAC_PORT GPIOA
//#define DAC_RES 4096
//#define VREF 3.325
//#endif /* SRC_DAC_H_ */
//
////uint16_t DAC_volt_conv(uint16_t[]);
//#define CALIBRATED_MULT 	1.251587463 	// Calculated Multiplier for Calibration Equation
//#define CALIBRATED_OFFSET 	12 				// Calculated Offset for Calibration Equation
