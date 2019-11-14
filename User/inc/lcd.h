#ifndef __LCD_H__
#define __LCD_H__

#include "types.h"

/* 颜色 */
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430

#define LCD_WIDTH        320
#define LCD_HEIGHT       240

typedef struct {
    u16 x;
    u16 y;
    u16 w;
    u16 h;
}rect_t;



void lcd_init(void);
void lcd_clear(u16 color);
void lcd_draw_point(u16 x, u16 y, u16 color);
void lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void lcd_draw_char(u16 x, u16 y, u8 c, u8 font, u16 color, u16 bgcolor);
void lcd_draw_string(u16 x, u16 y, u16 width, u16 height, u8 *str, u8 font, u16 color, u16 bgcolor);
void lcd_draw_rect(u16 x1, u16 y1, u16 w, u16 h, uint16_t color);
void lcd_fill_rect(u16 x1, u16 y1, u16 w, u16 h, u16 color);
void lcd_draw_string_center(u16 x, u16 y, u16 w, u16 h, u8 *str, u8 font, u16 color, u16 bgcolor);

#endif




