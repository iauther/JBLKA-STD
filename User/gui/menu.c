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


const dsp_var_t dspItems[CMD_ID_NUM]={
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
            {CONTROL_INPUTBOX,          "ECHO PREDELAY",      CMD_ID_Delay,             NULL,           NULL},
            {CONTROL_INPUTBOX,          "ECHO DELAY",         CMD_ID_Delay,             NULL,           NULL},
            {CONTROL_INPUTBOX,          "REVERB PREDELAY",    CMD_ID_Delay,             NULL,           NULL},
            {CONTROL_INPUTBOX,          "REVERB DELAY",       CMD_ID_Delay,             NULL,           NULL},
            {CONTROL_INPUTBOX,          "MAINL DELAY",        CMD_ID_Delay,             NULL,           NULL},
            {CONTROL_INPUTBOX,          "MAINR DELAY",        CMD_ID_Delay,             NULL,           NULL},
            {CONTROL_INPUTBOX,          "SUB DELAY",          CMD_ID_Delay,             NULL,           NULL},
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
#define ECHO    DSP.echo
#define REV     DSP.reverb
const item_info_t mscPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &MSC.peq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &MSC.peq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &MSC.peq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ4",        CMD_ID_EQ,        &MSC.peq[3],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ5",        CMD_ID_EQ,        &MSC.peq[4],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ6",        CMD_ID_EQ,        &MSC.peq[5],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ7",        CMD_ID_EQ,        &MSC.peq[6],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_info_t mscGeqItems[]={
    {CONTROL_LIST,     "GEQ1",        CMD_ID_EQ,        &MSC.geq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "GEQ2",        CMD_ID_EQ,        &MSC.geq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_info_t mscItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      &MSC.gain,          &dspItems[CMD_ID_Gain].var[0]},
    {CONTROL_LIST,     "GEQ",         0,                &MSC.geq,           mscGeqItems},
    {CONTROL_LIST,     "PEQ",         0,                &MSC.peq,           mscPeqItems},
    {CONTROL_LIST,     "PITCH",       CMD_ID_PitchShift,&MSC.pitch,         &dspItems[CMD_ID_PitchShift].var[0]},
    {CONTROL_LIST,     "INPUT",       CMD_ID_Input,     &MSC.input,         &dspItems[CMD_ID_Input].var[0]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &MSC.hpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &MSC.lpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, &MSC.noiseGate,     &dspItems[CMD_ID_NoiseGate].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
////////////////////////////////////////////////////////////////////////////////
const item_info_t micPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &MIC.peq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &MIC.peq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &MIC.peq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ4",        CMD_ID_EQ,        &MIC.peq[3],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ5",        CMD_ID_EQ,        &MIC.peq[4],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ6",        CMD_ID_EQ,        &MIC.peq[5],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ7",        CMD_ID_EQ,        &MIC.peq[6],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_info_t micGeqItems[]={
    {CONTROL_LIST,     "GEQ1",        CMD_ID_EQ,        &MIC.geq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "GEQ2",        CMD_ID_EQ,        &MIC.geq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "GEQ3",        CMD_ID_EQ,        &MIC.geq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_info_t micItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      &MIC.gain,          &dspItems[CMD_ID_Gain].var[0]},
    {CONTROL_LIST,     "GEQ",         0,                &MIC.geq,           micGeqItems},
    {CONTROL_LIST,     "PEQ",         0,                &MIC.peq,           micPeqItems},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &MIC.hpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &MIC.lpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "FEEDBACK",    CMD_ID_FeedBack,  &MIC.feedback,      &dspItems[CMD_ID_FeedBack].var[0]},
    {CONTROL_LIST,     "LIMITER",     CMD_ID_Limiter,   &MIC.limiter,       &dspItems[CMD_ID_Limiter].var[0]},
    {CONTROL_LIST,     "NOISEGATE",   CMD_ID_NoiseGate, &MIC.noiseGate,     &dspItems[CMD_ID_NoiseGate].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
////////////////////////////////////////////////////////////////////////////
const item_info_t echoPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &ECHO.peq[0],       &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &ECHO.peq[1],       &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &ECHO.peq[2],       &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_info_t echoItems[]={
    {CONTROL_LIST,     "PEQ",         0,                &ECHO.peq,          echoPeqItems},
    {CONTROL_LIST,     "REPEAT",      CMD_ID_Delay,     &ECHO.repeat,       &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "EFFECT VOL",  CMD_ID_Delay,     &ECHO.effVol,       &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "DIRECT VOL",  CMD_ID_Delay,     &ECHO.dirVol,       &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &ECHO.hpf,          &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &ECHO.lpf,          &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "PREDELAY",    CMD_ID_Delay,     &ECHO.preDelay,     &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "DELAY",       CMD_ID_Delay,     &ECHO.delay,        &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};

const item_info_t revPeqItems[]={
    {CONTROL_LIST,     "PEQ1",        CMD_ID_EQ,        &REV.peq[0],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ2",        CMD_ID_EQ,        &REV.peq[1],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_LIST,     "PEQ3",        CMD_ID_EQ,        &REV.peq[2],        &dspItems[CMD_ID_EQ].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_info_t reverbItems[]={
    {CONTROL_LIST,     "PEQ",         0,                &REV.peq,           echoPeqItems},
    {CONTROL_LIST,     "EFFECT VOL",  CMD_ID_Delay,     &REV.effVol,        &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "DIRECT VOL",  CMD_ID_Delay,     &REV.dirVol,        &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "HPF",         CMD_ID_HLPF,      &REV.hpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "LPF",         CMD_ID_HLPF,      &REV.lpf,           &dspItems[CMD_ID_HLPF].var[0]},
    {CONTROL_LIST,     "PREDELAY",    CMD_ID_Delay,     &REV.preDelay,      &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_LIST,     "TIME",        CMD_ID_Delay,     &REV.time,          &dspItems[CMD_ID_Delay].var[0]},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};
const item_info_t effItems[]={
    {CONTROL_LIST,     "GAIN",        CMD_ID_Gain,      &DSP.effGain,       &dspItems[CMD_ID_Gain].var[0]},
    {CONTROL_LIST,     "ECHO",        0,                &ECHO,              echoItems},
    {CONTROL_LIST,     "REVERB",      0,                &REV,               reverbItems},
    {CONTROL_NONE,     "",            0,                NULL,               NULL},
};

///////////////////////////////////////////////////////////////////////////////
int menu_init(void)
{
    gFuncs[gM].init();
    gui_post_refresh();

    return 0;
}


int menu_free(void)
{
    gFuncs[gM].free();
    return 0;
}


int menu_switch(u8 menu)
{
    if(gM!=menu) {
        gM = menu;
        menu_init();
    }

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


int menu_handle(key_t key)
{
    gFuncs[gM].handle(key);
    gui_post_refresh();

    return 0;
}

static u8 get_num(item_info_t *info)
{
    u8 i=0;
    while(info[i].control!=CONTROL_NONE) i++;
    return i;
}
static void add_item(listitem_t *l, const item_info_t *info)
{
    node_t n;
    u8 i,j,num;
    item_info_t inf;

    if(!l || !info) {
        return;
    }

    for(i=0;;i++) {
        inf = info[i];
        if(inf.control==CONTROL_LIST) {
            listitem_t *pl;
            rect_t r=MENU_RECT;
            
            num = get_num(&inf);
            pl = listitem_init(inf.txt, &r, num, sizeof(inf));
            if(inf.info) {
                add_item(pl, inf.info);
            }
            inf.handle = pl;
        }
        else if(inf.control==CONTROL_INPUTBOX){
            if(strlen(PARA_INFO[inf.cmd].info->name)==0) {
                break;
            }
            inf.handle = inputbox_init(info, j);
        }
        else {
            break;
        }

        n.ptr = &inf;
        n.len = sizeof(inf);
        listitem_add(l, &n);
    }
}
int menu_add_item(listitem_t *l, u8 menu)
{
    const item_info_t *info;

    if(!l) {
        return -1;
    }
    
    switch(menu) {
    
        case MENU_MUSIC:
        info = mscItems;
        break;

        case MENU_MIC:
        info = micItems;
        break;

        case MENU_EFFECT:
        info = effItems;
        break;
        
        case MENU_PRESET:
        //p = (item_info_t*)preItems;
        break;

        default:
        return -1;
    }

    add_item(l, info);

    return 0;
}















