/**
  ******************************************************************************
  * File Name          : I2C.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __i2c_H
#define __i2c_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal_i2c.h"
#include "types.h"

typedef enum {
    I2C_1,
    I2C_2,
    
    I2C_MAX
}eI2C;

int i2c_init(eI2C i2c, I2C_InitTypeDef *init);

int i2c_deinit(eI2C i2c);

int i2c_read(eI2C i2c, u16 devAddr, u8 *data, u16 len);

int i2c_write(eI2C i2c, u16 devAddr, u8 *data, u16 len);

I2C_HandleTypeDef *i2c_get_handle(eI2C i2c);

#ifdef __cplusplus
}
#endif

#endif /*__ i2c_H */

