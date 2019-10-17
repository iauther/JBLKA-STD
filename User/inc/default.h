#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#include "types.h"
#include "packet.h"
#include "dsp.h"

//#pragma pack(1)
typedef struct
{
    TypeS_Gain          gain;
    TypeS_Vol           vol;
    TypeS_EQBand        eq;
    TypeS_HLPFCoef      hlpf;
    TypeS_Delay         delay;
    TypeS_FeedBack      feedback;
    TypeS_Limiter       limiter;
    TypeS_PitchShift    pitch;
    TypeS_Mute          mute;
    TypeS_NoiseGate     noisegate;
}default_t;


typedef struct {
    const char          *name;
    const char          *unit;
    s32                 min;
    s32                 max;
    f32                 step;
    f32                 div;        //show number==value/div
}para_info_t;
//#pragma pack()

extern const default_t gDefault;
extern const fw_info_t FW_INFO;
extern const io_data_t IO_DATA;

extern const u16 GEQ2_FREQ[];
extern const u16 GEQ3_FREQ[];
extern const u16 PEQ3_FREQ[];
extern const u16 PEQ7_FREQ[];

#endif
