/**
  ******************************************************************************
  * @file		IR1838.c
  * @author		doanh762003@gmail.com
  * @date		10/7/2025
  ******************************************************************************
  */
#include "IR1838.h"
#include <stdio.h>

// Private variables
static TIM_HandleTypeDef *ir_htim;
static GPIO_TypeDef *ir_GPIO_Port;
static uint16_t ir_GPIO_Pin;

uint32_t IR_Code = 0;
uint8_t IR_Ready = 0;
static uint8_t bit_count = 0;

void IR1838_Init(TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    ir_htim = htim;
    ir_GPIO_Port = GPIOx;
    ir_GPIO_Pin = GPIO_Pin;
    HAL_TIM_Base_Start(ir_htim);
    __HAL_TIM_SET_COUNTER(ir_htim, 0);
}

void IR1838_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin != ir_GPIO_Pin) return;

    uint32_t pulse = __HAL_TIM_GET_COUNTER(ir_htim);
    __HAL_TIM_SET_COUNTER(ir_htim, 0);

    if (HAL_GPIO_ReadPin(ir_GPIO_Port, ir_GPIO_Pin) == GPIO_PIN_SET) {
        // HIGH: waiting for LOW pulse
        if (pulse > 8500 && pulse < 9500) {
            IR_Code = 0;
            bit_count = 0;
        }
    } else {
        // LOW: check for bit 0/1
        if (pulse > 1400 && pulse < 1800) {
            // Bit 1
            IR_Code = (IR_Code << 1) | 1;
            bit_count++;
        } else if (pulse > 400 && pulse < 700) {
            // Bit 0
            IR_Code = (IR_Code << 1);
            bit_count++;
        }

        if (bit_count >= 32) {
            IR_Ready = 1;
//            printf("IR Code: 0x%08lX\r\n", IR_Code);
            bit_count = 0;
        }
    }
}

const char* IR1838_Decode(uint32_t code) {
    switch (code) {
        case 0x00FFA25D: return "1";
        case 0x00FF629D: return "2";
        case 0x00FFE21D: return "3";
        case 0x00FF22DD: return "4";
        case 0x00FF02FD: return "5";
        case 0x00FFC23D: return "6";
        case 0x00FFE01F: return "7";
        case 0x00FFA857: return "8";
        case 0x00FF906F: return "9";
        case 0x00FF9867: return "0";
        case 0x00FF6897: return "*";
        case 0x00FFB04F: return "#";
        case 0x00FF38C7: return "OK";
        case 0x00FF18E7: return "UP";
        case 0x00FF4AB5: return "DOWN";
        case 0x00FF10EF: return "LEFT";
        case 0x00FF5AA5: return "RIGHT";
        default: return "UNKNOWN";
    }
}
