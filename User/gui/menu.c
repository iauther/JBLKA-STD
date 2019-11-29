#include "menu.h"


u8 gMenu=MENU_HOME;
u8 gMenuPrev=MENU_HOME;
#define INIT_FUNC(XX) \
    {\
        XX##_init, \
        XX##_free, \
        XX##_handle, \
        XX##_refresh, \
    }

func_t fn_funcs[MENU_MAX]={
    INIT_FUNC(home),
    INIT_FUNC(mic),
    INIT_FUNC(music),
    INIT_FUNC(effect),
    INIT_FUNC(presets),
};
/////////////////////////////////////////
func_t ocr_funcs[OCR_MAX]={
    NULL,
    NULL,
    INIT_FUNC(inputbox),
    INIT_FUNC(listitem),
};

int menu_init(void)
{
    fn_funcs[gMenu].init(NULL);
    
    return 0;
}


int menu_refresh(void)
{
    if(gMenu!=gMenuPrev) {
        fn_funcs[gMenuPrev].free(NULL);
        fn_funcs[gMenu].init(NULL);
    }

    fn_funcs[gMenu].refresh(NULL);
    return 0;
}


int menu_handle(u8 key)
{
    fn_funcs[gMenu].handle(NULL, key);
    
    return 0;
}

#include "dsp.h"
int menu_clear(void)
{
    rect_t r=MENU_RECT;
    
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    return 0;
}


const dsp_item_t dspItems[CMD_ID_NUM]={
    {//0
        {
            {TYPE_NONE,            "",                0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Gain
        {
            {TYPE_DATA,         "GAIN",             0,              NULL,           NULL},
            {TYPE_DATA,         "MUTE",             0,              NULL,           NULL},
            {TYPE_NONE,         "",                 0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Vol
        {
            {TYPE_DATA,         "VOL",              0,              NULL,           NULL},
            {TYPE_DATA,         "PHASE",            0,              NULL,           NULL},
            {TYPE_NONE,         "",                 0,              NULL,           NULL},
        }
    },

    {//CMD_ID_EQ
        {
            {TYPE_DATA,         "FREQ",             0,              NULL,           NULL},
            {TYPE_DATA,         "GAIN",             0,              NULL,           NULL},
            {TYPE_DATA,         "Q",                0,              NULL,           NULL},
            {TYPE_DATA,         "TYPE",             0,              NULL,           NULL},
            {TYPE_DATA,         "BYPASS",           0,              NULL,           NULL},
            {TYPE_NONE,         "",                 0,              NULL,           NULL},
        }
    },

    {//CMD_ID_HLPF
        {
            {TYPE_DATA,          "FREQ",            0,              NULL,           NULL},
            {TYPE_DATA,          "TYPE",            0,              NULL,           NULL},
            {TYPE_NONE,          "",                0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Delay
        {
            {TYPE_DATA,       "ECHO PREDELAY",      0,              NULL,           NULL},
            {TYPE_DATA,       "ECHO DELAY",         0,              NULL,           NULL},
            {TYPE_DATA,       "REVERB PREDELAY",    0,              NULL,           NULL},
            {TYPE_DATA,       "REVERB DELAY",       0,              NULL,           NULL},
            {TYPE_DATA,       "MAINL DELAY",        0,              NULL,           NULL},
            {TYPE_DATA,       "MAINR DELAY",        0,              NULL,           NULL},
            {TYPE_DATA,       "SUB DELAY",          0,              NULL,           NULL},
            {TYPE_NONE,       "",                   0,              NULL,           NULL},
        }
    },

    {//CMD_ID_FeedBack
        {
            {TYPE_DATA,       "FEEDBACK",           0,              NULL,           NULL},
            {TYPE_NONE,       "",                   0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Limiter
        {
            {TYPE_DATA,       "THRESHOLD",          0,              NULL,           NULL},
            {TYPE_DATA,       "ATTACKTIME",         0,              NULL,           NULL},
            {TYPE_DATA,       "RELEASETIME",        0,              NULL,           NULL},
            {TYPE_DATA,       "RATIO",              0,              NULL,           NULL},
            {TYPE_NONE,       "",                   0,              NULL,           NULL},
        }
    },

    {//CMD_ID_PitchShift
        {
            {TYPE_DATA,       "PITCH",              0,              NULL,           NULL},
            {TYPE_NONE,       "",                   0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Mute
        {
            {TYPE_DATA,       "MUTE",               0,              NULL,           NULL},
            {TYPE_NONE,       "",                   0,              NULL,           NULL},
        }
    },

    {//CMD_ID_NoiseGate
        {
            {TYPE_DATA,       "THRESHOLD",          0,              NULL,           NULL},
            {TYPE_NONE,       "",                   0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Input
        {
            {TYPE_DATA,       "INPUT",              0,              NULL,           NULL},
            {TYPE_NONE,       "",                   0,              NULL,           NULL},
        }
    },
};


#define DSP     uiParams.dsp
#define MSC     DSP.music
#define MIC     DSP.mic
#define ECHO    DSP.echo
#define REV     DSP.reverb
const item_data_t mscPeqItems[]={
    {TYPE_LIST,     "PEQ1",        CMD_ID_EQ,        &MSC.peq[0],       NULL},
    {TYPE_LIST,     "PEQ2",        CMD_ID_EQ,        &MSC.peq[1],       NULL},
    {TYPE_LIST,     "PEQ3",        CMD_ID_EQ,        &MSC.peq[2],       NULL},
    {TYPE_LIST,     "PEQ4",        CMD_ID_EQ,        &MSC.peq[3],       NULL},
    {TYPE_LIST,     "PEQ5",        CMD_ID_EQ,        &MSC.peq[4],       NULL},
    {TYPE_LIST,     "PEQ6",        CMD_ID_EQ,        &MSC.peq[5],       NULL},
    {TYPE_LIST,     "PEQ7",        CMD_ID_EQ,        &MSC.peq[6],       NULL},
    {TYPE_NONE,     "",            0,                NULL,              NULL},
};
const item_data_t mscGeqItems[]={
    {TYPE_LIST,     "GEQ1",        CMD_ID_EQ,        &MSC.geq[0],       NULL},
    {TYPE_LIST,     "GEQ2",        CMD_ID_EQ,        &MSC.geq[1],       NULL},
    {TYPE_NONE,     "",            0,                NULL,              NULL},
};
const item_data_t mscItems[]={
    {TYPE_LIST,     "GAIN",        CMD_ID_Gain,      &MSC.gain,         NULL},
    {TYPE_LIST,     "GEQ",         0,                &MSC.geq,          NULL},
    {TYPE_LIST,     "PEQ",         0,                &MSC.peq,          NULL},
    {TYPE_LIST,     "PITCH",       CMD_ID_PitchShift,&MSC.pitch,        NULL},
    {TYPE_LIST,     "INPUT",       CMD_ID_Input,     &MSC.input,        NULL},
    {TYPE_LIST,     "HPF",         CMD_ID_HLPF,      &MSC.hpf,          NULL},
    {TYPE_LIST,     "LPF",         CMD_ID_HLPF,      &MSC.lpf,          NULL},
    {TYPE_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, &MSC.noiseGate,    NULL},
    {TYPE_NONE,     "",            0,                NULL,              NULL},
};
////////////////////////////////////////////////////////////////////////////////
const item_data_t micPeqItems[]={
    {TYPE_LIST,     "PEQ1",        CMD_ID_EQ,        &MIC.peq[0],        NULL},
    {TYPE_LIST,     "PEQ2",        CMD_ID_EQ,        &MIC.peq[1],        NULL},
    {TYPE_LIST,     "PEQ3",        CMD_ID_EQ,        &MIC.peq[2],        NULL},
    {TYPE_LIST,     "PEQ4",        CMD_ID_EQ,        &MIC.peq[3],        NULL},
    {TYPE_LIST,     "PEQ5",        CMD_ID_EQ,        &MIC.peq[4],        NULL},
    {TYPE_LIST,     "PEQ6",        CMD_ID_EQ,        &MIC.peq[5],        NULL},
    {TYPE_LIST,     "PEQ7",        CMD_ID_EQ,        &MIC.peq[6],        NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t micGeqItems[]={
    {TYPE_LIST,     "GEQ1",        CMD_ID_EQ,        &MIC.geq[0],        NULL},
    {TYPE_LIST,     "GEQ2",        CMD_ID_EQ,        &MIC.geq[1],        NULL},
    {TYPE_LIST,     "GEQ3",        CMD_ID_EQ,        &MIC.geq[2],        NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t micItems[]={
    {TYPE_LIST,     "GAIN",        CMD_ID_Gain,      &MIC.gain,          NULL},
    {TYPE_LIST,     "GEQ",         0,                &MIC.geq,           micPeqItems},
    {TYPE_LIST,     "PEQ",         0,                &MIC.peq,           NULL},
    {TYPE_LIST,     "HPF",         CMD_ID_HLPF,      &MIC.hpf,           NULL},
    {TYPE_LIST,     "LPF",         CMD_ID_HLPF,      &MIC.lpf,           NULL},
    {TYPE_LIST,     "FEEDBACK",    CMD_ID_FeedBack,  &MIC.feedback,      NULL},
    {TYPE_LIST,     "LIMITER",     CMD_ID_HLPF,      &MIC.limiter,       NULL},
    {TYPE_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, &MIC.noiseGate,     NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};
////////////////////////////////////////////////////////////////////////////
const item_data_t echoPeqItems[]={
    {TYPE_LIST,     "PEQ1",        CMD_ID_EQ,        &ECHO.peq[0],       NULL},
    {TYPE_LIST,     "PEQ2",        CMD_ID_EQ,        &ECHO.peq[1],       NULL},
    {TYPE_LIST,     "PEQ3",        CMD_ID_EQ,        &ECHO.peq[2],       NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t echoItems[]={
    {TYPE_LIST,     "PEQ",         0,                &ECHO.peq,          NULL},
    {TYPE_LIST,     "REPEAT",      CMD_ID_Delay,     &ECHO.repeat,       NULL},
    {TYPE_LIST,     "EFFECT VOL",  CMD_ID_Delay,     &ECHO.effVol,       NULL},
    {TYPE_LIST,     "DIRECT VOL",  CMD_ID_Delay,     &ECHO.dirVol,       NULL},
    {TYPE_LIST,     "HPF",         CMD_ID_HLPF,      &ECHO.hpf,          NULL},
    {TYPE_LIST,     "LPF",         CMD_ID_HLPF,      &ECHO.lpf,          NULL},
    {TYPE_LIST,     "PREDELAY",    CMD_ID_Delay,     &ECHO.preDelay,     NULL},
    {TYPE_LIST,     "DELAY",       CMD_ID_Delay,     &ECHO.delay,        NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};

const item_data_t revPeqItems[]={
    {TYPE_LIST,     "PEQ1",        CMD_ID_EQ,        &REV.peq[0],        NULL},
    {TYPE_LIST,     "PEQ2",        CMD_ID_EQ,        &REV.peq[1],        NULL},
    {TYPE_LIST,     "PEQ3",        CMD_ID_EQ,        &REV.peq[2],        NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t reverbItems[]={
    {TYPE_LIST,     "PEQ",         0,                &REV.peq,           NULL},
    {TYPE_LIST,     "EFFECT VOL",  CMD_ID_Delay,     &REV.effVol,        NULL},
    {TYPE_LIST,     "DIRECT VOL",  CMD_ID_Delay,     &REV.dirVol,        NULL},
    {TYPE_LIST,     "HPF",         CMD_ID_HLPF,      &REV.hpf,           NULL},
    {TYPE_LIST,     "LPF",         CMD_ID_HLPF,      &REV.lpf,           NULL},
    {TYPE_LIST,     "PREDELAY",    CMD_ID_Delay,     &REV.preDelay,      NULL},
    {TYPE_LIST,     "TIME",        CMD_ID_Delay,     &REV.time,          NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};
item_data_t effItems[]={
    {TYPE_LIST,     "GAIN",        0,                &DSP.effGain,       NULL},
    {TYPE_LIST,     "ECHO",        0,                &ECHO,              NULL},
    {TYPE_LIST,     "REVERB",      0,                &REV,               NULL},
    {TYPE_NONE,     "",            0,                NULL,               NULL},
};



















