/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "i2c.h"
#include "config.h"










I2C_HandleTypeDef hi2c[I2C_MAX];
I2C_TypeDef *i2cDef[I2C_MAX]={I2C1, I2C2};
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
    GPIO_InitTypeDef init = {0};
    
    switch((u32)i2cHandle->Instance) {
        case (u32)I2C1:
        {
            __HAL_RCC_I2C1_CLK_ENABLE();
            __HAL_RCC_GPIOB_CLK_ENABLE();

            init.Pin = GPIO_PIN_6|GPIO_PIN_7;
            init.Mode = GPIO_MODE_AF_OD;
            init.Pull = GPIO_PULLUP;
            init.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOB, &init);
        }
        break;
    }
}



int i2c_init(eI2C i2c, I2C_InitTypeDef *init)
{
    
    hi2c[i2c].Instance = i2cDef[i2c];
    hi2c[i2c].Init = *init;
    hi2c[i2c].Mode = HAL_I2C_MODE_MASTER;
    return HAL_I2C_Init(&hi2c[i2c]);
}


int i2c_deinit(eI2C i2c)
{
    return HAL_I2C_DeInit(&hi2c[i2c]);
}


int i2c_read(eI2C i2c, u16 devAddr, u8 *data, u16 len)
{
    return HAL_I2C_Master_Receive(&hi2c[i2c], devAddr, data, len, 0xFFFF);
}


int i2c_write(eI2C i2c, u16 devAddr, u8 *data, u16 len)
{
    return HAL_I2C_Master_Transmit(&hi2c[i2c], devAddr, data, len, 0xFFFF);
}


I2C_HandleTypeDef *i2c_get_handle(eI2C i2c)
{
    return &hi2c[i2c];
}





