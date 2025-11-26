/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : xpt2046.c
  * @brief          : XPT2046 Touch Controller Driver
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "xpt2046.h"

/* Private variables ---------------------------------------------------------*/
static SPI_HandleTypeDef *xpt_hspi;
static GPIO_TypeDef *xpt_cs_port;
static uint16_t xpt_cs_pin;

/* Private function prototypes -----------------------------------------------*/
static void XPT2046_CS_Low(void);
static void XPT2046_CS_High(void);
static uint16_t XPT2046_ReadChannel(uint8_t cmd);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set CS pin low (select device)
  */
static void XPT2046_CS_Low(void)
{
    HAL_GPIO_WritePin(xpt_cs_port, xpt_cs_pin, GPIO_PIN_RESET);
}

/**
  * @brief  Set CS pin high (deselect device)
  */
static void XPT2046_CS_High(void)
{
    HAL_GPIO_WritePin(xpt_cs_port, xpt_cs_pin, GPIO_PIN_SET);
}

/**
  * @brief  Read a channel from XPT2046
  * @param  cmd: Command byte (channel select)
  * @retval 12-bit ADC value
  */
static uint16_t XPT2046_ReadChannel(uint8_t cmd)
{
    uint8_t tx_data[3] = {cmd, 0x00, 0x00};
    uint8_t rx_data[3] = {0};
    uint16_t result;

    XPT2046_CS_Low();
    
    /* Small delay after CS goes low */
    for(volatile int i = 0; i < 10; i++);
    
    HAL_SPI_TransmitReceive(xpt_hspi, tx_data, rx_data, 3, HAL_MAX_DELAY);
    
    XPT2046_CS_High();
    
    /* Extract 12-bit result from response
     * Response format: 
     * Byte 0: Don't care (during command transmission)
     * Byte 1: Bit 7-0 = D11-D4
     * Byte 2: Bit 7-4 = D3-D0, Bit 3-0 = Don't care
     */
    result = ((rx_data[1] << 8) | rx_data[2]) >> 3;
    result &= 0x0FFF; /* Mask to 12 bits */
    
    return result;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize XPT2046 touch controller
  */
void XPT2046_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    xpt_hspi = hspi;
    xpt_cs_port = cs_port;
    xpt_cs_pin = cs_pin;
    
    /* Make sure CS is high (device deselected) */
    XPT2046_CS_High();
    
    /* Perform a dummy read to initialize the controller */
    XPT2046_ReadChannel(XPT2046_CMD_READ_X);
}

/**
  * @brief  Read raw X coordinate
  */
uint16_t XPT2046_ReadX(void)
{
    return XPT2046_ReadChannel(XPT2046_CMD_READ_X);
}

/**
  * @brief  Read raw Y coordinate
  */
uint16_t XPT2046_ReadY(void)
{
    return XPT2046_ReadChannel(XPT2046_CMD_READ_Y);
}

/**
  * @brief  Check if screen is being touched (using pressure detection)
  */
uint8_t XPT2046_IsTouched(void)
{
    uint16_t z1, z2;
    
    z1 = XPT2046_ReadChannel(XPT2046_CMD_READ_Z1);
    z2 = XPT2046_ReadChannel(XPT2046_CMD_READ_Z2);
    
    /* Touch detected if Z1 > threshold and Z2 < (4095 - threshold) */
    if (z1 > XPT2046_MIN_PRESSURE && z2 < (4095 - XPT2046_MIN_PRESSURE))
    {
        return 1;
    }
    return 0;
}

/**
  * @brief  Read touch data with averaging for better accuracy
  */
uint8_t XPT2046_ReadTouch(XPT2046_TouchData *touch)
{
    uint32_t x_sum = 0, y_sum = 0;
    uint16_t z1;
    const uint8_t samples = 4;
    
    /* Read pressure first */
    z1 = XPT2046_ReadChannel(XPT2046_CMD_READ_Z1);
    
    touch->z = z1;
    
    /* Check if touched */
    if (z1 < XPT2046_MIN_PRESSURE)
    {
        touch->pressed = 0;
        touch->x = 0;
        touch->y = 0;
        return 0;
    }
    
    /* Take multiple samples and average */
    for (uint8_t i = 0; i < samples; i++)
    {
        x_sum += XPT2046_ReadChannel(XPT2046_CMD_READ_X);
        y_sum += XPT2046_ReadChannel(XPT2046_CMD_READ_Y);
    }
    
    touch->x = x_sum / samples;
    touch->y = y_sum / samples;
    touch->pressed = 1;
    
    return 1;
}
