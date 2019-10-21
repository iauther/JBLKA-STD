/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： i2c.c
  * @作者       ： strongerHuang
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年08月07日
  * @摘要       ： I2C驱动源文件
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2016-08-07 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
#include "i2c2.h"
#include "delay.h"


/************************************************
函数名称 ： I2C_Delay
功    能 ： I2C延时(非标准延时,请根据MCU速度 调节大小)
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
static void I2C_Delay(void)
{
  //uint16_t cnt = 100;
  //while(cnt--);
    delay_us(10);
}

/************************************************
函数名称 ： I2C_GPIO_Configuration
功    能 ： I2C引脚配置(开漏输出)
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void I2C_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = PIN_I2C_SCL | PIN_I2C_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(PORT_I2C_SCL, &GPIO_InitStructure);
}

/************************************************
函数名称 ： I2C_Initializes
功    能 ： I2C初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void I2C_Initializes(void)
{
  I2C_GPIO_Configuration();

  I2C_SCL_HIGH;                                  //置位状态
  I2C_SDA_HIGH;
}

/************************************************
函数名称 ： I2C_SDA_SetOutput
功    能 ： I2C_SDA设置为输出
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void I2C_SDA_SetOutput(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = PIN_I2C_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);
}

/************************************************
函数名称 ： I2C_SDA_SetInput
功    能 ： I2C_SDA设置为输入
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void I2C_SDA_SetInput(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = PIN_I2C_SDA;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(PORT_I2C_SDA, &GPIO_InitStructure);
}

/************************************************
函数名称 ： I2C_Start
功    能 ： I2C开始
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void I2C_Start(void)
{
  I2C_SCL_HIGH;                                  //SCL高
  I2C_Delay();

  I2C_SDA_HIGH;                                  //SDA高 -> 低
  I2C_Delay();
  I2C_SDA_LOW;                                   //SDA低
  I2C_Delay();

  I2C_SCL_LOW;                                   //SCL低(待写地址/数据)
  I2C_Delay();
}

/************************************************
函数名称 ： I2C_Stop
功    能 ： I2C停止
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void I2C_Stop(void)
{
  I2C_SDA_LOW;                                   //SDA低 -> 高
  I2C_Delay();

  I2C_SCL_HIGH;                                  //SCL高
  I2C_Delay();

  I2C_SDA_HIGH;                                  //SDA高
  I2C_Delay();
}

/************************************************
函数名称 ： I2C_PutAck
功    能 ： I2C主机产生应答(或非应答)位
参    数 ： I2C_ACK ----- 应答
            I2C_NOACK --- 非应答
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void I2C_PutAck(uint8_t Ack)
{
  I2C_SCL_LOW;                                   //SCL低
  I2C_Delay();

  if(I2C_ACK == Ack)
    I2C_SDA_LOW;                                 //应答
  else
    I2C_SDA_HIGH;                                //非应答
  I2C_Delay();

  I2C_SCL_HIGH;                                  //SCL高 -> 低
  I2C_Delay();
  I2C_SCL_LOW;                                   //SCL低
  I2C_Delay();
}

/************************************************
函数名称 ： I2C_GetAck
功    能 ： I2C主机读取应答(或非应答)位
参    数 ： 无
返 回 值 ： I2C_ACK ----- 应答
            I2C_NOACK --- 非应答
作    者 ： strongerHuang
*************************************************/
uint8_t I2C_GetAck(void)
{
  uint8_t ack;

  I2C_SCL_LOW;                                   //SCL低 -> 高
  I2C_Delay();

  I2C_SDA_SetInput();                            //SDA配置为输入模式(开漏模式可以不用切换方向)

  I2C_SCL_HIGH;                                  //SCL高(读取应答位)
  I2C_Delay();

  if(I2C_SDA_READ)
    ack = I2C_NOACK;                             //非应答
  else
    ack = I2C_ACK;                               //应答

  I2C_SCL_LOW;                                   //SCL低
  I2C_Delay();

  I2C_SDA_SetOutput();                           //SDA配置为输出模式

  return ack;                                    //返回应答位
}

/************************************************
函数名称 ： I2C_WriteByte
功    能 ： I2C写一字节
参    数 ： Data -------- 数据
返 回 值 ： I2C_ACK ----- 应答
            I2C_NOACK --- 非应答
作    者 ： strongerHuang
*************************************************/
uint8_t I2C_WriteByte(uint8_t Data)
{
  uint8_t cnt;

  for(cnt=0; cnt<8; cnt++)
  {
    I2C_SCL_LOW;                                 //SCL低(SCL为低电平时变化SDA有效)
    I2C_Delay();

    if(Data & 0x80)
      I2C_SDA_HIGH;                              //SDA高
    else
      I2C_SDA_LOW;                               //SDA低
    Data <<= 1;
    I2C_Delay();

    I2C_SCL_HIGH;                                //SCL高(发送数据)
    I2C_Delay();
  }
  I2C_SCL_LOW;                                   //SCL低(等待应答信号)
  I2C_Delay();

  return I2C_GetAck();                           //返回应答位
}

/************************************************
函数名称 ： I2C_ReadByte
功    能 ： I2C读一字节
参    数 ： ack --------- 产生应答(或者非应答)位
返 回 值 ： data -------- 读取的一字节数据
作    者 ： strongerHuang
*************************************************/
uint8_t I2C_ReadByte(uint8_t ack)
{
  uint8_t cnt;
  uint8_t data;

  I2C_SCL_LOW;                                   //SCL低
  I2C_Delay();

  I2C_SDA_SetInput();                            //SDA配置为输入模式
  for(cnt=0; cnt<8; cnt++)
  {
    I2C_SCL_HIGH;                                //SCL高(读取数据)
    I2C_Delay();

    data <<= 1;
    if(I2C_SDA_READ)
      data |= 0x01;                              //SDA为高(数据有效)

    I2C_SCL_LOW;                                 //SCL低
    I2C_Delay();
  }
  I2C_SDA_SetOutput();                           //SDA配置为输出模式

  I2C_PutAck(ack);                               //产生应答(或者非应答)位

  return data;                                   //返回数据
}


/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/
