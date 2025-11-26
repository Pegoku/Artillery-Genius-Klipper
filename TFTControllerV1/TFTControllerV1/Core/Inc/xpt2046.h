/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : xpt2046.h
  * @brief          : XPT2046 Touch Controller Driver Header
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __XPT2046_H
#define __XPT2046_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdint.h>

/* XPT2046 Commands ----------------------------------------------------------*/
#define XPT2046_CMD_READ_X      0xD0  /* Read X position (12-bit, differential) */
#define XPT2046_CMD_READ_Y      0x90  /* Read Y position (12-bit, differential) */
#define XPT2046_CMD_READ_Z1     0xB0  /* Read Z1 (pressure) */
#define XPT2046_CMD_READ_Z2     0xC0  /* Read Z2 (pressure) */

/* Touch detection threshold */
#define XPT2046_MIN_PRESSURE    100   /* Minimum pressure to register a touch */

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;      /* Pressure */
    uint8_t pressed;
} XPT2046_TouchData;

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  Initialize XPT2046 touch controller
  * @param  hspi: Pointer to SPI handle
  * @param  cs_port: GPIO port for CS pin
  * @param  cs_pin: GPIO pin for CS
  * @retval None
  */
void XPT2046_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

/**
  * @brief  Read touch data from XPT2046
  * @param  touch: Pointer to touch data structure
  * @retval 1 if touch detected, 0 otherwise
  */
uint8_t XPT2046_ReadTouch(XPT2046_TouchData *touch);

/**
  * @brief  Read raw X coordinate
  * @retval Raw 12-bit X value
  */
uint16_t XPT2046_ReadX(void);

/**
  * @brief  Read raw Y coordinate
  * @retval Raw 12-bit Y value
  */
uint16_t XPT2046_ReadY(void);

/**
  * @brief  Check if screen is being touched
  * @retval 1 if touched, 0 otherwise
  */
uint8_t XPT2046_IsTouched(void);

#ifdef __cplusplus
}
#endif

#endif /* __XPT2046_H */
