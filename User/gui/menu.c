#include "menu.h"
#include "dsp.h"

CONST dsp_var_t dspItems[CMD_ID_NUM]={
    {//0
        {
            {CONTROL_NONE,              "",                  0,                         NULL,           NULL},
        }
    },

    {//CMD_ID_Gain
        {
            {CONTROL_INPUTBOX,          "GAIN",              CMD_ID_Gain,               NULL,           NULL},
            {CONTROL_INPUTBOX,          "MUTE",              CMD_ID_Gain,               NULL,           NULL},
            {CONTROL_NONE,              "",                  0,                         NULL,           NULL},
        }
    },

    {//CMD_ID_Vol
        {
            {CONTROL_INPUTBOX,          "VOL",               CMD_ID_Vol,                NULL,           NULL},
            {CONTROL_INPUTBOX,          "PHASE",             CMD_ID_Vol,                NULL,           NULL},
            {CONTROL_NONE,              "",                  0,                         NULL,           NULL},
        }
    },

    {//CMD_ID_EQ
        {
            {CONTROL_INPUTBOX,          "FREQ",              CMD_ID_EQ,                 NULL,           NULL},
            {CONTROL_INPUTBOX,          "GAIN",              CMD_ID_EQ,                 NULL,           NULL},
            {CONTROL_INPUTBOX,          "Q",                 CMD_ID_EQ,                 NULL,           NULL},
            {CONTROL_INPUTBOX,          "TYPE",              CMD_ID_EQ,                 NULL,           NULL},
            {CONTROL_INPUTBOX,          "BYPASS",            CMD_ID_EQ,                 NULL,           NULL},
            {CONTROL_NONE,              "",                  0,                         NULL,           NULL},
        }
    },

    {//CMD_ID_HLPF
        {
            {CONTROL_INPUTBOX,          "FREQ",               CMD_ID_HLPF,              NULL,           NULL},
            {CONTROL_INPUTBOX,          "TYPE",               CMD_ID_HLPF,              NULL,           NULL},
            {CONTROL_NONE,              "",                   CMD_ID_HLPF,              NULL,           NULL},
        }
    },

    {//CMD_ID_Delay
        {
            {CONTROL_INPUTBOX,          "DELAY",              CMD_ID_Delay,             NULL,           NULL},
            {CONTROL_NONE,              "",                   0,                        NULL,           NULL},
        }
    },

    {//CMD_ID_FeedBack
        {
            {CONTROL_INPUTBOX,          "FEEDBACK",           CMD_ID_FeedBack,          NULL,           NULL},
            {CONTROL_NONE,              "",                   0,                        NULL,           NULL},
        }
    },

    {//CMD_ID_Limiter
        {
            {CONTROL_INPUTBOX,          "THRESHOLD",          CMD_ID_Limiter,           NULL,           NULL},
            {CONTROL_INPUTBOX,          "ATTACKTIME",         CMD_ID_Limiter,           NULL,           NULL},
            {CONTROL_INPUTBOX,          "RELEASETIME",        CMD_ID_Limiter,           NULL,           NULL},
            {CONTROL_INPUTBOX,          "RATIO",              CMD_ID_Limiter,           NULL,           NULL},
            {CONTROL_NONE,              "",                   0,                        NULL,           NULL},
        }
    },

    {//CMD_ID_PitchShift
        {
            {CONTROL_INPUTBOX,          "PITCH",              CMD_ID_PitchShift,        NULL,           NULL},
            {CONTROL_NONE,              "",                   0,                        NULL,           NULL},
        }
    },

    {//CMD_ID_Mute
        {
            {CONTROL_INPUTBOX,          "MUTE",               CMD_ID_Mute,              NULL,           NULL},
            {CONTROL_NONE,              "",                   0,                        NULL,           NULL},
        }
    },

    {//CMD_ID_NoiseGate
        {
            {CONTROL_INPUTBOX,          "THRESHOLD",          CMD_ID_NoiseGate,         NULL,           NULL},
            {CONTROL_NONE,              "",                   0,                        NULL,           NULL},
        }
    },

    {//CMD_ID_Input
        {
            {CONTROL_INPUTBOX,          "INPUT",              CMD_ID_Input,             NULL,           NULL},
            {CONTROL_NONE,              "",                   0,                        NULL,           NULL},
        }
    },
};
#define DSP     uiParams.dsp
#define MSC     DSP.music
#define MIC     DSP.mic
#define MIC     DSP.mic

#define ECHO    DSP.echo
#define REV     DSP.reverb
CONST item_info_t mscPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        EQ_CH_Music,        2+0,        &MSC.peq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        EQ_CH_Music,        2+1,        &MSC.peq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        EQ_CH_Music,        2+2,        &MSC.peq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ4",        CMD_ID_EQ,        EQ_CH_Music,        2+3,        &MSC.peq[3],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ5",        CMD_ID_EQ,        EQ_CH_Music,        2+4,        &MSC.peq[4],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ6",        CMD_ID_EQ,        EQ_CH_Music,        2+5,        &MSC.peq[5],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ7",        CMD_ID_EQ,        EQ_CH_Music,        2+6,        &MSC.peq[6],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
CONST item_info_t mscGeqItems[]={
    {CONTROL_LIST,     "GEQ1",        CMD_ID_EQ,        EQ_CH_Music,        0,          &MSC.geq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "GEQ2",        CMD_ID_EQ,        EQ_CH_Music,        1,          &MSC.geq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
CONST item_info_t mscItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      Gain_CH_Music,      0,          &MSC.gain,          &dspItems[CMD_ID_Gain].var[0]},
    {CONTROL_LIST,     "GEQ",         0,                EQ_CH_Music,        0,          &MSC.geq,           mscGeqItems},
    {CONTROL_LIST,     "PEQ",         0,                EQ_CH_Music,        0,          &MSC.peq,           mscPeqItems},
    {CONTROL_LIST,     "PITCH",       CMD_ID_PitchShift,0,                  0,          &MSC.pitch,         &dspItems[CMD_ID_PitchShift].var[0]},
    {CONTROL_LIST,     "INPUT",       CMD_ID_Input,     0,                  0,          &MSC.input,         &dspItems[CMD_ID_Input].var[0]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      HLPF_CH_Music,      HLPF_Hpf,   &MSC.hpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      HLPF_CH_Music,      HLPF_Lpf,   &MSC.lpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, NoiseGate_CH_Music, 0,          &MSC.noiseGate,     &dspItems[CMD_ID_NoiseGate].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
////////////////////////////////////////////////////////////////////////////////
CONST item_info_t micPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        EQ_CH_Mic,          3+0,        &MIC.peq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        EQ_CH_Mic,          3+1,        &MIC.peq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        EQ_CH_Mic,          3+2,        &MIC.peq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ4",        CMD_ID_EQ,        EQ_CH_Mic,          3+3,        &MIC.peq[3],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ5",        CMD_ID_EQ,        EQ_CH_Mic,          3+4,        &MIC.peq[4],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ6",        CMD_ID_EQ,        EQ_CH_Mic,          3+5,        &MIC.peq[5],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ7",        CMD_ID_EQ,        EQ_CH_Mic,          3+6,        &MIC.peq[6],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
CONST item_info_t micGeqItems[]={
    {CONTROL_LIST,     "GEQ1",        CMD_ID_EQ,        EQ_CH_Mic,          0,          &MIC.geq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "GEQ2",        CMD_ID_EQ,        EQ_CH_Mic,          1,          &MIC.geq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "GEQ3",        CMD_ID_EQ,        EQ_CH_Mic,          2,          &MIC.geq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
CONST item_info_t micItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      Gain_CH_Mic,        0,          &MIC.gain,          &dspItems[CMD_ID_Gain].var[0]},
    {CONTROL_LIST,     "GEQ",         0,                EQ_CH_Mic,          0,          &MIC.geq,           micGeqItems},
    {CONTROL_LIST,     "PEQ",         0,                EQ_CH_Mic,          0,          &MIC.peq,           micPeqItems},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      HLPF_CH_Mic,        HLPF_Hpf,   &MIC.hpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      HLPF_CH_Mic,        HLPF_Lpf,   &MIC.lpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "FEEDBACK",    CMD_ID_FeedBack,  FeedBack_CH_Mic,    0,          &MIC.feedback,      &dspItems[CMD_ID_FeedBack].var[0]},
    {CONTROL_LIST,     "LIMITER",     CMD_ID_Limiter,   Limiter_CH_Mic,     0,          &MIC.limiter,       &dspItems[CMD_ID_Limiter].var[0]},
    {CONTROL_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, NoiseGate_CH_Mic,   0,          &MIC.noiseGate,     &dspItems[CMD_ID_NoiseGate].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
////////////////////////////////////////////////////////////////////////////
CONST item_info_t echoPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        EQ_CH_Echo,         0,          &ECHO.peq[0],       &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        EQ_CH_Echo,         1,          &ECHO.peq[1],       &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        EQ_CH_Echo,         2,          &ECHO.peq[2],       &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
CONST item_info_t echoItems[]={
    {CONTROL_LIST,     "PEQ",         0,                EQ_CH_Echo,         0,          &ECHO.peq,          echoPeqItems},
    {CONTROL_LIST,     "REPEAT",      CMD_ID_Delay,     Vol_CH_Echo_Repeat, 0,          &ECHO.repeat,       &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "EFFECT VOL",  CMD_ID_Delay,     Vol_CH_Echo_EffVol, 0,          &ECHO.effVol,       &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "DIRECT VOL",  CMD_ID_Delay,     Vol_CH_Echo_DirVol, 0,          &ECHO.dirVol,       &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      HLPF_CH_Echo,       HLPF_Hpf,   &ECHO.hpf,          &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      HLPF_CH_Echo,       HLPF_Lpf,   &ECHO.lpf,          &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "PREDELAY",    CMD_ID_Delay,     Delay_CH_Echo_PreDelay, 0,      &ECHO.preDelay,     &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "DELAY",       CMD_ID_Delay,     Delay_CH_Echo_Delay,    0,      &ECHO.delay,        &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};

CONST item_info_t revPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        EQ_CH_Rev,          0,          &REV.peq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        EQ_CH_Rev,          1,          &REV.peq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        EQ_CH_Rev,          2,          &REV.peq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
CONST item_info_t reverbItems[]={
    {CONTROL_LIST,     "PEQ",         0,                EQ_CH_Rev,          0,          &REV.peq,           echoPeqItems},
    {CONTROL_LIST,     "EFFECT VOL",  CMD_ID_Delay,     Vol_CH_Rev_EffVol,  0,          &REV.effVol,        &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "DIRECT VOL",  CMD_ID_Delay,     Vol_CH_Rev_DirVol,  0,          &REV.dirVol,        &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      HLPF_CH_Rev,        HLPF_Hpf,   &REV.hpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      HLPF_CH_Rev,        HLPF_Lpf,   &REV.lpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "PREDELAY",    CMD_ID_Delay,     Delay_CH_Rev_PreDelay, 0,       &REV.preDelay,      &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "TIME",        CMD_ID_Delay,     Delay_CH_Rev_Time,  0,          &REV.time,          &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
CONST item_info_t effItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      Gain_CH_Eff,        0,          &DSP.effGain,       &dspItems[CMD_ID_Gain].var[0]},
    {CONTROL_LIST,     "ECHO",        0,                EQ_CH_Echo,         0,          &ECHO,              echoItems},
    {CONTROL_LIST,     "REVERB",      0,                EQ_CH_Rev,          0,          &REV,               reverbItems},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
/////////////////////////////////////////////////////////////////////////////////
CONST item_info_t preItems[]={
    {CONTROL_LIST,     "PRESET1",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_LIST,     "PRESET2",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_LIST,     "PRESET3",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_LIST,     "PRESET4",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_LIST,     "PRESET5",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_LIST,     "PRESET6",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_LIST,     "PRESET7",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_LIST,     "PRESET8",     0,                0,                  0,          NULL,               NULL},
    {CONTROL_NONE,     "",            0,                0,                  0,          NULL,               NULL},
};
///////////////////////////////////////////////////////////////////////////////

u8 gM=MENU_HOME;
listitem_t *gLists[MENU_MAX]={NULL};
cchr *gTitles[MENU_MAX]={"HOME", "MIC", "MUSIC", "EFFECT", "PRESET"};
CONST item_info_t *gInfos[MENU_MAX]={NULL, micItems, mscItems, effItems, preItems};


static void add_items(u8 menu)
{
    listitem_t *l;
    item_info_t *info;

    if(menu>=MENU_MAX) {
        return;
    }

    info = (item_info_t*)gInfos[menu];
    l = listitem_create(gTitles[menu], info, NULL, gui_post_refresh);
    gLists[menu] = l;
}




int menu_init(void)
{
    u8 m;
    
    for(m=0; m<MENU_MAX; m++) {
        add_items(m);
    }
    home_refresh();

    return 0;
}


int menu_free(void)
{
    return 0;
}


int menu_switch(u8 menu)
{
    if(gM!=menu) {
        if(menu==MENU_HOME) {
            listitem_quit(gLists[gM]);
            home_set_refresh(REFRESH_HOME_ALL);
        }
        else {
            listitem_set_refresh(gLists[menu], REFRESH_ALL);
        }

        gM = menu;
    }

    return 0;
}

int menu_refresh(void)
{
    if(gM==MENU_HOME) {
        home_refresh();
    }
    else {
        listitem_refresh(gLists[gM]);
    }

    
    return 0;
}


int menu_clear(void)
{
    rect_t r1=TITLE_RECT;
    rect_t r2=MENU_RECT;

    lcd_fill_rect(r1.x, r1.y, r1.w, r1.h, LCD_BC);
    lcd_fill_rect(r2.x, r2.y, r2.w, r2.h, LCD_BC);

    return 0;
}


int menu_handle(key_t *key)
{
    if(gM==MENU_HOME) {
        //
    }
    else {
        listitem_handle(&gLists[gM], key);
    }

    return 0;
}
















