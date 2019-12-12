#include <stdio.h>
#include <string.h>
#include "dac.h"
#include "lcd.h"
#include "lock.h"
#include "delay.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "config.h"

#define BACKLIGHT_USE_DAC

#define LCD_PWR(n)  (n?GPIO_WriteBit(LCD_PWR_GRP, LCD_PWR_PIN, Bit_SET):GPIO_WriteBit(LCD_PWR_GRP, LCD_PWR_PIN, Bit_RESET))
#define LCD_RST(n)  (n?GPIO_WriteBit(LCD_RST_GRP, LCD_RST_PIN, Bit_SET):GPIO_WriteBit(LCD_RST_GRP, LCD_RST_PIN, Bit_RESET))
#define LCD_RS(n)   (n?GPIO_WriteBit(LCD_RS_GRP, LCD_RS_PIN, Bit_SET):GPIO_WriteBit(LCD_RS_GRP, LCD_RS_PIN, Bit_RESET))
#define LCD_CS(n)   (n?GPIO_WriteBit(LCD_CS_GRP, LCD_CS_PIN, Bit_SET):GPIO_WriteBit(LCD_CS_GRP, LCD_CS_PIN, Bit_RESET))


#define LCD_BUF_SIZE 2000
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

    LCD_CS(0);LCD_RST(0);LCD_RS(0);

#ifdef BACKLIGHT_USE_DAC
    dac_init(DAC_CH2);
#else
  	gpio_output_set(LCD_PWR_GRP, LCD_PWR_PIN);
#endif
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
    init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    init.SPI_FirstBit = SPI_FirstBit_MSB;
    init.SPI_CRCPolynomial = 10;

    SPI_Init(SPI2, &init);
    SPI_Cmd(SPI2, ENABLE);
}

static inline void spi_write(u8 *data, u16 len)
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

static inline void lcd_write(u8 *data, u16 len)
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


static inline void lcd_set_address(u16 x1, u16 y1, u16 x2, u16 y2)
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
        lcd_set_bright(100);
    }
    else {
        lcd_write_cmd(0x28);
        lcd_set_bright(0);
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
    lcd_display(1);
}


void lcd_clear(u16 color)
{   
    lcd_fill_rect(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1, color);
}


void lcd_set_bright(u8 percent)
{
    if(percent>100) {
        return;
    }

#ifdef BACKLIGHT_USE_DAC
    dac_set(DAC_CH2, (percent*4095)/100);
#else
    LCD_PWR(percent);
#endif
}



inline void lcd_draw_point(u16 x, u16 y, u16 color)
{
    u8 tmp[2];

    lcd_set_address(x, y, x, y);

    LCD_RS(1);
    tmp[0] = color>>8;
    tmp[1] = color&0xff;
    lcd_write(tmp, 2);

    //lcd_write_data(color >> 8);
    //lcd_write_data(color & 0x00ff); 
}


void lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		lcd_draw_point(uRow,uCol,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

u16 tmpBuf[600];
void lcd_draw_char2(u16 x, u16 y, u8 c, u8 font, u16 color, u16 bgcolor, u8 pure)
{
    u16 i,j,len;
    font_info_t inf = font_get(font);
        
    if(font==FONT_96) {
        if(c=='.') c = 10;
        else if(c>='0' && c<='9') c = c-'0';
        else return;
    }
    else {
        c = c - ' ';
    }
    
    for(i=0; i<inf.size; i++) {
        //一列一列的刷
        



    }


}
void lcd_draw_char(u16 x, u16 y, u8 c, u8 font, u16 color, u16 bgcolor, u8 pure)
{                             
    u16 temp, t1, t;
    u16 y0 = y;
    font_info_t inf = font_get(font);
    
    if(font==FONT_96) {
        if(c=='.') {
            c = 10;
        }
        else if(c>='0' && c<='9'){
            c = c-'0';
        }
        else {
            return;
        }
    }
    else {
        c = c - ' ';
    }


    for(t = 0; t < inf.size; t++) {
        if(FONT_16 == font) {
            temp = font_1608[c][t];
        }
        else if(FONT_24 == font) {
            temp = font_2412[c][t];
        }
        else if(FONT_32 == font) {
            temp = font_3216[c][t];
        }
        else if(FONT_48 == font){
            temp = font_4824[c][t];
        }
        else if(FONT_96 == font){
            temp = font_9648[c][t];
        }
        else {   
            return;
        }
        for(t1 = 0; t1 < 8; t1++) {
                       
            if(temp & 0x80) {
                lcd_draw_point(x, y, color);
            }
            else {
                if(!pure) {
                    lcd_draw_point(x, y, bgcolor);
                }
            }

            temp <<= 1;
            y++;
            
            if(y >= LCD_HEIGHT) {
                return;
            }
            if((y - y0) == inf.height) {
                y = y0;
                x++;
                if(x >= LCD_WIDTH) {
                    return;
                }
                break;
            }
        }    
    }                     
}


void lcd_draw_string(u16 x, u16 y, u16 w, u16 h, u8 *str, u8 font, u16 color, u16 bgcolor, u8 pure)
{
    u16 x0 = x;
    font_info_t inf = font_get(font);
    
    w += x;
    h += y;
    while((*str<='~') && (*str>=' ')) {       /* 鍒ゆ柇鏄笉鏄潪娉曞瓧绗? */
        if(x >= w) {
            x = x0; 
            y += inf.height;
        }
        
        if(y >= h) {   
            break;
        }
        lcd_draw_char(x, y, *str, font, color, bgcolor, pure);
        x += inf.height/2;
        str++;
    }  
}


void lcd_draw_rect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
    u16  x2,y2;

    x2 = x + w;
    y2 = y + h;
    lcd_draw_line(x, y, x2, y, color);
    lcd_draw_line(x, y, x, y2, color);
    lcd_draw_line(x, y2, x2, y2, color);
    lcd_draw_line(x2, y, x2, y2, color);
}


void lcd_draw_arc( u16 x, u16 y, u16 r, u8 s, u16 color)
{
    u16 x1,y1,xd,yd,e;

    xd = 1 - (r << 1);
    yd = 0;
    e = 0;
    x1 = r;
    y1 = 0;

    while (x1 >= y1) {
        // Q1
        if ( s & 0x01 ) lcd_draw_point(x + x1, y - y1, color);
        if ( s & 0x02 ) lcd_draw_point(x + y1, y - x1, color);

        // Q2
        if ( s & 0x04 ) lcd_draw_point(x - y1, y - x1, color);
        if ( s & 0x08 ) lcd_draw_point(x - x1, y - y1, color);

        // Q3
        if ( s & 0x10 ) lcd_draw_point(x - x1, y + y1, color);
        if ( s & 0x20 ) lcd_draw_point(x - y1, y + x1, color);

        // Q4
        if ( s & 0x40 ) lcd_draw_point(x + y1, y + x1, color);
        if ( s & 0x80 ) lcd_draw_point(x + x1, y + y1, color);

        y1++;
        e += yd;
        yd += 2;
        if (((e << 1) + xd) > 0) {
            x1--;
            e += xd;
            xd += 2;
        }
    }
}


void lcd_draw_round_rect(u16 x, u16 y, u16 w, u16 h, u16 r, u16 color)
{
    lcd_draw_line(x+r, y,   x+w-r, y,   color);
    lcd_draw_line(x+r, y+h, x+w-r, y+h, color);
    lcd_draw_line(x,   y+r, x,   y+h-r, color);
    lcd_draw_line(x+w, y+r, x+w, y+h-r, color);

    lcd_draw_arc(x+r, y+r, r, 0x0c, color);
    lcd_draw_arc(x+w-r, y+r, r, 0x03, color);
    lcd_draw_arc(x+r, y+h-r, r, 0x30, color);
    lcd_draw_arc(x+w-r, y+h-r, r, 0xc0, color);
}


void lcd_fill_rect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
    u16 i = 0;
    u16  x2,y2;
    u32 size = 0, size_remain = 0;
    
    x2 = x + w;
    y2 = y + h;
    size = (x2 - x + 1) * (y2 - y + 1) * 2;

    if(size > LCD_BUF_SIZE) {
        size_remain = size - LCD_BUF_SIZE;
        size = LCD_BUF_SIZE;
    }

    lcd_set_address(x, y, x2, y2);
    while(1) {
        for(i = 0; i < size / 2; i++) {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color & 0xff;
        }

        LCD_RS(1);
        lcd_write(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_BUF_SIZE) {
            size_remain = size_remain - LCD_BUF_SIZE;
        }

        else {
            size = size_remain;
            size_remain = 0;
        }
    }
}


void lcd_fill_round_rect(u16 x, u16 y, u16 w, u16 h, u16 r, u16 color)
{
    u16 x2,y2,x3,y3,xd;
    x2 = x+w;
    y2 = y+h;

    if (x2 < x) {
        x3 = x2;
        x2 = x;
        x = x3;
    }

   if ( y2 < y ) {
        y3 = y2;
        y2 = y;
        y = y3;
   }

    xd = 3-(r<<1);
    x3 = 0;
    y3 = r;
    
    lcd_fill_rect(x+r, y, w-r*2, h, color);
    while (x3 <= y3) {
        if( y3 > 0 ) {
            lcd_draw_line(x2+x3-r, y-y3+r, x2+x3-r, y3+y2-r, color);
            lcd_draw_line(x-x3+r, y-y3+r, x-x3+r, y3+y2-r, color);
        }

        if(x3 > 0) {
            lcd_draw_line(x - y3 + r, y - x3 + r, x - y3 + r, x3 + y2 - r, color);
            lcd_draw_line(x2 + y3 - r, y - x3 + r, x2 + y3 - r, x3 + y2 - r, color);
        }

        if (xd < 0) {
            xd += (x3 << 2) + 6;
        }
        else {
            xd += ((x3-y3)<<2)+10;
            y3--;
        }
        x3++;
    }
}


void lcd_draw_string_align(u16 x, u16 y, u16 w, u16 h, u8 *str, u8 font, u16 color, u16 bgcolor, u8 align, u8 pure)
{
    font_info_t info;
    u16 x2,y2,w2,h2;
    
    info = font_get(font);
    w2 = strlen((char*)str)*info.width;
    y2 = y+(h-info.height)/2;
    h2 = info.height;
    
    if(w2>w) {
        w2 = w;
    }

    if(align==ALIGN_LEFT) {
        x2 = x;
    }
    else if(align==ALIGN_MIDDLE) {
        x2 = x+(w-w2)/2;
    }
    else if(align==ALIGN_RIGHT){
        x2 = x+w-w2;
    }
    else {
        return;
    }

#if 0
    lcd_fill_rect(x, y, x2-x, h, bgcolor);
    lcd_fill_rect(x2+w2, y, w-w2-(x2-x), h, bgcolor);

    lcd_fill_rect(x2, y, w2, y2-y, bgcolor);
    lcd_fill_rect(x2, y2+h2, w2, h-h2-(y2-y), bgcolor);
#endif

    lcd_draw_string(x2, y2, w2, h2, str, font, color, bgcolor, pure);
}



