#include <stdio.h>
#include "default.h"
#include "config.h"

#define offset_of(type, member) ((u32) &((type *)0)->member)


const def_dsp_t DEF_DSP={
    .music = {
        .gain       = {.Gain = 50, .Mute = 0},
        
        .eq        = {
                        .BandCoef = {
                            {.Freq = 100,  .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 1000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},

                            {.Freq = 1000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 2000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 3000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 4000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 5000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 6000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 7000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                        }
                      },

        .hpf        = {.Freq = 20, .Type = HLPF_Type_12dBButterworth},
        .lpf        = {.Freq = 20000, .Type = HLPF_Type_12dBButterworth},

        .noiseGate  = {.threshold = -60},
        
        .pitch      = {.PitchShift = 0},
        .input      = {.input = 0}, //0:vod 1:dvd 2:bgm 3:btusb 4:optical 5:hdmi-in 6:hdmi-arc
    },
        
    .mic = {
        .gain       = {.Gain = 50, .Mute = 0},

        .eq        = {
                        .BandCoef = {
                            {.Freq = 100,   .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 1000,  .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 10000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},

                            {.Freq = 2000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 3000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 4000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 5000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 6000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 7000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 8000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                        }
                      },

        .hpf        = {.Freq = 20, .Type = HLPF_Type_12dBButterworth},
        .lpf        = {.Freq = 20000, .Type = HLPF_Type_12dBButterworth},
        .feedback   = {.FeedBack = 2},      //ÒÆÆµÁ¿ 0:off,  1~3:4Hz/6Hz/8Hzs
        .limiter    = {.threshold = -3, .attackTime = 100, .releaseTime= 1000, .ratio = 10},
        .noiseGate  = {.threshold = -60},
    },
    
    .effGain        = {.Gain = 35, .Mute = 0},
    .echo = {
        .eq        = {
                        .BandCoef = {
                            {.Freq = 100, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 1000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 10000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                        }
                      },

        .repeat     = {.Vol = 55, .Phase = 0},
        .effVol     = {.Vol = 100, .Phase = 0},
        .dirVol     = {.Vol = 15, .Phase = 0},


        .hpf        = {.Freq = 20, .Type = HLPF_Type_12dBButterworth},
        .lpf        = {.Freq = 20000, .Type = HLPF_Type_12dBButterworth},
        .preDelay   = {.Delay = 2},
        .delay      = {.Delay = 235},
    },
    
    .reverb = {
        .eq        = {
                        .BandCoef = {
                            {.Freq = 100, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 1000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 10000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                        }
                      },
        
        .effVol     = {.Vol = 100, .Phase = 0},
        .dirVol     = {.Vol = 15, .Phase = 0},

        .hpf        = {.Freq = 20, .Type = HLPF_Type_12dBButterworth},
        .lpf        = {.Freq = 20000, .Type = HLPF_Type_12dBButterworth},
        .preDelay   = {.Delay = 12},
        .time       = {.Delay = 4300},
    },

    .main = {
        .eq        = {
                        .BandCoef = {
                            {.Freq = 100,  .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 1000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 10000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 2000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 3000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 4000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 5000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                        }
                      },

        .hpf        = {.Freq = 20, .Type = HLPF_Type_12dBButterworth},
        .lpf        = {.Freq = 20000, .Type = HLPF_Type_12dBButterworth},

        .musicVol   = {.Vol = 100, .Phase = 0},
        .dirVol     = {.Vol = 100, .Phase = 0},
        .echoVol    = {.Vol = 100, .Phase = 0},
        .reverbVol  = {.Vol = 50,  .Phase = 0},

        .lVol       = {.Vol = 100, .Phase = 0},
        .rVol       = {.Vol = 100, .Phase = 0},


        .lDelay     = {.Delay = 0},
        .rDelay     = {.Delay = 0},
        .limiter    = {.threshold = -5, .attackTime = 100, .releaseTime = 1000, .ratio = 40},
        .L          = {.Mute = 0},
        .R          = {.Mute = 0},
    },

    .sub = {
        .eq        = {
                        .BandCoef = {
                            {.Freq = 100,  .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 1000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 10000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 2000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 3000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 4000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 5000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                        }
                      },

        .hpf        = {.Freq = 23, .Type = HLPF_Type_12dBButterworth},
        .lpf        = {.Freq = 105, .Type = HLPF_Type_12dBButterworth},
            
        .delay     = {.Delay = 0},

        .musicVol   = {.Vol = 100, .Phase = 0},
        .dirVol     = {.Vol = 0, .Phase = 0},
        .echoVol    = {.Vol = 0, .Phase = 0},
        .reverbVol  = {.Vol = 0, .Phase = 0},
        .vol        = {.Vol = 100, .Phase = 0},

        .limiter    = {.threshold = -2, .attackTime = 100, .releaseTime = 1000, .ratio = 20},
        .mute       = {.Mute = 0},
    },

    .rec = {
        .eq        = {
                        .BandCoef = {
                            {.Freq = 100,  .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 1000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 10000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 2000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 3000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 4000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                            {.Freq = 5000, .Gain = 0, .Q = 22, .Type = EQ_Type_PEQ, .Bypass = 0},
                        }
                      },

        .hpf        = {.Freq = 20, .Type = HLPF_Type_12dBButterworth},
        .lpf        = {.Freq = 20000, .Type = HLPF_Type_12dBButterworth},

        .musicVol   = {.Vol = 100, .Phase = 0},
        .dirVol     = {.Vol = 100, .Phase = 0},
        .echoVol    = {.Vol = 100, .Phase = 0},
        .reverbVol  = {.Vol = 100, .Phase = 0},
        .vol        = {.Vol = 100, .Phase = 0},

        .limiter    = {.threshold = -2, .attackTime = 100, .releaseTime = 1000, .ratio = 20},
        .mute       = {.Mute = 0},
    },
};
/////////////////////////////////////////////////////////////////////////


const para_txt_t HLPF_TXT={
    {
        "BYPASS",
        "12-BUTT",     //"12dB BUTTERWORTH",
        "12-BESS",     //"12dB BESSEL",
        "12-LINK",     //"12dB LINKRILEY",
        "18-BUTT",     //"18dB BUTTERWORTH",
        "18-BESS",     //"18dB BESSEL",
        "24-BUTT",     //"24dB BUTTERWORTH",
        "24-BESS",     //"24dB BESSEL",
        "24-LINK",     //"24dB LINKRILEY",
    }
};
para_txt_t PITCH_TXT={
    {
        "b5",
        "b4",
        "b3",
        "b2",
        "b1",
        "0",
        "#1",
        "#2",
        "#3",
        "#4",
        "#5",
    }
};
para_txt_t FEEDBACK_TXT={
    {
        "OFF",
        "4HZ",
        "6HZ",
        "8HZ",
    }
};
para_txt_t INPUT_TXT={
    {
        "VOD",
        "DVD",
        "BGM",
        "BTUSB",
        "OPT",
        "HIN",
        "HARC",
    }
};
para_txt_t EQTYPE_TXT={
    {
        "PEQ",
        "LS",
        "HS",
    }
};
para_txt_t ONOFF_TXT={
    {
        "OFF",
        "ON",
    }
};
para_txt_t PHASE_TXT={
    {
        "+",
        "-",
    }
};


u32 parainfo_length=sizeof(PARA_INFO);
const paras_info_t PARA_INFO[CMD_ID_NUM]={
    {//0
        {
            {"",                    "",                1,                   1,                  1,                  1,              0,              NULL},
        }
    },

    {//CMD_ID_Gain
        {
            {"GAIN",                "dB",               0,                  100,                1,                  1,              0,              NULL},
            {"MUTE",                "",                 0,                  1,                  1,                  1,              0,              &ONOFF_TXT},
            {"",                    "",                 1,                  1,                  1,                  1,              0,              NULL},
        }
    },

    {//CMD_ID_Vol
        {
            {"VOL",                 "",                 0,                  200,                1,                  1,              0,              NULL},
            {"PHASE",               "",                 0,                  1,                  1,                  1,              0,              &PHASE_TXT},
            {"",                    "",                 1,                  1,                  1,                  1,              0,              NULL},
        }
    },

    {//CMD_ID_EQ
        {
            {"FREQ",                "Hz",               20,                 20000,              1,                  1,              0,              NULL},
            {"GAIN",                "dB",               -24,                12,                 0.1,                1,              0,              NULL},
            {"Q",                   "",                 1,                  1280,               1,                  10,             1,              NULL},
            {"TYPE",                "",                 0,                  2,                  1,                  1,              0,              &EQTYPE_TXT},
            {"BYPASS",              "",                 0,                  1,                  1,                  1,              0,              NULL},
            {"",                    "",                 1,                  1,                  1,                  1,              0,              NULL},
        }
    },

    {//CMD_ID_HLPF
        {
            {"FREQ",                "",                 20,                 21000,              1,                  1,              0,              NULL},
            {"TYPE",                "",                 HLPF_Type_Bypass,   HLPF_Type_24dBLinkRiley, 1,             1,              0,              &HLPF_TXT},
            {"",                    "",                 1,                  1,                  1,                  1,              0,              NULL},
        }
    },

    {//CMD_ID_Delay
        {
            {"ECHO PREDELAY",       "ms",               0,                  4800,               48,                 48,              0,             NULL},
            {"ECHO DELAY",          "ms",               0,                  14400,              48,                 48,              0,             NULL},
            {"REVERB PREDELAY",     "ms",               0,                  4800,               48,                 48,              0,             NULL},
            {"REVERB DELAY",        "ms",               0,                  8000,               1,                  1,               0,             NULL},
            {"MAINL DELAY",         "ms",               0,                  1920,               48,                 48,              0,             NULL},
            {"MAINR DELAY",         "ms",               0,                  1920,               48,                 48,              0,             NULL},
            {"SUB DELAY",           "ms",               0,                  1920,               48,                 48,              0,             NULL},
            {"",                    "",                 1,                  1,                  1,                  1,               0,             NULL},
        }
    },

    {//CMD_ID_FeedBack
        {
            {"FEEDBACK",            "",                 0,                  3,                  1,                  1,               0,             &FEEDBACK_TXT},
            {"",                    "",                 1,                  1,                  1,                  1,               0,             NULL},
        }
    },

    {//CMD_ID_Limiter
        {
            {"THRESHOLD",           "dB",               -34,                0,                  1,                  1,               0,             NULL},
            {"ATTACKTIME",          "ms",               0,                  900,                1,                  1,               0,             NULL},
            {"RELEASETIME",         "ms",               0,                  9000,               1,                  1,               0,             NULL},
            {"RATIO",               "ms",               0,                  1000,               1,                  10,              1,             NULL},
            {"",                    "",                 1,                  1,                  1,                  1,               0,             NULL},
        }
    },

    {//CMD_ID_PitchShift
        {
            {"PITCH",               "",                 -5,                 5,                  1,                  1,               0,             &PITCH_TXT},
            {"",                    "",                 1,                  1,                  1,                  1,               0,             NULL},
        }
    },

    {//CMD_ID_Mute
        {
            {"MUTE",                "",                 0,                  1,                  1,                  1,               0,             &ONOFF_TXT},
            {"",                    "",                 1,                  1,                  1,                  1,               0,             NULL},
        }
    },

    {//CMD_ID_NoiseGate
        {
            {"THRESHOLD",           "dB",               -60,                0,                  1,                  1,               0,             NULL},
            {"",                    "",                 1,                  1,                  1,                  1,               0,             NULL},
        }
    },

    {//CMD_ID_Input
        {
            {"INPUT",               "",                 INPUT_VOD,          INPUT_HDMI_ARC,     1,                  1,               0,             &INPUT_TXT},
            {"",                    "",                 1,                  1,                  1,                  1,               0,             NULL},
        }
    },
};


const fw_info_t FW_INFO = {
    .mode = 0,
    .model = MODEL_NAME,
    .boot = "",
    .mcu = MCU_VERSION,
    .dsp = "",
    //.bldtime = __DATE__,
};

const io_data_t IO_DATA = {
    .amp_en = 1,
    .pwr_en = 1,
    .rca_mute = 0,
    .input = 0,
    
    .amp_ppwr = 1,
    .amp_npwr = 0,
    .rx485_en = 0,
    .tx485_en = 0,
};



