#ifndef __IR_H__
#define __IR_H__

#include "types.h"

typedef struct {
    u32  key;       //按键值
    u16  htime;     //高电平时间,单位us
    u16  ltime;     //低电平时间,单位us
	u8   bits;
	u8   started;
    u8   repeat;	
}ircode_t;
 
typedef struct {
    u8   code;
    u8   key;
}irtab_t;

int ir_init(void);


#endif
