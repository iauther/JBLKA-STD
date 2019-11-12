#include "string.h"
#include "e2p.h"
#include "lock.h"
#include "queue.h"
#include "delay.h"
#include "config.h"
#ifdef APP
//#include "cmsis_os2.h"
#endif

#define E2P_ADDR            0xA0
#ifdef USE_24XX512
    #define PAGE_SIZE       128
#else   //24C02
    #define PAGE_SIZE       8
#endif


#define QUEUE_MAX           30


#define USE_HAL
//#define USE_STD
#ifdef USE_STD
    #define USE_STD_HW_I2C
    //#define USE_STD_SW_I2C
#endif


queue_t *eq=0;
#ifdef USE_HAL
#include "i2c.h"
static I2C_HandleTypeDef *i2c_handle=0;
int e2p_init(void)
{
    int r;
    I2C_InitTypeDef init;
    
    init.ClockSpeed = E2P_I2C_SPEED;
    init.DutyCycle = I2C_DUTYCYCLE_2;
    init.OwnAddress1 = 0;
    init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    init.OwnAddress2 = 0;
    init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    i2c_handle = i2c_get_handle(E2P_I2C);

    r = i2c_init(E2P_I2C, &init);
    eq = queue_init(QUEUE_MAX);
    
    return r;
}


int e2p_put(node_t *n)
{
    int r;

    lock_on(LOCK_E2P);
    r = queue_put(eq, n, 1);
    lock_off(LOCK_E2P);

    return r;
}


int e2p_get(node_t *n)
{
    int r;

    lock_on(LOCK_E2P);
    r = queue_get(eq, n);
    lock_off(LOCK_E2P);

    return r;
}


int e2p_read_byte(u32 addr, u8 *data)
{
    return HAL_I2C_Mem_Read(i2c_handle, E2P_ADDR, addr, I2C_MEMADD_SIZE_16BIT, data, 1, 0xFFFF);
}


int e2p_write_byte(u32 addr, u8 *data)
{
    int r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr, I2C_MEMADD_SIZE_16BIT, data, 1, 0xFFFF);
    delay_ms(5);
    return r;
}


int e2p_read(u32 addr, u8 *data, u16 len)
{
    return HAL_I2C_Mem_Read(i2c_handle, E2P_ADDR, addr, I2C_MEMADD_SIZE_16BIT, data, len, 0xFFFF);
}


static int write_page(u32 addr, u8 *data, u16 len)
{
    int r;
    //u8  tmp[PAGE_SIZE];
    
    r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr, I2C_MEMADD_SIZE_16BIT, data, len, 0xFFFF);
    //r = HAL_I2C_Mem_Read(i2c_handle, E2P_ADDR, addr, I2C_MEMADD_SIZE_16BIT, tmp, len, 0xFFFF);
    //if(memcmp(data, tmp, len)) {
    //    return -1;
    //}
    
    delay_ms(5);
    
    return r;
}

int e2p_write(u32 addr, u8 *data, u16 len)
{
#if 0
    int r;
    u16 i = 0;
    u16 cnt = 0;		//写入字节计数
    
    if(0 == addr%PAGE_SIZE) {   //起始地址刚好是页开始地址
        if(len <= PAGE_SIZE) {  //写入的字节数不大于一页，直接写入
            r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr, I2C_MEMADD_SIZE_16BIT, data, len, 0xFFFF);
        }
        else {  //写入的字节数大于一页
            for(i = 0;i < len/PAGE_SIZE; i++) { //先将整页循环写入
                r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr+cnt, I2C_MEMADD_SIZE_16BIT, data+cnt, PAGE_SIZE, 0xFFFF);
                cnt += PAGE_SIZE;
                delay_ms(5);
            }

            //将剩余的字节写入
            r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr+cnt, I2C_MEMADD_SIZE_16BIT, data+cnt, len-cnt, 0xFFFF);
        }
    }
    else {   //起始地址偏离页开始地址
        if(len <= (PAGE_SIZE - addr%PAGE_SIZE)) {    //在该页可以写完
            r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr, I2C_MEMADD_SIZE_16BIT, data, len, 0xFFFF);
        }
        else {  //该页写不完, 先将该页写完
            cnt += PAGE_SIZE - addr%PAGE_SIZE;
            r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr+cnt, I2C_MEMADD_SIZE_16BIT, data, cnt, 0xFFFF);
            delay_ms(5);

            addr += cnt;
            for(i=0; i<(len-cnt)/PAGE_SIZE; i++) {  //循环写整页数据
                r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr+cnt, I2C_MEMADD_SIZE_16BIT, data+cnt, PAGE_SIZE, 0xFFFF);
                delay_ms(5);
                cnt += PAGE_SIZE;
            }
            
            //将剩下的字节写入
            r = HAL_I2C_Mem_Write(i2c_handle, E2P_ADDR, addr+cnt, I2C_MEMADD_SIZE_16BIT, data+cnt, len-cnt, 0xFFFF);
        }
    }
    delay_ms(5);
#else
    int r;
    u16 pages = 0, bytes = 0, start = 0, count = 0;

    start = addr % PAGE_SIZE;
    count = PAGE_SIZE - start;
    pages =  len / PAGE_SIZE;
    bytes = len % PAGE_SIZE;
 
    if(start == 0)  {        //起始地址在页首
        if(pages == 0)  {//不足一页
            r = write_page(addr, data, bytes); 
        }
        else  {//大于等于1页
            while(pages--) {
                r = write_page(addr, data, PAGE_SIZE); 
                addr +=  PAGE_SIZE;
                data += PAGE_SIZE;
            }

            if(bytes) {
                r = write_page(addr, data, bytes); 
            }
        }
    }
    else {  //起始地址在页中
        if(pages== 0) { //不足一页
            r = write_page(addr, data, bytes); 
        }
        else {  //大于等于1页
            len -= count;
            pages =  len / PAGE_SIZE;
            bytes = len % PAGE_SIZE;    

            if(count) {
                r = write_page(addr, data, count);
                if(r) {
                    return r;
                }
                addr += count;
                data += count;
            } 

            while(pages--) {
                r = write_page(addr, data, PAGE_SIZE); 
                if(r) {
                    return r;
                }
                addr +=  PAGE_SIZE;
                data += PAGE_SIZE;  
            }

            if(bytes != 0) {
                r = write_page(addr, data, bytes); 
            }
        }
    }

#endif

    return r;
}
#endif



#ifdef USE_STD_HW_I2C
#include "stm32f10x.h"
static void gpio_config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
static void i2c_config(void)
{
    I2C_InitTypeDef I2C_InitStructure;

    I2C_DeInit(I2C1);                                                  //复位I2C
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                         //I2C模式
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                 //占空比(快速模式时)
    I2C_InitStructure.I2C_OwnAddress1 = E2P_ADDR;               //设备地址
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                        //应答
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;                      //速度
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);                                             //使能I2C
}

int e2p_init(void)
{
    gpio_config();
    i2c_config();
    return 0;
}

int e2p_read_byte(u32 Addr, u8 *Data)
{
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    /* 1.开始 */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* 2.设备地址/写 */
    I2C_Send7bitAddress(I2C1, E2P_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.数据地址 */
    I2C_SendData(I2C1, (u8)(Addr>>8));                            //数据地址(16位)
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, (u8)(Addr&0x00FF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* 4.重新开始 */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* 5.设备地址/读 */
    I2C_Send7bitAddress(I2C1, E2P_ADDR, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* 6.读一字节数据 */
    I2C_AcknowledgeConfig(I2C1, DISABLE);                              //产生非应答
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
    *Data = I2C_ReceiveData(I2C1);                                     //读取数据

    /* 7.停止 */
    I2C_GenerateSTOP(I2C1, ENABLE);

    return 0;
}

int e2p_write_byte(u32 Addr, u8 *Data)
{
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    /* 1.开始 */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* 2.设备地址/写 */
    I2C_Send7bitAddress(I2C1, E2P_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.数据地址 */
    I2C_SendData(I2C1, (u8)(Addr>>8));                            //数据地址(16位)
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, (u8)(Addr&0x00FF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* 4.写一字节数据 */
    I2C_SendData(I2C1, *Data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* 5.停止 */
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}

static int write_page(u32 Addr, u8 *pData, u16 Length)
{
    u16 cnt;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    /* 1.开始 */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* 2.设备地址/写 */
    I2C_Send7bitAddress(I2C1, E2P_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.数据地址 */
    I2C_SendData(I2C1, (u8)(Addr>>8));                            //数据地址(16位)
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, (u8)(Addr&0x00FF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* 4.连续写数据 */
    for(cnt=0; cnt<(Length-1); cnt++)
    {
        I2C_SendData(I2C1, *pData);
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
        pData++;
    }
    I2C_SendData(I2C1, *pData);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* 5.停止 */
    I2C_GenerateSTOP(I2C1, ENABLE);
    delay_ms(5);

    return 0;
}


int e2p_write(u32 Addr, u8 *pData, u16 Length)
{
    u32 addr_offset;                                              //偏移地址
    u8  num_page = 0;                                             //页数(Length字节共多少页)
    u8  num_single = 0;                                           //"单"字节数(除了整页外的字节数)
    u8  count = 0;                                                //页剩余字节数量(偏移地址 -> 页末)

    addr_offset = Addr % PAGE_SIZE;                             //"起始地址"偏移该页地址多少
    count = PAGE_SIZE - addr_offset;                            //页剩余字节数量
    num_page = Length / PAGE_SIZE;                              //页数
    num_single = Length % PAGE_SIZE;

    /* 1.起始地址未偏移(位于页首地址) */
    if(0 == addr_offset) {
        /* 数据量小于1页 */
        if(0 == num_page) {
            write_page(Addr, pData, Length);                         //页首地址,写入小于1页的数据
        }
        /* 数据量大于等于1页 */
        else {
            while(num_page--) {                                     //写num_page页数据
                write_page(Addr, pData, PAGE_SIZE);
                Addr += PAGE_SIZE;
                pData += PAGE_SIZE;
            }
            
            if(0 != num_single) {                                  //写整页外剩下的字节数
                write_page(Addr, pData, num_single);
            }
        }
    }
    else {  /* 2.起始地址已偏移(不在页首地址) */
        /* 数据量小于1页 */
        if(0 == num_page) {
            /* 不超过该页 */
            if(Length < count) {
                write_page(Addr, pData, Length);                       //页偏移地址,写入小于该页的数据
            }
            /* 超过该页 */
            else {
                write_page(Addr, pData, count);                        //页偏移地址,写满该页的数据
                Addr += count;
                pData += count;
                                                                             //下页首地址,写完剩下的数据
                write_page(Addr, pData, Length - count);
            }
        }
        else {
            Length -= count;
            num_page = Length / PAGE_SIZE;                          //剩下的页数(减去前面写的数)
            num_single = Length % PAGE_SIZE;                        //最后一页需要写的字节数

            write_page(Addr, pData, count);                          //页偏移地址,写满该页的数据
            Addr += count;
            pData += count;

            while(num_page--) {                                                              //写num_page页数据
                write_page(Addr, pData, PAGE_SIZE);
                Addr += PAGE_SIZE;
                pData += PAGE_SIZE;
            }
            
            if(0 != num_single) {                                                              //写整页外剩下的字节数
                write_page(Addr, pData, num_single);
            }
        }
    }
  
    return 0;
}

int e2p_read(u32 Addr, u8 *pData, u16 Length)
{
    u16 cnt;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    /* 1.开始 */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* 2.设备地址/写 */
    I2C_Send7bitAddress(I2C1, E2P_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* 3.数据地址 */
    I2C_SendData(I2C1, (u8)(Addr>>8));                            //数据地址(16位)
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, (u8)(Addr&0x00FF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* 4.重新开始 */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* 5.设备地址/读 */
    I2C_Send7bitAddress(I2C1, E2P_ADDR, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* 6.读多字节数据 */
    for(cnt=0; cnt<(Length-1); cnt++)
    {
        I2C_AcknowledgeConfig(I2C1, ENABLE);                             //产生应答
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
        *pData = I2C_ReceiveData(I2C1);                                  //连续读取(Length-1)字节
        pData++;
    }
    I2C_AcknowledgeConfig(I2C1, DISABLE);                              //读取最后1字节(产生非应答)
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
    *pData = I2C_ReceiveData(I2C1);                                    //读取数据

    /* 7.停止 */
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}
#endif


#ifdef USE_STD_SW_I2C
#include "i2c2.h"

int e2p_init(void)
{
    I2C_Initializes();
    return 0;
}

int e2p_read_byte(u32 Addr, u8 *Data)
{
  u8  ack;

  /* 1.开始 */
  I2C_Start();

  /* 2.设备地址/写 */
  ack = I2C_WriteByte(E2P_ADDR | 0);
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }

  /* 3.数据地址 */
  ack = I2C_WriteByte((u8)(Addr>>8));       //数据地址(16位)
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }
  ack = I2C_WriteByte((u8)(Addr&0x00FF));
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }

  /* 4.重新开始 */
  I2C_Start();

  /* 5.设备地址/读 */
  ack = I2C_WriteByte(E2P_ADDR | 1);
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }

  /* 6.读一字节数据 */
  *Data = I2C_ReadByte(I2C_NOACK);               //只读取1字节(产生非应答)

  /* 7.停止 */
  I2C_Stop();

  return I2C_ACK;
}

int e2p_write_byte(u32 Addr, u8 *Data)
{
  u8  ack;

  /* 1.开始 */
  I2C_Start();

  /* 2.设备地址/写 */
  ack = I2C_WriteByte(E2P_ADDR | 0);
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }

  /* 3.数据地址 */
  ack = I2C_WriteByte((u8)(Addr>>8));       //数据地址(16位)
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }
  ack = I2C_WriteByte((u8)(Addr&0x00FF));
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }

  /* 4.写一字节数据 */
  ack = I2C_WriteByte(*Data);
  if(I2C_NOACK == ack)
  {
    I2C_Stop();
    return I2C_NOACK;
  }

  /* 5.停止 */
  I2C_Stop();

  return I2C_ACK;
}

int e2p_read(u32 addr, u8 *data, u16 len)
{
    int i;
    for(i=0; i<len; i++) {
        e2p_read_byte(addr+i, data+i);
    }
    return 0;
}

int e2p_write(u32 addr, u8 *data, u16 len)
{
    int i;
    for(i=0; i<len; i++) {
        e2p_write_byte(addr+i, data+i);
    }
    return 0;
}

#endif



#if 1
typedef struct {
    u32 x[1000];
}test_t;
int r1,r2;
u8 tmp;
test_t tt;
void e2p_test(void)
{
    int i;
    for(i=0;i<1000;i++) {
        tt.x[i] = i;
    }
    

    r1 = e2p_write(0, (u8*)&tt, sizeof(tt));

    memset(&tt, 0, sizeof(tt));
    r2 = e2p_read(0, (u8*)&tt, sizeof(tt));
    delay_ms(100);
}

#endif
