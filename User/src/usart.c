/**
  ******************************************************************************
  * @file    usart.c
  * $Author: wdluo $
  * $Revision: 67 $
  * $Date:: 2012-08-15 19:00:29 +0800 #$
  * @brief   串口相关函数。
  *****************************************************************************/
#include "usart.h"
#include "tmr.h"
#include "usb_desc.h"

#define UART_RX_BUF_LEN  100
u16 uart4_rx_len=0;
u8 UART_RX_BUF[UART_RX_BUF_LEN];
datain_func uart_fn=NULL;


static void uart_rcc_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
}


static void uart_io_config(void)
{
	GPIO_InitTypeDef init;
	init.GPIO_Mode = GPIO_Mode_AF_PP;
	init.GPIO_Pin = GPIO_Pin_10;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &init);

	init.GPIO_Mode = GPIO_Mode_IPU;
	init.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &init);
}



static void usart4_dma_init()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器
    
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//开启空闲中断
    USART_DMACmd(UART4, USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收
    USART_Cmd(UART4, ENABLE);

    DMA_DeInit(DMA2_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR; //DMA外设usart基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)UART_RX_BUF;  //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
    DMA_InitStructure.DMA_BufferSize = UART_RX_BUF_LEN;  //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道

    DMA_Cmd(DMA2_Channel3, ENABLE);  //正式驱动DMA传输
}
int usart4_init(datain_func fn)
{
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	uart_rcc_config();
	uart_io_config();

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(UART4, &USART_InitStruct);

  	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(UART4, USART_IT_RXNE,ENABLE);//使能接收中断
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_Cmd(UART4, ENABLE);//使能串口
    
    usart4_dma_init();

    uart_fn = fn;

    return 0;
}




void uart4_rx_callback(void)
{
    uint32_t tmp;
    //USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
    //USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);

    //if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) {
    //    USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    //}

    if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        //USART_ClearITPendingBit(UART4, USART_IT_IDLE); 

        USART_ReceiveData(UART4);//读取数据注意：这句必须要，否则不能够清除中断标志位。
        
        DMA_Cmd(DMA2_Channel3, DISABLE);
        uart4_rx_len =UART_RX_BUF_LEN-DMA_GetCurrDataCounter(DMA2_Channel3); //算出接本帧数据长度

        if(uart_fn) {
            uart_fn(UART_RX_BUF, uart4_rx_len);
        }
        
        tmp=UART4->SR;
        tmp=UART4->DR;
        
        USART_ClearITPendingBit(UART4, USART_IT_IDLE);         //清除中断标志
        DMA_SetCurrDataCounter(DMA2_Channel3, UART_RX_BUF_LEN);
        DMA_Cmd(DMA2_Channel3, ENABLE);
    }

    //USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
    //USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}


int usart4_write(u8 *data, u16 len)
{
    while(len--) {
        USART_SendData(UART4, *data);
        while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
        data++;
    }
    return len;
}


int usart4_read(u8 *data, u16 len)
{
    int i=0;
    
    data[i] = USART_ReceiveData(UART4);
 return 0;
}


