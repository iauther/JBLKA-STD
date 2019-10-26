#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "font.h"
#include "delay.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "config.h"

#define LCD_PWR(n)  (n?GPIO_WriteBit(LCD_PWR_GRP, LCD_PWR_PIN, Bit_SET):GPIO_WriteBit(LCD_PWR_GRP, LCD_PWR_PIN, Bit_RESET))
#define LCD_RST(n)  (n?GPIO_WriteBit(LCD_RST_GRP, LCD_RST_PIN, Bit_SET):GPIO_WriteBit(LCD_RST_GRP, LCD_RST_PIN, Bit_RESET))
#define LCD_RS(n)   (n?GPIO_WriteBit(LCD_RS_GRP, LCD_RS_PIN, Bit_SET):GPIO_WriteBit(LCD_RS_GRP, LCD_RS_PIN, Bit_RESET))
#define LCD_CS(n)   (n?GPIO_WriteBit(LCD_CS_GRP, LCD_CS_PIN, Bit_SET):GPIO_WriteBit(LCD_CS_GRP, LCD_CS_PIN, Bit_RESET))


#define LCD_BUF_SIZE 1000
u8 lcd_buf[LCD_BUF_SIZE];


static void gpio_output_set(GPIO_TypeDef  *grp, u32 pin)
{
    GPIO_InitTypeDef init;
    
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = pin;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(grp, &init);
}
static void lcd_gpio_init(void)
{
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );

    LCD_PWR(0);LCD_CS(0);LCD_RST(0);LCD_RS(0);

  	gpio_output_set(LCD_PWR_GRP, LCD_PWR_PIN);
    gpio_output_set(LCD_RST_GRP, LCD_RST_PIN);
    gpio_output_set(LCD_RS_GRP, LCD_RS_PIN);
    gpio_output_set(LCD_CS_GRP, LCD_CS_PIN);
}

#ifdef LCD_SOFT_SPI

#define	LCD_SDA(n)	(n?HAL_GPIO_WritePin(LCD_SDA_GRP, LCD_SDA_PIN, GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_SDA_GRP, LCD_SDA_PIN, GPIO_PIN_RESET))
#define	LCD_SCK(n)	(n?HAL_GPIO_WritePin(LCD_SCK_GRP, LCD_SCK_PIN, GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_SCK_GRP, LCD_SCK_PIN, GPIO_PIN_RESET))



static void lcd_pre_init(void)
{
    lcd_gpio_init();
}

static void lcd_write(u8 *data, u16 len)
{
    u8  i;
    u16 j=0;

    LCD_CS(0);
    while(len--) {
        for(i=8; i>0; i--,j++)
        {			  
            LCD_SCK(0);
            if(data[j] & (1<<(i-1)))
                LCD_SDA(1);
            else 
                LCD_SDA(0);
            LCD_SCK(1);
        }
    }
    LCD_CS(1);
}
#else
static void lcd_spi_init(void)
{
    SPI_InitTypeDef init={0};
    
    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );

    init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    init.SPI_Mode = SPI_Mode_Master;
    init.SPI_DataSize = SPI_DataSize_8b;
    init.SPI_CPOL = SPI_CPOL_Low;
    init.SPI_CPHA = SPI_CPHA_1Edge;
    init.SPI_NSS = SPI_NSS_Soft;
    init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    init.SPI_FirstBit = SPI_FirstBit_MSB;
    init.SPI_CRCPolynomial = 10;

    SPI_Init(SPI2, &init);
    SPI_Cmd(SPI2, ENABLE);
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

static void lcd_pre_init(void)
{
    lcd_gpio_init();
    lcd_spi_init();
}

static void lcd_write(u8 *data, u16 len)
{
    LCD_CS(0);
    spi_write(data, len);
    LCD_CS(1);
}
#endif

static void lcd_write_cmd(u8 cmd)
{
    LCD_RS(0);
    lcd_write(&cmd, 1);
}

static void lcd_write_data(u8 data)
{
    LCD_RS(1);
    lcd_write(&data, 1);
}


static void lcd_set_address(u16 x1, u16 y1, u16 x2, u16 y2)
{
    lcd_write_cmd(0x2a);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1);
    lcd_write_data(x2 >> 8);
    lcd_write_data(x2);

    lcd_write_cmd(0x2b);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1);
    lcd_write_data(y2 >> 8);
    lcd_write_data(y2);

    lcd_write_cmd(0x2C);
}


static void lcd_display(int on)
{
    if(on) {
        lcd_write_cmd(0x29);
        LCD_PWR(1); 
    }
    else {
        LCD_PWR(0); 
        lcd_write_cmd(0x28);
    }
}

static void lcd_reset(void)
{
    LCD_RST(1);
    delay_ms(2);
    LCD_RST(0);
    delay_ms(10);
    LCD_RST(1);
    delay_ms(120);
}
////////////////////////////////////////////////////////////
void lcd_init(void)
{
    lcd_pre_init();
    lcd_reset();

    lcd_write_cmd(0x29);
    lcd_write_cmd(0x11);
    delay_ms(120);

    /* Memory Data Access Control */
    lcd_write_cmd(0x36);
/*
    D7  MY,  page addr order         0: top to bottom              1: bottom to top
    D6  MX,  colum addr order        0: left to right              1: right to left
    D5  MV,  page/column order       0: normal mode                1: reverse mdoe
    D4  ML,  line address order      0: refresh top to bottom      1: refresh bottom to top
    D3  RGB, RGB order:              0: RGB                        1: BGR
    D2  MH,  data latch data order   0: refresh left to right      1: refresh right to left
*/
    lcd_write_data(0x60);
    //lcd_write_data(0xA0);

    /* RGB 5-6-5-bit  */
    lcd_write_cmd(0x3A);
    lcd_write_data(0x55);

    /* Porch Setting 
    lcd_write_cmd(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);*/

    /*  Gate Control */
    lcd_write_cmd(0xB7);
    lcd_write_data(0x72);

    /* VCOM Setting */
    lcd_write_cmd(0xBB);
    lcd_write_data(0x3D);   //Vcom=1.625V

    /* LCM Control */
    lcd_write_cmd(0xC0);
    lcd_write_data(0x2C);

    /* VDV and VRH Command Enable */
    lcd_write_cmd(0xC2);
    lcd_write_data(0x01);

    /* VRH Set */
    lcd_write_cmd(0xC3);
    lcd_write_data(0x19);

    /* VDV Set */
    lcd_write_cmd(0xC4);
    lcd_write_data(0x20);

    /* Frame Rate Control in Normal Mode */
    lcd_write_cmd(0xC6);
    lcd_write_data(0x0F);   //60MHZ

    /* Power Control 1 */
    lcd_write_cmd(0xD0);
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    /* Positive Voltage Gamma Control */
    lcd_write_cmd(0xE0);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0D);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2B);
    lcd_write_data(0x3F);
    lcd_write_data(0x54);
    lcd_write_data(0x4C);
    lcd_write_data(0x18);
    lcd_write_data(0x0D);
    lcd_write_data(0x0B);
    lcd_write_data(0x1F);
    lcd_write_data(0x23);

    /* Negative Voltage Gamma Control */
    lcd_write_cmd(0xE1);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0C);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2C);
    lcd_write_data(0x3F);
    lcd_write_data(0x44);
    lcd_write_data(0x51);
    lcd_write_data(0x2F);
    lcd_write_data(0x1F);
    lcd_write_data(0x1F);
    lcd_write_data(0x20);
    lcd_write_data(0x23);

    lcd_clear(LCD_BC);
    lcd_draw_string(50, 100, 200, 60, (u8*)"FFF U!", FONT_32, LCD_FC);

    lcd_display(1);
}


void lcd_clear(u16 color)
{   
    lcd_fill_rect(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1, color);
}


void lcd_draw_point(u16 x, u16 y, u16 color)
{
    lcd_set_address(x, y, x, y);
    lcd_write_data(color >> 8);
    lcd_write_data(color & 0x00ff); 
}


void lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16  delta_x = 0, delta_y = 0;
    s8   incx = 0, incy = 0;
    u16  distance = 0;
    u16  t = 0;
    u16  x = 0, y = 0;
    u16  x_temp = 0, y_temp = 0;

    /* 画斜线（Bresenham算法） */
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    if(delta_x > 0)
    {
        //斜线(从左到右)
        incx = 1;
    }
    else if(delta_x == 0)
    {
        //垂直斜线(竖线)
        incx = 0;
    }
    else
    {
        //斜线(从右到左)
        incx = -1;
        delta_x = -delta_x;
    }
    if(delta_y > 0)
    {
        //斜线(从左到右)
        incy = 1;
    }
    else if(delta_y == 0)
    {
        //水平斜线(水平线)
        incy = 0;
    }
    else
    {
        //斜线(从右到左)
        incy = -1;
        delta_y = -delta_y;
    }           
    
    /* 计算画笔打点距离(取两个间距中的最大值) */
    if(delta_x > delta_y)
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y;
    }
    
    /* 开始打点 */
    x = x1;
    y = y1;
    //第一个点无效，所以t的次数加一
    for(t = 0; t <= distance + 1;t++)
    {
        lcd_draw_point(x, y, color);
    
        /* 判断离实际值最近的像素点 */
        x_temp += delta_x;  
        if(x_temp > distance)
        {
            //x方向越界，减去距离值，为下一次检测做准备
            x_temp -= distance;     
            //在x方向递增打点
            x += incx;
                
        }
        y_temp += delta_y;
        if(y_temp > distance)
        {
            //y方向越界，减去距离值，为下一次检测做准备
            y_temp -= distance;
            //在y方向递增打点
            y += incy;
        }
    }
}


void lcd_draw_char(u16 x, u16 y, u8 c, u8 font, u16 color)
{                             
    u16 temp, t1, t;
    u16 y0 = y;
    font_info_t inf = font_get(font);
    
    c = c - ' ';    /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */
    for(t = 0; t < inf.size; t++)  /*遍历打印所有像素点到LCD */
    {   
        if(FONT_16 == font)
        {
            temp = font_1608[c][t];   /* 调用1608字体 */
        }
        else if(FONT_24 == font)
        {
            temp = font_2412[c][t];   /* 调用2412字体 */
        }
        else if(FONT_32 == font)
        {
            temp = font_3216[c][t];   /* 调用3216数码管字体 */
        }
        else if(FONT_48 == font)
        {
            temp = font_4824[c][t];   /* 调用4824数码管字体 */
        }
        else
        {   
            return;     /* 没有找到对应的字库 */
        }
        for(t1 = 0; t1 < 8; t1++)   /* 打印一个像素点到液晶 */
        {               
            if(temp & 0x80)
            {
                lcd_draw_point(x, y, color);
            }
            temp <<= 1;
            y++;
            
            if(y >= LCD_HEIGHT)
            {
                return;     /* 超区域了 */
            }
            if((y - y0) == inf.height)
            {
                y = y0;
                x++;
                if(x >= LCD_WIDTH)
                {
                    return; /* 超区域了 */
                }
                break;
            }
        }    
    }                     
} 


void lcd_draw_string(u16 x, u16 y, u16 w, u16 h, u8 *str, u8 font, u16 color)
{
    u16 x0 = x;
    font_info_t inf = font_get(font);
    
    w += x;
    h += y;
    
    while((*str<='~') && (*str>=' '))       /* 判断是不是非法字符! */
    {       
        if(x >= w)
        {
            x = x0; 
            y += inf.height;
        }
        
        if(y >= h)
        {   
            break;
        }
        lcd_draw_char(x, y, *str, font, color);
        x += inf.height/2;
        str++;
    }  
}



void lcd_draw_rect(u16 x1, u16 y1, u16 w, u16 h, u16 color)
{
    u16  x2,y2;

    x2 = x1 + w;
    y2 = y1 + h;
    lcd_draw_line(x1,y1,x2,y1,color);
    lcd_draw_line(x1,y1,x1,y2,color);
    lcd_draw_line(x1,y2,x2,y2,color);
    lcd_draw_line(x2,y1,x2,y2,color);
}


void lcd_fill_rect(u16 x1, u16 y1, u16 w, u16 h, u16 color)
{
    u16 i = 0;
    u16  x2,y2;
    u32 size = 0, size_remain = 0;
    
    x2 = x1 + w;
    y2 = y1 + h;
    size = (x2 - x1 + 1) * (y2 - y1 + 1) * 2;

    if(size > LCD_BUF_SIZE)
    {
        size_remain = size - LCD_BUF_SIZE;
        size = LCD_BUF_SIZE;
    }

    lcd_set_address(x1, y1, x2, y2);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color & 0xff;
        }

        LCD_RS(1);
        lcd_write(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_BUF_SIZE)
        {
            size_remain = size_remain - LCD_BUF_SIZE;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}


void lcd_draw_string_center(u16 x, u16 y, u16 w, u16 h, u8 *str, u8 font, u16 color)
{
    u16  w1;
    font_info_t inf;
    
    inf = font_get(font);
    w1 = strlen((char*)str)*inf.width;
    
    lcd_fill_rect(x, y, w, h, LCD_BC);
    lcd_draw_string(x+(w-w1)/2, y+(h-inf.height)/2, w1, inf.height, str, font, color);
}


