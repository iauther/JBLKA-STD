#ifndef __FONT_H__
#define __FONT_H__	 

#include "types.h"

//常用ASCII表
//偏移量32 
//ASCII字符集: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//PC2LCD2002取模方式设置：阴码+逐列式+顺向+C51格式
//每个字符所占用的字节数为:(size/8+((size%8)?1:0))*(size/2),其中size:是字库生成时的点阵大小(12/16/24...)

extern const unsigned char font_1608[95][16]; 
extern const unsigned char font_2412[95][36];
extern const unsigned char font_3216[95][64];
extern const unsigned char font_4824[95][144];
extern const unsigned char font_6432[95][256];
extern const unsigned char font_8040[10][400];

enum {
    FONT_16=0,
    FONT_24,
    FONT_32,
    FONT_48,
    FONT_64,
    FONT_80,

    FONT_MAX
};

typedef struct {
    u8  width;
    u8  height;
    u8  size;
}font_info_t;


font_info_t font_get(u8 font);

#endif

