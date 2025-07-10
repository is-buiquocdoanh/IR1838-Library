/**
  ******************************************************************************
  * @file		IR1838.h
  * @author		doanh762003@gmail.com
  * @date		10/7/2025
  ******************************************************************************
  */
#ifndef __IR1838_H
#define __IR1838_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// NEC Protocol timing (us)
#define NEC_START_LOW    9000
#define NEC_START_HIGH   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE    1690
#define NEC_ZERO_SPACE   560

// Public variables
extern uint32_t IR_Code;
extern uint8_t  IR_Ready;

// API
void IR1838_Init(TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
const char* IR1838_Decode(uint32_t code);
void IR1838_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __IR1838_H */
