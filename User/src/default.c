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

const para_info_t PARA_INFO[CMD_ID_NUM]={
    {//0
        {
            {"",                    "",                1,                   1,                 1,                  1},
        }
    },

    {//CMD_ID_Gain
        {
            {"GAIN",                "",                 1,                  1000,               1,                  1},
            {"MUTE",                "",                 0,                  1,                  1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_Vol
        {
            {"VOL",                 "",                 0,                  200,                1,                  1},
            {"PHASE",               "",                 0,                  1,                  1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_EQ
        {
            {"FREQ",                "Hz",               20,                 20000,              1,                  1},
            {"GAIN",                "dB",               -24,                12,                 0.1,                1},
            {"Q",                   "",                 1,                  1280,               1,                  10},
            {"TYPE",                "",                 0,                  2,                  1,                  1},
            {"BYPASS",              "",                 0,                  1,                  1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_HLPF
        {
            {"FREQ",                "",                 20,                 21000,              1,                  1},
            {"TYPE",                "",                 HLPF_Type_Bypass,   HLPF_Type_24dBLinkRiley, 1,             1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_Delay
        {
            {"ECHO PREDELAY",       "ms",               0,                  4800,               48,                 48},
            {"ECHO DELAY",          "ms",               0,                  14400,              48,                 48},
            {"REVERB PREDELAY",     "ms",               0,                  4800,               48,                 48},
            {"REVERB DELAY",        "ms",               0,                  8000,               1,                  1},
            {"MAINL DELAY",         "ms",               0,                  1920,               48,                 48},
            {"MAINR DELAY",         "ms",               0,                  1920,               48,                 48},
            {"SUB DELAY",           "ms",               0,                  1920,               48,                 48},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_FeedBack
        {
            {"FEEDBACK",            "",                 0,                  3,                  1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_Limiter
        {
            {"THRESHOLD",           "dB",               -34,                0,                  1,                  1},
            {"ATTACKTIME",          "ms",               0,                  900,                1,                  1},
            {"RELEASETIME",         "ms",               0,                  9000,               1,                  1},
            {"RATIO",               "ms",               0,                  1000,               1,                  10},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_PitchShift
        {
            {"PITCH",               "",                 -5,                 5,                  1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_Mute
        {
            {"MUTE",                "",                 0,                  1,                  1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_NoiseGate
        {
            {"THRESHOLD",           "dB",               -60,                0,                  1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
        }
    },

    {//CMD_ID_Input
        {
            {"INPUT",               "",                 INPUT_VOD,          INPUT_HDMI_ARC,     1,                  1},
            {"",                    "",                 1,                  1,                  1,                  1},
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

cchr *HLPF_STR[HLPF_Type_NUM] = {
    "BYPASS",
    "12dB BUTTERWORTH",
    "12dB BESSEL",
    "12dB LINKRILEY",
    "18dB BUTTERWORTH",
    "18dB BESSEL",
    "24dB BUTTERWORTH",
    "24dB BESSEL",
    "24dB LINKRILEY",
};


