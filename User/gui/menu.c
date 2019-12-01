#include "menu.h"
#include "dsp.h"

u8 gM=MENU_HOME;
menu_func_t gFuncs[MENU_MAX]={
    MAKE_FUNC(home),
    MAKE_FUNC(mic),
    MAKE_FUNC(music),
    MAKE_FUNC(effect),
    MAKE_FUNC(presets),
};


const dsp_item_t dspItems[CMD_ID_NUM]={
    {//0
        {
            {CONTROL_NONE,              "",                0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Gain
        {
            {CONTROL_INPUTBOX,          "GAIN",             0,              NULL,           NULL},
            {CONTROL_INPUTBOX,          "MUTE",             0,              NULL,           NULL},
            {CONTROL_NONE,              "",                 0,              NULL,           NULL},
        }
    },

    {//CMD_ID_Vol
        {
            {CONTROL_INPUTBOX,          "VOL",              0,              NULL,           NULL},
            {CONTROL_INPUTBOX,          "PHASE",            0,              NULL,           NULL},
            {CONTROL_NONE,              "",                 0,              NULL,           NULL},
        }
    },

    {//CMD_ID_EQ
        {
            {CONTROL_INPUTBOX,          "FREQ",             0,              NULL,           NULL},
            {CONTROL_INPUTBOX,          "GAIN",             0,              NULL,           NULL},
            {CONTROL_INPUTBOX,          "Q",                0,              NULL,           NULL},
            {CONTROL_INPUTBOX,          "TYPE",             0,              NULL,           NULL},
            {CONTROL_INPUTBOX,          "BYPASS",           0,              NULL,           NULL},
            {CONTROL_NONE,              "",                 0,              NULL,           NULL},
        }
    },

    {//CMD_ID_HLPF
        {
            {CONTROL_INPUTBOX,          "FREQ",            0,               NULL,           NULL},
            {CONTROL_INPUTBOX,          "TYPE",            0,               NULL,           NULL},
            {CONTROL_NONE,              "",                0,               NULL,           NULL},
        }
    },

    {//CMD_ID_Delay
        {
            {CONTROL_INPUTBOX,          "ECHO PREDELAY",      0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "ECHO DELAY",         0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "REVERB PREDELAY",    0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "REVERB DELAY",       0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "MAINL DELAY",        0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "MAINR DELAY",        0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "SUB DELAY",          0,            NULL,           NULL},
            {CONTROL_NONE,              "",                   0,            NULL,           NULL},
        }
    },

    {//CMD_ID_FeedBack
        {
            {CONTROL_INPUTBOX,          "FEEDBACK",           0,            NULL,           NULL},
            {CONTROL_NONE,              "",                   0,            NULL,           NULL},
        }
    },

    {//CMD_ID_Limiter
        {
            {CONTROL_INPUTBOX,          "THRESHOLD",          0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "ATTACKTIME",         0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "RELEASETIME",        0,            NULL,           NULL},
            {CONTROL_INPUTBOX,          "RATIO",              0,            NULL,           NULL},
            {CONTROL_NONE,              "",                   0,            NULL,           NULL},
        }
    },

    {//CMD_ID_PitchShift
        {
            {CONTROL_INPUTBOX,          "PITCH",              0,            NULL,           NULL},
            {CONTROL_NONE,              "",                   0,            NULL,           NULL},
        }
    },

    {//CMD_ID_Mute
        {
            {CONTROL_INPUTBOX,          "MUTE",               0,            NULL,           NULL},
            {CONTROL_NONE,              "",                   0,            NULL,           NULL},
        }
    },

    {//CMD_ID_NoiseGate
        {
            {CONTROL_INPUTBOX,          "THRESHOLD",          0,            NULL,           NULL},
            {CONTROL_NONE,              "",                   0,            NULL,           NULL},
        }
    },

    {//CMD_ID_Input
        {
            {CONTROL_INPUTBOX,          "INPUT",              0,            NULL,           NULL},
            {CONTROL_NONE,              "",                   0,            NULL,           NULL},
        }
    },
};


#define DSP     uiParams.dsp
#define MSC     DSP.music
#define MIC     DSP.mic
#define ECHO    DSP.echo
#define REV     DSP.reverb
const item_data_t mscPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &MSC.peq[0],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &MSC.peq[1],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &MSC.peq[2],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ4",        CMD_ID_EQ,        &MSC.peq[3],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ5",        CMD_ID_EQ,        &MSC.peq[4],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ6",        CMD_ID_EQ,        &MSC.peq[5],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ7",        CMD_ID_EQ,        &MSC.peq[6],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t mscGeqItems[]={
    {CONTROL_LIST,     "GEQ1",        CMD_ID_EQ,        &MSC.geq[0],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "GEQ2",        CMD_ID_EQ,        &MSC.geq[1],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t mscItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      &MSC.gain,          (void*)&dspItems[CMD_ID_Gain]},
    {CONTROL_LIST,     "GEQ",         0,                &MSC.geq,           (void*)mscGeqItems},
    {CONTROL_LIST,     "PEQ",         0,                &MSC.peq,           (void*)mscPeqItems},
    {CONTROL_LIST,     "PITCH",       CMD_ID_PitchShift,&MSC.pitch,         (void*)&dspItems[CMD_ID_PitchShift]},
    {CONTROL_LIST,     "INPUT",       CMD_ID_Input,     &MSC.input,         (void*)&dspItems[CMD_ID_Input]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &MSC.hpf,           (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &MSC.lpf,           (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, &MSC.noiseGate,     (void*)&dspItems[CMD_ID_NoiseGate]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
////////////////////////////////////////////////////////////////////////////////
const item_data_t micPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &MIC.peq[0],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &MIC.peq[1],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &MIC.peq[2],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ4",        CMD_ID_EQ,        &MIC.peq[3],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ5",        CMD_ID_EQ,        &MIC.peq[4],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ6",        CMD_ID_EQ,        &MIC.peq[5],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ7",        CMD_ID_EQ,        &MIC.peq[6],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t micGeqItems[]={
    {CONTROL_LIST,     "GEQ1",        CMD_ID_EQ,        &MIC.geq[0],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "GEQ2",        CMD_ID_EQ,        &MIC.geq[1],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "GEQ3",        CMD_ID_EQ,        &MIC.geq[2],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t micItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      &MIC.gain,          (void*)&dspItems[CMD_ID_Gain]},
    {CONTROL_LIST,     "GEQ",         0,                &MIC.geq,           (void*)micGeqItems},
    {CONTROL_LIST,     "PEQ",         0,                &MIC.peq,           (void*)micPeqItems},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &MIC.hpf,           (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &MIC.lpf,           (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "FEEDBACK",    CMD_ID_FeedBack,  &MIC.feedback,      (void*)&dspItems[CMD_ID_FeedBack]},
    {CONTROL_LIST,     "LIMITER",     CMD_ID_Limiter,   &MIC.limiter,       (void*)&dspItems[CMD_ID_Limiter]},
    {CONTROL_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, &MIC.noiseGate,     (void*)&dspItems[CMD_ID_NoiseGate]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
////////////////////////////////////////////////////////////////////////////
const item_data_t echoPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &ECHO.peq[0],       (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &ECHO.peq[1],       (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &ECHO.peq[2],       (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t echoItems[]={
    {CONTROL_LIST,     "PEQ",         0,                &ECHO.peq,          (void*)echoPeqItems},
    {CONTROL_LIST,     "REPEAT",      CMD_ID_Delay,     &ECHO.repeat,       (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_LIST,     "EFFECT VOL",  CMD_ID_Delay,     &ECHO.effVol,       (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_LIST,     "DIRECT VOL",  CMD_ID_Delay,     &ECHO.dirVol,       (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &ECHO.hpf,          (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &ECHO.lpf,          (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "PREDELAY",    CMD_ID_Delay,     &ECHO.preDelay,     (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_LIST,     "DELAY",       CMD_ID_Delay,     &ECHO.delay,        (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};

const item_data_t revPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &REV.peq[0],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &REV.peq[1],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &REV.peq[2],        (void*)&dspItems[CMD_ID_EQ]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_data_t reverbItems[]={
    {CONTROL_LIST,     "PEQ",         0,                &REV.peq,           (void*)echoPeqItems},
    {CONTROL_LIST,     "EFFECT VOL",  CMD_ID_Delay,     &REV.effVol,        (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_LIST,     "DIRECT VOL",  CMD_ID_Delay,     &REV.dirVol,        (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &REV.hpf,           (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &REV.lpf,           (void*)&dspItems[CMD_ID_HLPF]},
    {CONTROL_LIST,     "PREDELAY",    CMD_ID_Delay,     &REV.preDelay,      (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_LIST,     "TIME",        CMD_ID_Delay,     &REV.time,          (void*)&dspItems[CMD_ID_Delay]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
item_data_t effItems[]={
    {CONTROL_LIST,     "GAIN",        0,                &DSP.effGain,       (void*)&dspItems[CMD_ID_Gain]},
    {CONTROL_LIST,     "ECHO",        0,                &ECHO,              (void*)echoItems},
    {CONTROL_LIST,     "REVERB",      0,                &REV,               (void*)reverbItems},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};

///////////////////////////////////////////////////////////////////////////////


int menu_init(void)
{
    gFuncs[gM].init();
    return 0;
}


int menu_free(void)
{
    gFuncs[gM].free();
    return 0;
}


int menu_refresh(void)
{
    gFuncs[gM].refresh();
    return 0;
}


int menu_clear(void)
{
    rect_t r=MENU_RECT;
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    return 0;
}


int menu_handle(u8 key)
{
    gFuncs[gM].handle(key);
    return 0;
}

static u8 get_item_num(item_data_t *item)
{
    u8 i=0;
    while(item[i].control!=CONTROL_NONE) i++;
    return i;
}
int menu_add_item(listitem_t *l, u8 menu)
{
    u8 i,j,num;
    node_t n;
    item_data_t e;
    rect_t rect=MENU_RECT;

    if(!l) {
        return -1;
    }

    for(i=0;;i++) {
        e = mscItems[i];
        if(e.control==CONTROL_LIST) {
            num = get_item_num(e.item);
            listitem_t *pl=listitem_init(e.txt, &rect, num, sizeof(e));
        }
        else if(e.control==CONTROL_INPUTBOX){
            inputbox_t *b;
            //num = get_item_num();
            //b = inputbox_init(e.txt, &rect, 6, sizeof(e));
        }
        else {
            break;
        }

        n.ptr = &e;
        n.len = sizeof(e);
        listitem_add(l, &n);
    }

    return 0;
}















