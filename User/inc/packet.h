#ifndef __PACKET_H__
#define __PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "dsp.h"

typedef enum {
    TYPE_DSP=0,                     //dsp_data_t in dsp.h
    TYPE_IODAT,                     //io_data_t
    TYPE_STATUS,                    //device status
    TYPE_PARAS,                     //paras_data_t
    TYPE_PRESET,                    //preset_data_t
    TYPE_ACK,                       //ack_data_t
    TYPE_DEFAULT,                   //reset paras to default
    TYPE_EQRESET,                   //eq reset
    
    TYPE_UPGRADE=0xA0,              //used for upgrade dsp,mcu firmware and bootloader
    TYPE_VERSION,
    TYPE_UPGRADE_REQ,
    TYPE_REBOOT,

    TYPE_APP_EXIT=0xE0,             //pc application exit
}eTYPE;

typedef enum {
    INPUT_VOD,
    INPUT_DVD,
    INPUT_BGM,
    INPUT_BTUSB,
    INPUT_OPT,
    INPUT_HDMI_IN,
    INPUT_HDMI_ARC,

    INPUT_MAX
}eINPUT;

#pragma pack(1)

typedef struct {
    u8              amp_temp;           //amplifier temperature
    u8              pwr_temp;
    u8              fan_speed;
}status_data_t;

typedef struct {
    u8              amp_en;
    u8              pwr_en;
    u8              rca_mute;
    u8              input;
    
    u8              amp_ppwr;           //positive power
    u8              amp_npwr;           //negative power
    u8              rx485_en;
    u8              tx485_en;
}io_data_t;

typedef struct {
    u8              mode;               //1: boot   2: app
    u8              model[10];          //KA350 or KA650i
    u8              boot[10];           //bootloader version
    u8              mcu[10];            //mcu firmware version
    u8              dsp[10];            //dsp firmware version
}fw_info_t;

typedef struct {
    u8              index;             //preset index
    u8              dlen;              //sizeof(Dsp_Paras)
    u8              data[];            //Dsp_Paras
}preset_data_t;

typedef struct {
    fw_info_t       fw;

    Dsp_Paras       dsp;
    io_data_t       iodat;
    u8              pre;                //the current used preset index
}paras_data_t;

typedef struct {   //only used for mcu ui display
    fw_info_t       *pfw;
    dsp_paras_t     dsp;
    io_data_t       *pio;
    u8              *pre;
}paras_ui_t;

typedef struct {
    u8              type;       //eTYPE, the type of the ack respond to
    s32             err;
}ack_data_t;

typedef struct {
    //u16             magic;            //magic number: 0xaa88, reserved
    u8              type;               //refer to eTYPE
    u8              nck;                //0: no need ack, 1: need ack
    u16             pkts;               //sub spacket number
    u16             pid;                //sub packet id, start form 0
    u16             dlen;               //sub packet data length
    u8              data[];             //sub packet data;
    //data          ......
    //u16           crc;                //use it?  reserved
}packet_t;
#pragma pack()


#ifdef APP
extern paras_data_t gParams; 
extern paras_ui_t   uiParams;
extern status_data_t gStatus;
#endif

#ifdef __cplusplus
}
#endif


#endif
