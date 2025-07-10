# IR1838 STM32 Library

This library allows STM32 microcontrollers (using HAL) to receive and decode infrared signals from NEC protocol remotes via the IR1838 receiver module. It reads IR codes and maps them to button names.

---
![](https://github.com/is-buiquocdoanh/IR1838-Library/blob/main/Media/module-thu-hong-ngoai-remote-ir1838.jpg)
## 1. Introduction

### 1.1. IR Receiver IR1838
The IR1838 is an infrared receiver module commonly used to detect IR signals from TV/DVD remotes or mini 21-button remotes. It operates at a carrier frequency of **38kHz**, filters ambient noise, and outputs digital HIGH/LOW pulses to the MCU.

---

### 1.2.NEC Protocol Overview
The NEC protocol uses 38kHz modulation to transmit 32-bit frames:

| Segment         | Duration          |
|------------------|--------------------|
| Start LOW        | ~9ms               |
| Start HIGH       | ~4.5ms             |
| Bit 0            | 562.5µs LOW + 562.5µs HIGH |
| Bit 1            | 562.5µs LOW + 1.6875ms HIGH |
| Frame Structure  | 8-bit Addr + ~Addr + Cmd + ~Cmd |

Example code when button `1` is pressed: 0x00FFA25D


---

## 2. How It Works

The library works by:
- 🕑 **Timer (TIM1):** measures pulse durations between signal edges.
- 🔥 **EXTI Interrupt:** triggered on both rising and falling edges of IR signal.
- ⏱️ Processing the timing to distinguish:
  - Start Frame (9ms LOW + 4.5ms HIGH)
  - Bit 0 (~560µs)
  - Bit 1 (~1.69ms)
- 📝 When 32 bits are received, sets the flag `IR_Ready` and stores the code in `IR_Code`.

---

## 3. STM32 Configuration

### 3.1. GPIO (for IR Receiver)
| Parameter     | Value              |
|---------------|---------------------|
| Pin           | PA2                |
| Mode          | GPIO_INPUT         |
| Pull          | Pull-up            |
| EXTI          | Rising & Falling Edge Trigger |

---

### 3.2. Timer (TIM1)
The timer must provide microsecond accuracy for correct pulse width measurement.

| Parameter     | Default Value        | Notes                                 |
|---------------|----------------------|---------------------------------------|
| Clock source  | APB1 (72 MHz)        | After PLL configuration              |
| Prescaler     | 71                   | \(72\,MHz ÷ (71+1) = 1\,MHz\) (1 µs tick)|
| Period        | 65535                | Max duration ~65.5 ms                |

### 3.3. How to Calculate Prescaler

![image1](https://github.com/is-buiquocdoanh/IR1838-Library/blob/main/Media/fomula.png)


Where:
- \(f_{APB1}\): Timer clock (e.g., 72 MHz)
- \(f_{tick}\): Desired tick frequency (e.g., 1 MHz for 1 µs resolution)

#### Example

| Desired Tick | Prescaler               |
|--------------|--------------------------|
| 1 µs (1 MHz) | \((72\,MHz ÷ 1\,MHz)-1=71\)|
| 0.5 µs (2 MHz)| \((72\,MHz ÷ 2\,MHz)-1=35\)|

| Parameter           | Value           |
|---------------------|------------------|
| Prescaler           | 71 (for 1µs tick with SYSCLK 72MHz) |
| Period              | 65535            |
| Counter Mode        | Up               |

---

### 3.4. EXTI Interrupt
- Enable EXTI2 (for PA2)
- Trigger: Rising & Falling Edge
- Callback: `HAL_GPIO_EXTI_Callback()`

---

## 4. How to Use

### Add Library
1. Copy `IR1838.h` and `IR1838.c` into:
Core/Inc
Core/Src
2. Include in your project:
#include "IR1838.h"
3.  Initialization in main.c
```c
IR1838_Init(&htim1, GPIOA, GPIO_PIN_2);
```
4. Handle EXTI Interrupt
```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    IR1838_EXTI_Callback(GPIO_Pin);
}
```
5. Read and Print Button Name
Read and Print Button Name

### Example main.c
```c
#include "main.h"
#include "IR1838.h"

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_TIM1_Init();

    IR1838_Init(&htim1, GPIOA, GPIO_PIN_2);
    printf("=== IR Receiver Test Start ===\r\n");

    while (1) {
        if (IR_Ready) {
            const char* btn = IR1838_Decode(IR_Code);
            printf("Button: %s (Code: 0x%08lX)\r\n", btn, IR_Code);
            IR_Ready = 0;
        }
        HAL_Delay(100);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    IR1838_EXTI_Callback(GPIO_Pin);
}
```
### Video demo
Link video: https://www.youtube.com/watch?v=CkQcgg_We1s

## Author
- Name: BUI QUOC DOANH
- GitHub: [is-buiquocdoanh](https://github.com/is-buiquocdoanh)
- Email: doanh762003@gmail.com
