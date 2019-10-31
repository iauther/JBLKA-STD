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

uart_paras_t uartParas[UART_MAX]={0};
DMA_Channel_TypeDef      *dmaChannel[UART_MAX]={DMA1_Channel5, DMA1_Channel6, DMA1_Channel3, DMA2_Channel3, NULL};
USART_TypeDef      *uartDef[UART_MAX]={USART1, USART2, USART3, UART4, UART5};
IRQn_Type          uartIRQn[UART_MAX]={USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn};
IRQn_Type          dmaIRQn[UART_MAX]={DMA1_Channel5_IRQn, DMA1_Channel6_IRQn, DMA1_Channel3_IRQn, DMA2_Channel3_IRQn, NonMaskableInt_IRQn};
u32                rccUartPeriph[UART_MAX]={RCC_APB2Periph_USART1, RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4, RCC_APB1Periph_UART5};


static void rcc_config(eUART uart)
{
    if(uart == UART_1) {
        RCC_APB2PeriphClockCmd(rccUartPeriph[uart], ENABLE);
    }
    else {
        RCC_APB1PeriphClockCmd(rccUartPeriph[uart], ENABLE);
    }
}


static void io_config(eUART uart)
{
	GPIO_InitTypeDef init;

    switch(uart) {
        case UART_1:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        
        init.GPIO_Mode = GPIO_Mode_AF_PP;   //tx
        init.GPIO_Pin = GPIO_Pin_9;
        init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &init);

        init.GPIO_Mode = GPIO_Mode_IN_FLOATING; //rx
        init.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOA, &init);
        break;
        
        case UART_2:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

        init.GPIO_Mode = GPIO_Mode_AF_PP;
        init.GPIO_Pin = GPIO_Pin_2;
        init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &init);

        init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        init.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOA, &init);
        break;
        
        case UART_3:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

        init.GPIO_Mode = GPIO_Mode_AF_PP;
        init.GPIO_Pin = GPIO_Pin_10;
        init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &init);

        init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        init.GPIO_Pin = GPIO_Pin_11;
        GPIO_Init(GPIOC, &init);
        break;
        
        case UART_4:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

        init.GPIO_Mode = GPIO_Mode_AF_PP;
        init.GPIO_Pin = GPIO_Pin_10;
        init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &init);

        init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        init.GPIO_Pin = GPIO_Pin_11;
        GPIO_Init(GPIOC, &init);
        break;
        
        case UART_5:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

        init.GPIO_Mode = GPIO_Mode_AF_PP;
        init.GPIO_Pin = GPIO_Pin_12;
        init.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &init);

        init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        init.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOD, &init);
        break;

        default:
        break;
    }
}



static void dma_init(eUART uart)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    
    if(uart<UART_4) {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    }
    else if(uart==UART_4){
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    }
    else {
        return;
    }

    NVIC_InitStructure.NVIC_IRQChannel = uartIRQn[uart];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器
    
    USART_ITConfig(uartDef[uart], USART_IT_IDLE, ENABLE);//开启空闲中断
    USART_DMACmd(uartDef[uart], USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收
    USART_Cmd(uartDef[uart], ENABLE);

    DMA_DeInit(dmaChannel[uart]);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(uartDef[uart]->DR); //DMA外设usart基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uartParas[uart].buf;  //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
    DMA_InitStructure.DMA_BufferSize = uartParas[uart].buf_len;  //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
    DMA_Init(dmaChannel[uart], &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道

    DMA_Cmd(dmaChannel[uart], ENABLE);  //正式驱动DMA传输
}


int usart_init(eUART uart, uart_paras_t *paras)
{
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

    if(paras) {
        uartParas[uart] = *paras;
    }
	io_config(uart);

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(uartDef[uart], &USART_InitStruct);

  	NVIC_InitStructure.NVIC_IRQChannel = uartIRQn[uart];
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(uartDef[uart], USART_IT_RXNE,ENABLE);//使能接收中断
    USART_ITConfig(uartDef[uart], USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_Cmd(uartDef[uart], ENABLE);//使能串口
    
    dma_init(uart);

    return 0;
}




void uart_rx_callback(void)
{
    eUART i=UART_4;
    u32 tmp;
    u16 data_len;
    //USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
    //USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);

    //if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) {
    //    USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    //}

    for(i=UART_1; i<UART_MAX; i++) {
        if(USART_GetITStatus(uartDef[i], USART_IT_IDLE) != RESET) {
            //USART_ClearITPendingBit(UART4, USART_IT_IDLE); 

            USART_ReceiveData(uartDef[i]);//读取数据注意：这句必须要，否则不能够清除中断标志位。
            
            DMA_Cmd(dmaChannel[i], DISABLE);
            data_len =uartParas[i].buf_len-DMA_GetCurrDataCounter(dmaChannel[i]); //算出接本帧数据长度

            if(uartParas[i].fn) {
                uartParas[i].fn(uartParas[i].buf, data_len);
            }
            
            tmp=uartDef[i]->SR;
            tmp=uartDef[i]->DR;
            
            USART_ClearITPendingBit(uartDef[i], USART_IT_IDLE);         //清除中断标志
            DMA_SetCurrDataCounter(dmaChannel[i], uartParas[i].buf_len);
            DMA_Cmd(dmaChannel[i], ENABLE);
        }
    }

    //USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
    //USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}


int usart_write(eUART uart, u8 *data, u16 len)
{
    u16 i;
    for(i=0; i<len; i++) {
        USART_SendData(uartDef[uart], data[i]);
        while(USART_GetFlagStatus(uartDef[uart], USART_FLAG_TXE) == RESET);
    }
    return len;
}


int usart_read(eUART uart, u8 *data, u16 len)
{
    u16 i=0;
    for(i=0; i<len; i++) {
        while(USART_GetFlagStatus(uartDef[uart], USART_FLAG_RXNE) == RESET);
        data[i] = USART_ReceiveData(uartDef[uart]);
    }

    return 0;
}


