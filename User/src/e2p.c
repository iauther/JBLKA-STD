#include "e2p.h"
#include "i2c.h"
#include "delay.h"
#include "config.h"
#ifdef APP
//#include "cmsis_os2.h"
#endif

#define E2P_ADDR        0xA0
#ifdef USE_24XX512
    #define PAGE_SIZE       128
#else   //24C02
    #define PAGE_SIZE       8
#endif



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
    //e2p_test();
    
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


int e2p_write(u32 addr, u8 *data, u16 len)
{
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
    return r;
}

#if 0
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
