/**
  ******************************************************************************
  * @file    usart.h
  * $Author: wdluo $
  * $Revision: 67 $
  * $Date:: 2012-08-15 19:00:29 +0800 #$
  * @brief   串口操作相关函数声明.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USART_H
#define _USART_H
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stm32f10x.h>

typedef int (*datain_func)(u8 *data, u16 len);

int usart4_init(datain_func fn);
int usart4_write(u8 *data, u16 len);
int usart4_read(u8 *data, u16 len);
void uart4_rx_callback(void);

#endif /*_usart_H*/

/*********************************END OF FILE**********************************/
