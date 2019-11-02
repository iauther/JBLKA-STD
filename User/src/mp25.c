#include "mp25.h"
#include "stm32f10x.h"



static void set_cs(int v)
{
    if (v) {
        GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_SET);
    }
    else {
        GPIO_WriteBit(GPIOC, GPIO_Pin_6, Bit_RESET);
    }
}
static void spi_write(u8 *data, u16 len)
{
    u16 i;
    for(i=0; i<len; i++) {
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPI2, data[i]);
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
        SPI_I2S_ReceiveData(SPI2);
    }
}
static u8 spi_write_byte(u8 data)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, data);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return (u8)SPI_I2S_ReceiveData(SPI2);
}


static void io_config(void)
{
    GPIO_InitTypeDef init;
    SPI_InitTypeDef sinit={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );
    
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_6;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &init);

    sinit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    sinit.SPI_Mode = SPI_Mode_Master;
    sinit.SPI_DataSize = SPI_DataSize_8b;
    sinit.SPI_CPOL = SPI_CPOL_High;
    sinit.SPI_CPHA = SPI_CPHA_2Edge;
    sinit.SPI_NSS = SPI_NSS_Soft;
    sinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    sinit.SPI_FirstBit = SPI_FirstBit_MSB;
    sinit.SPI_CRCPolynomial = 10;

    SPI_Init(SPI2, &sinit);
    SPI_Cmd(SPI2, ENABLE);
}


static int wait_busy(void)
{
    u8 st=0x01;
	set_cs(1);
   /* 读取状态寄存器中的数据，判断忙标志0x01位 置位代表忙 */
	//SPI_SendData(W25X_ReadStatusReg);      
   /* 只读取状态寄存器的BUSY位，即第一位 */
	//while((st & 0x01) == 1) 
	//	st=spi_write_byte(0x00); 
	set_cs(0);

    return 0;
}

static int read_enable(void)
{
    //spi_write_byte(CMD_READ);

    return 0;
}
static int write_enable(void)
{
    //spi_write_byte(CMD_WRITE);
    
    return 0;
}
static int read_id(u8 *id, u8 len)
{
    u8 i;
 
    if(len<20) {
        return -1;
    }

    set_cs(0);
    //spi_write_byte(CMD_READ_ID); 

    for(i=0;i<20;i++) {
        id[i] = spi_write_byte(0); 
    }
 
    set_cs(1);

    return 0;
}


static int erase_sector(u32 addr)
{
    write_enable();
    //spi_write_byte(CMD_ERASE);
    spi_write_byte((addr & 0xff0000) >> 16);
	spi_write_byte((addr & 0xff00) >> 8);
	spi_write_byte(addr & 0xff);

    wait_busy();

    return 0;
}


static int write_page(u32 addr, u8 *data, int len)
{
    
    write_enable();

    set_cs(1);

    return 0;
}



int mp25_init(void)
{
    //
    return 0;
}


int mp25_read(u32 addr, u8 *data, int len)
{
    int i;
    set_cs(0);

    read_enable();
    spi_write_byte((addr & 0xff0000) >> 16);
	spi_write_byte((addr & 0xff00) >> 8);
	spi_write_byte(addr & 0xff);
    for(i=0;i<len;i++) {
        data[i] = spi_write_byte(0);
    }

    set_cs(1);

    return 0;
}


int mp25_write(u32 addr, u8 *data, int len)
{
    int i;

    set_cs(0);
    write_enable();
    //spi_write_byte(CMD_PAGE_PROGRAM);

    spi_write_byte((addr & 0xff0000) >> 16);
	spi_write_byte((addr & 0xff00) >> 8);
	spi_write_byte(addr & 0xff);
    for(i=0;i<len;i++) {
        spi_write_byte(data[i]);
    }
    set_cs(1);
    wait_busy();

    return 0;
}





