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

typedef enum {
    UART_1,
    UART_2,
    UART_3,
    UART_4,
    UART_5,

    UART_MAX
}eUART;

typedef void (*idle_func)(u8 *data, u16 data_len);

typedef struct {
    idle_func   fn;
    u8          *buf;
    u16         buf_len;
}uart_paras_t;




int usart_init(eUART uart, uart_paras_t *paras);
int usart_write(eUART uart, u8 *data, u16 len);
int usart_read(eUART uart, u8 *data, u16 len);
void uart_rx_callback(void);

#endif /*_usart_H*/

/*********************************END OF FILE**********************************/
