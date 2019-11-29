#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#include "types.h"
#include "packet.h"
#include "dsp.h"

//#pragma pack(1)
typedef struct {
    TypeS_Gain          gain;
    TypeS_EQ            eq;

    TypeS_HLPFCoef      hpf;
    TypeS_HLPFCoef      lpf;
    TypeS_NoiseGate     noiseGate;

    TypeS_PitchShift    pitch;
    TypeS_Input         input;
}def_music_t;

typedef struct {
    TypeS_Gain          gain;
    TypeS_EQ            eq;

    TypeS_HLPFCoef      hpf;
    TypeS_HLPFCoef      lpf;
    TypeS_FeedBack      feedback;
    TypeS_Limiter       limiter;
    TypeS_NoiseGate     noiseGate;
}def_mic_t;
typedef struct {
    TypeS_EQ            eq;

    TypeS_Vol           repeat;
    TypeS_Vol           effVol;
    TypeS_Vol           dirVol;

    TypeS_HLPFCoef      hpf;
    TypeS_HLPFCoef      lpf;

    //以下step=1
    TypeS_Delay         preDelay;      //0~4800 :    0~100ms
    TypeS_Delay         delay;         //0~14400 :    0~300ms
}def_echo_t;

typedef struct {
    TypeS_EQ            eq;

    TypeS_Vol           effVol;
    TypeS_Vol           dirVol;

    TypeS_HLPFCoef      hpf;
    TypeS_HLPFCoef      lpf;

    TypeS_Delay         preDelay;       //混响预延时 //0~4800 :    0~100ms
    TypeS_Delay         time;           //混响时间0~80 : 0~8000ms
}def_reverb_t;

typedef struct {
    TypeS_EQ            eq;

    TypeS_HLPFCoef      hpf;
    TypeS_HLPFCoef      lpf;

    TypeS_Delay         delay;          //0~1920 :    0~40ms

    TypeS_Vol           musicVol;
    TypeS_Vol           dirVol;
    TypeS_Vol           echoVol;
    TypeS_Vol           reverbVol;
    TypeS_Vol           vol;

    TypeS_Limiter       limiter;
    TypeS_Mute          mute;
}def_sub_t;

typedef struct {
    TypeS_EQ            eq;

    TypeS_HLPFCoef      hpf;
    TypeS_HLPFCoef      lpf;

    TypeS_Vol           musicVol;
    TypeS_Vol           dirVol;
    TypeS_Vol           echoVol;
    TypeS_Vol           reverbVol;
    TypeS_Vol           vol;

    TypeS_Limiter       limiter;
    TypeS_Mute          mute;
}def_rec_t;

typedef struct {
    TypeS_EQ            eq;

    TypeS_HLPFCoef      hpf;
    TypeS_HLPFCoef      lpf;

    TypeS_Vol           musicVol;
    TypeS_Vol           dirVol;
    TypeS_Vol           echoVol;
    TypeS_Vol           reverbVol;

    TypeS_Vol           lVol;
    TypeS_Vol           rVol;

    //以下step=0.1
    TypeS_Delay         lDelay;         //主输出左声道 //0~1920 :    0~40ms
    TypeS_Delay         rDelay;         //0~1920 :    0~40ms

    TypeS_Limiter       limiter;
    TypeS_Mute          L;
    TypeS_Mute          R;
}def_main_t;

typedef struct
{
    def_music_t         music;
    def_mic_t           mic;

    TypeS_Gain          effGain;
    def_echo_t          echo;
    def_reverb_t        reverb;
    
    def_sub_t           sub;
    def_rec_t           rec;
    def_main_t          main;
}def_dsp_t;

#define VAR_MAX        8
typedef struct {
    cchr                *name;
    cchr                *unit;
    s16                 min;
    s16                 max;
    f32                 step;
    f32                 div;        //show number==value/div
}para_item_t;


typedef struct {
    para_item_t         items[VAR_MAX];
}para_info_t;
//#pragma pack()


extern const def_dsp_t DEF_DSP;
extern const fw_info_t FW_INFO;
extern const io_data_t IO_DATA;
extern const para_info_t PARA_INFO[CMD_ID_NUM];
extern cchr *HLPF_STR[HLPF_Type_NUM];

#endif
