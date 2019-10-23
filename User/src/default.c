#include "default.h"
#include "config.h"

#define offset_of(type, member) ((u32) &((type *)0)->member)


const default_t gDefault={
    //gain
    .gain = {
        .Gain       = 0,        //0~100
        .Mute       = 0,        //0:mute off  1:mute on
    },
    
    //vol
    .vol = {
        .Vol        = 0,        //0~200     step=1
        .Phase      = 0,        //0:不反相  1:反相
    },
    
    //eq
    .eq = {
        .Freq       = 20,       //20~20000
        .Gain       = 0,        //-24~+12dB
        .Q          = 1,        //1~1280
        .Type       = 0,        //0,1,2: PEQ,LS,HS
        .Bypass     = 0,        //0:not bypass  1:bypass
    },
    
    //hlpf
    .hlpf = {
        .Freq       = 20,       //20~21000
        .Type       = 0,        //HLPF_Type_Bypass
    },
    
    //delay
    .delay = {
        .Delay      = 0,
    },
    
    //feedback
    .feedback = {
        .FeedBack   = 0,        //移频量, 0:off,  1~3:4Hz/6Hz/8Hz
    },
    
    //limiter
    .limiter = {
        .threshold  = -34,      //启动电平 -34~+0dB
        .attackTime = 0,        //启动时间 0~900: 0~900ms
        .releaseTime= 0,        //释放时间0~9000: 0~9000ms
        .ratio      = 0,        //压限比例 0~1000: 0~100.0
    },
    
    //pitch
    .pitch = {
        .PitchShift = -5,       //变调 -5~+5
    },
    
    //mute
    .mute = {
        .Mute       = 0,        //0: mute off  1: mute on
    },
    
    //noisegate
    .noisegate = {
        .threshold  = -60,      //启动阀值  -60~0dB
    },
};

const para_info_t infoGain[] = {
    //
    {"GAIN",    "1",        1,          1000,           1,          1},
    {"MUTE",    "1",        0,          1,              1,          1},
};

const para_info_t infoEq[] = {
    //
    {"FREQ",    "Hz",       20,         20000,          1,          1},
    {"GAIN",    "dB",       -24,        12,             0.1,        1},
    {"Q",       "",         1,          1280,           1,          10},
    {"TYPE",    "",         0,          2,              1,          1},
    {"BYPASS",  "",         0,          1,              1,          1},
};

const para_info_t infoHlpf[] = {
    {"FREQ",    "",         20,         21000,          1,          1},
    {"TYPE",    "",         0,          8,              1,          1},
};

const para_info_t infoVol[] = {
    {"VOL",      "",         0,         200,             1,          1},
    {"PHASE",    "",         0,          1,              1,          1},
};

const para_info_t infoDelay[] = {
    {"Echo PreDelay",       "ms",        0,         4800,             48,          48},
    {"Echo Delay",          "ms",        0,         14400,            48,          48},

    {"Reverb PreDelay",     "ms",        0,         4800,             48,          48},
    {"Reverb Delay",        "ms",        0,         8000,              1,           1},

    {"MainL Delay",         "ms",        0,         1920,             48,          48},
    {"MainR Delay",         "ms",        0,         1920,             48,          48},

    {"Sub Delay",           "ms",        0,         1920,             48,          48},
};

const fw_info_t FW_INFO = {
    .ver1 = VERSION,
    .ver2 = "",
    .bldtime = __DATE__,
};

const io_data_t IO_DATA = {
    .amp_en = 1,
    .pwr_en = 1,
    .rca_mute = 1,
    .input = 0,
    
    .amp_ppwr = 1,
    .amp_npwr = 0,
    .rx485_en = 0,
    .tx485_en = 0,
};

const u16 GEQ2_FREQ[2] = {500, 5000};
const u16 GEQ3_FREQ[3] = {100, 1000, 10000};
const u16 PEQ3_FREQ[3] = {1000, 1000, 1000};
const u16 PEQ7_FREQ[7] = {1000, 1000, 1000, 1000, 1000, 1000, 1000};


