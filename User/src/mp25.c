#include "mp25.h"
#include "stm32f10x.h"

#define PAGE_SIZE               512
#define SECTOR_SIZE             (64*1024)       //32 sectors
#define SECTOR_MAX              32


#define CMD_WRITE_EN            0x06
#define CMD_READ_ID             0x9f
#define CMD_READ_STATUS         0x05
#define CMD_WRITE_STATUS        0x01
#define CMD_READ_BYTES          0x03
#define CMD_FAST_READ_BYTES     0x0b
#define CMD_WRITE_PAGE          0x02
#define CMD_ERASE_SECTOR        0xd8
#define CMD_ERASE_BULK          0xc7


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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    set_cs(1);
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_6;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &init);

/*
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
*/
}
static void send_cmd(u8 cmd)
{
    set_cs(0);
    spi_write_byte(cmd);
    set_cs(1);
}

static void wait_write(void)
{
	while(spi_write_byte(CMD_READ_STATUS)&0x01);
}


static void read_id(u8 *id, u8 len)
{
    u8 i;
 
    if(len<3) {
        return;
    }

    send_cmd(CMD_READ_ID);

    set_cs(0);
    for(i=0;i<3;i++) {
        id[i] = spi_write_byte(0); 
    }
    set_cs(1);
}


static int erase_sector(u32 addr)
{
    send_cmd(CMD_ERASE_SECTOR);
    
    set_cs(0);
    spi_write_byte((addr & 0xff0000) >> 16);
	spi_write_byte((addr & 0xff00) >> 8);
	spi_write_byte(addr & 0xff);
    wait_write();
    set_cs(1);

    return 0;
}



int mp25_init(void)
{
    u8 id[4];
    io_config();
    read_id(id, 4);

    return 0;
}


static void my_read(u32 addr, u8 *data, int len)
{
    int i;

    send_cmd(CMD_READ_BYTES);
    
    set_cs(0);
    spi_write_byte((addr & 0xff0000) >> 16);
	spi_write_byte((addr & 0xff00) >> 8);
	spi_write_byte(addr & 0xff);
    for(i=0;i<len;i++) {
        data[i] = spi_write_byte(0);
    }
    set_cs(1);
}
static void my_write(u32 addr, u8 *data, int len)
{
    int i;

    spi_write_byte(CMD_WRITE_PAGE);

    set_cs(0);
    spi_write_byte((addr & 0xff0000) >> 16);
	spi_write_byte((addr & 0xff00) >> 8);
	spi_write_byte(addr & 0xff);
    for(i=0;i<len;i++) {
        data[i] = spi_write_byte(0);
    }
    set_cs(1);
}


int mp25_read(u32 addr, u8 *data, int len)
{
    int i;

    set_cs(0);
    my_read(addr, data, len);
    set_cs(1);

    return 0;
}

static void write_en(void)
{
    set_cs(0);
    spi_write_byte(CMD_WRITE_EN);
    set_cs(1);
}
int mp25_write(u32 addr, u8 *data, int len)
{
    int i;

    write_en();

    set_cs(0);
    spi_write_byte(CMD_WRITE_PAGE);

    spi_write_byte((addr & 0xff0000) >> 16);
	spi_write_byte((addr & 0xff00) >> 8);
	spi_write_byte(addr & 0xff);
    for(i=0;i<len;i++) {
        spi_write_byte(data[i]);
    }
    set_cs(1);
    wait_write();

    return 0;
}





