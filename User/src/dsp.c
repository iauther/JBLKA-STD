#include "dsp.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "e2p.h"
#include "lock.h"
#include "paras.h"
#include "packet.h"
#include "config.h"
#include "default.h"
#include <stm32f10x.h>

#define STARTUP_CODE    0xEFBEADDE

#define UPLOAD_SIZE         44
#define DOWNLOAD_SIZE       256
#define ComHeadLen          4 //长度包括 ID+Ch+No+crc 四个字节

typedef struct {
    TypeS_CmdSt cmd;
    u16         buf[200];
    u16         ver;
}dsp_buf_t;

#define RX_BUF_LEN 100
u8 dspRxBuf[RX_BUF_LEN];
u8 dspStarted=0;
u8 dsp_ack_flag=0;
//extern paras_data_t gParams;
//extern paras_ui_t uiParams;
#if 0
UartParas_t dspParas={0};
UART_HandleTypeDef *hdsp;
#endif

dsp_buf_t gDspBuf;
u16 crc_calc(u16 *data, u16 length)
{
    u16 i;
    u16 sum=0;

    for (i = 0; i < length; i++) {
        sum += data[i];
    }

    return sum;
}


static int get_node(dsp_data_t *dd, node_t *node)
{
    node_t n;
    Dsp_Paras *dsp=&gParams.dsp;

    switch(dd->id) {
        case CMD_ID_Gain:
        if(dd->ch>=Gain_CH_NUM) return -1;
        n.len = sizeof(TypeS_Gain);
        n.ptr = &dsp->Array_Gain[dd->ch];
        break;

        case CMD_ID_Vol:
        if(dd->ch>=Vol_CH_NUM) return -1;
        n.len = sizeof(TypeS_Vol);
        n.ptr = &dsp->Array_Vol[dd->ch];
        break;

        case CMD_ID_EQ:
        if(dd->ch>=EQ_CH_NUM) return -1;
        n.len = sizeof(TypeS_EQBand);
        n.ptr = &dsp->Array_EQ[dd->ch].BandCoef[dd->n];
        break;

        case CMD_ID_HLPF:
        if(dd->ch>=HLPF_CH_NUM) return -1;
        n.len = sizeof(TypeS_HLPFCoef);
        n.ptr = &dsp->Array_HLPF[dd->ch].HLPFCoef[dd->n];
        break;

        case CMD_ID_Delay:
        if(dd->ch>=Delay_CH_NUM) return -1;
        n.len = sizeof(TypeS_Delay);
        n.ptr = &dsp->Array_Delay[dd->ch];
        break;

        case CMD_ID_FeedBack:
        if(dd->ch>=FeedBack_CH_NUM) return -1;
        n.len = sizeof(TypeS_FeedBack);
        n.ptr = &dsp->Array_FeedBack[dd->ch];
        break;

        case CMD_ID_Limiter:
        if(dd->ch>=Limiter_CH_NUM) return -1;
        n.len = sizeof(TypeS_Limiter);
        n.ptr = &dsp->Array_Limiter[dd->ch];
        break;

        case CMD_ID_PitchShift:
        n.len = sizeof(TypeS_PitchShift);
        n.ptr = &dsp->Array_PitchShift;
        break;

        case CMD_ID_Mute:
        if(dd->ch>=Mute_CH_NUM) return -1;
        n.len = sizeof(TypeS_Mute);
        n.ptr = &dsp->Array_Mute[dd->ch];
        break;

        case CMD_ID_NoiseGate:
        if(dd->ch>=NoiseGate_CH_NUM) return -1;
        n.len = sizeof(TypeS_NoiseGate);
        n.ptr = &dsp->Array_NoiseGate[dd->ch];
        break;

        case CMD_ID_Input:
        n.len = sizeof(TypeS_Input);
        n.ptr = &dsp->Array_Input;
        break;
        
        case CMD_ID_DSPVer:
        n.len = 1;
        n.ptr = &dsp->Array_Input;
        break;

        default:
        return -1;
    }
    if(dd->dlen != n.len) {
        return -1;
    }
    
    if(node) {
        *node = n;
    }
    
    return 0;
}

static void dsp_wait_started(void)
{
    u16 i;
    for(i=0; i<200; i++) {
        delay_ms(10);
        if(dspStarted) break;
    }
}

static int dsp_write(dsp_buf_t *db, u8 wait)
{
    int r,wlen;
    u16 crc;
    u16 *ptr = (u16*)db->cmd.DataPtr;
    u16 u16Len = db->cmd.Len/2;

    db->buf[0] = 0xFF55;
    db->buf[1] = u16Len+ComHeadLen;
    db->buf[2] = db->cmd.ID;
    db->buf[3] = db->cmd.Ch;
    db->buf[4] = db->cmd.No;
    memcpy(&db->buf[5], ptr, db->cmd.Len);

    crc = crc_calc(&db->buf[1], u16Len+ComHeadLen);
    db->buf[5+u16Len] = crc;

    wlen = sizeof(u16)*(6+u16Len);
    r = usart_write(DSP_UART, (u8*)db->buf, wlen);
    if(wait) {
        dsp_ack_flag = 0;while(dsp_ack_flag==0);
    }
    
    return (r==wlen)?0:-1;
}


static int dsp_read(u8 *data, u16 len)
{
    return usart_read(DSP_UART, data, len);
}

/*
case CMD_ID_UpdataDSP:
case CMD_ID_Download:
cmd.Len = 128;//每次下载128个双字节，即256字节
cmd.No = No;//表示发送了第几个128个双字节
//cmd.DataPtr = buf;//buf用来存储128个双字节*/
static int do_download(void *data, u16 len)
{
    u32 i,left,times;
    u8 *ptr = (u8*)data;

    left = len%DOWNLOAD_SIZE;
    times = len/DOWNLOAD_SIZE;

    gDspBuf.cmd.ID = CMD_ID_Download;
    gDspBuf.cmd.Len = DOWNLOAD_SIZE;
    for(i=0; i<times; i++) {
        gDspBuf.cmd.Ch = i;
        gDspBuf.cmd.No = (left==0 && i==times-1);
        gDspBuf.cmd.DataPtr = (u16*)(ptr+i*DOWNLOAD_SIZE);
        dsp_write(&gDspBuf, 1);
    }
    
    if(left>0) {
        gDspBuf.cmd.No = 1;
        gDspBuf.cmd.Ch = i;
        gDspBuf.cmd.Len = (left%2==0)?left:(left+1);
        gDspBuf.cmd.DataPtr = (u16*)(ptr+i*DOWNLOAD_SIZE);
        dsp_write(&gDspBuf, 1);
    }

    return 0;
}


void dsp_reset(void)
{
    GPIO_InitTypeDef init={0};
    
    dspStarted = 0;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_8;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &init);

    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
    delay_ms(20);
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);

    dsp_wait_started();
    dsp_version();
}

static void save_version(dsp_version_t *ver)
{
    u8 tmp[20];
    node_t n;
    
    sprintf((char*)tmp, "V%04d", ver->ver);
    if(strcmp((char*)tmp, (char*)gParams.fw.dsp)) {
        strcpy((char*)gParams.fw.dsp, (char*)tmp);

        n.ptr = &gParams.fw;
        n.len = sizeof(gParams.fw);
        e2p_put(&n);
    }
}
static void dsp_rx_cb(u8 *data, u16 len)
{
    if(len==4) {
        if(*(u32*)data==STARTUP_CODE) {
            dspStarted = 1;
        }
    }
    else if(len==sizeof(dsp_version_t)) {
        dsp_version_t *ver=(dsp_version_t*)data;
        save_version(ver);
    }
    else if(len==sizeof(dsp_ack_t)){
        dsp_ack_t *ack=(dsp_ack_t*)data;
    }
    dsp_ack_flag = 1;
}


int dsp_init(void)
{
    uart_paras_t para={dsp_rx_cb, dspRxBuf, RX_BUF_LEN};
    
    usart_init(DSP_UART, &para);
    dsp_reset();
    do_download(&gParams.dsp, sizeof(gParams.dsp));

    return 0;
}


static void pre_download(TypeS_Gain *g)
{
    u8 i;
    TypeS_Gain gain;

    gDspBuf.cmd.ID = CMD_ID_Gain;
    gDspBuf.cmd.Len = sizeof(TypeS_Gain);
    gDspBuf.cmd.No = 0;
    gDspBuf.cmd.DataPtr = (u16*)&gain;
    
    gain.Mute = 1;
    for(i=0; i<Gain_CH_NUM; i++) {
        gain.Gain = g[i].Gain;  //??
        gDspBuf.cmd.Ch = i;
        dsp_write(&gDspBuf, 0);
    }
}
static void post_download(TypeS_Gain *g)
{
    u8 i;
    TypeS_Gain gain;

    gDspBuf.cmd.ID = CMD_ID_Gain;
    gDspBuf.cmd.Len = sizeof(TypeS_Gain);
    gDspBuf.cmd.No = 0;
    gDspBuf.cmd.DataPtr = (u16*)&gain;
    
    for(i=0; i<Gain_CH_NUM; i++) {
        gain = g[i];
        gDspBuf.cmd.Ch = i;
        dsp_write(&gDspBuf, 0);
    }
}

int dsp_download(void)
{
    TypeS_Gain *g=gParams.dsp.Array_Gain;

    lock_on(LOCK_DSP);

    pre_download(g);
    delay_ms(1000);
    do_download(&gParams.dsp, sizeof(gParams.dsp));
    delay_ms(1000);
    post_download(g);

    lock_off(LOCK_DSP);

    return 0;
}

int dsp_reset_peq(eq_reset_t *rst, node_t *n)
{
    u8 i;
    int r;
    dsp_data_t dsp={0};

    lock_on(LOCK_DSP);
    r = paras_reset_peq(rst);
    if(r) {
        goto quit;
    }

    dsp.ch = rst->ch;
    dsp.id = CMD_ID_EQ;
    dsp.dlen = sizeof(TypeS_EQBand);
    for(i=0; i<MaxEQBand; i++) {
        dsp.n = i;
        dsp_send(&dsp);
    }

    if(n) {
        n->ptr = &gParams.dsp.Array_EQ[rst->ch];
        n->len = sizeof(TypeS_EQ);
    }

quit:
    lock_off(LOCK_DSP);

    return r;
}


int dsp_set_started(void)
{
    dspStarted = 1;
    return 0;
}


int dsp_is_started(void)
{
    return dspStarted;
}


//只有EQ,HLPF,用到No
//用在EQ的时候，No就是Band
//用在HLPF的时候，No=0/1:hpf/lpf
int dsp_send(dsp_data_t *dsp)
{
    int r;
    node_t n;
    
    if(!dspStarted) {
        return -1;
    }

    lock_on(LOCK_DSP);
    r = get_node(dsp, &n);
    if(r) {
        goto quit;
    }

    gDspBuf.cmd.ID = dsp->id;
    gDspBuf.cmd.Ch = dsp->ch;
    gDspBuf.cmd.No = dsp->n;
    gDspBuf.cmd.Len = n.len;
    gDspBuf.cmd.DataPtr = (u16 *)n.ptr;

    if(dsp->id==CMD_ID_Input) {
        TypeS_Input *input=(TypeS_Input*)dsp->data;
        sys_set_input(input->input);
    }

    r = dsp_write(&gDspBuf, 0);

quit:
    lock_off(LOCK_DSP);

    return r;
}


int dsp_upload(void *data, u16 len)
{
    return 0;
}


int dsp_version(void)
{
    u16 tmp[10];
    
    lock_on(LOCK_DSP);
    gDspBuf.cmd.ID = CMD_ID_DSPVer;
    gDspBuf.cmd.Len = 1;
    gDspBuf.cmd.Ch = 0;
    gDspBuf.cmd.DataPtr = tmp;
    gDspBuf.cmd.No = 0;
    
    dsp_write(&gDspBuf, 0);
    lock_off(LOCK_DSP);

    return 0;
}


int dsp_upgrade(u16 index, u8 *data, u16 len, u8 last)
{
    if(!dspStarted) {
        return -1;
    }

    if(len>DOWNLOAD_SIZE || len%2) {
        return -1;
    }

    lock_on(LOCK_DSP);
    gDspBuf.cmd.ID = CMD_ID_UpdataDSP;
    gDspBuf.cmd.Len = len;

    gDspBuf.cmd.Ch = index;
    gDspBuf.cmd.DataPtr = (u16*)data;
    gDspBuf.cmd.No = last;
    
    dsp_write(&gDspBuf, 0);
    if(last) {
        //dsp_reset();
        //dsp_download();
    }
    lock_off(LOCK_DSP);
    
    return 0;
}


void dsp_remap(dsp_paras_t *paras, Dsp_Paras *dsp)
{
    u8 i;
    
    //music
    paras->music.gain = &dsp->Array_Gain[Gain_CH_Music];
    paras->music.eq = &dsp->Array_EQ[EQ_CH_Music];
    for(i=0; i<2; i++) {
        paras->music.geq[i] = &dsp->Array_EQ[EQ_CH_Music].BandCoef[i];
    }
    for(i=2; i<PEQ_BANDS+2; i++) {
        paras->music.peq[i-2] = &dsp->Array_EQ[EQ_CH_Music].BandCoef[i];
    }
    paras->music.hpf = &dsp->Array_HLPF[HLPF_CH_Music].HLPFCoef[HLPF_Hpf];
    paras->music.lpf = &dsp->Array_HLPF[HLPF_CH_Music].HLPFCoef[HLPF_Lpf];
    paras->music.noiseGate = &dsp->Array_NoiseGate[NoiseGate_CH_Music];
    paras->music.input = &dsp->Array_Input;
    paras->music.pitch = &dsp->Array_PitchShift;

    //mic
    paras->mic.gain = &dsp->Array_Gain[Gain_CH_Mic];
    paras->mic.eq = &dsp->Array_EQ[EQ_CH_Mic];
    for(i=0; i<3; i++) {
        paras->mic.geq[i] = &dsp->Array_EQ[EQ_CH_Mic].BandCoef[i];
    }
    for(i=3; i<PEQ_BANDS+3; i++) {
        paras->mic.peq[i-3] = &dsp->Array_EQ[EQ_CH_Mic].BandCoef[i];
    }
    paras->mic.hpf = &dsp->Array_HLPF[HLPF_CH_Mic].HLPFCoef[HLPF_Hpf];
    paras->mic.lpf = &dsp->Array_HLPF[HLPF_CH_Mic].HLPFCoef[HLPF_Lpf];
    paras->mic.feedback = &dsp->Array_FeedBack[FeedBack_CH_Mic];
    paras->mic.limiter = &dsp->Array_Limiter[Limiter_CH_Mic];
    paras->mic.noiseGate = &dsp->Array_NoiseGate[NoiseGate_CH_Mic];

    //rec
    paras->rec.eq = &dsp->Array_EQ[EQ_CH_Rec];
    for(i=0; i<PEQ_BANDS; i++) {
        paras->rec.peq[i] = &dsp->Array_EQ[EQ_CH_Rec].BandCoef[i];
    }
    paras->rec.hpf = &dsp->Array_HLPF[HLPF_CH_Rec].HLPFCoef[HLPF_Hpf];
    paras->rec.lpf = &dsp->Array_HLPF[HLPF_CH_Rec].HLPFCoef[HLPF_Lpf];
    paras->rec.musicVol = &dsp->Array_Vol[Vol_CH_Rec_MusicVol];
    paras->rec.dirVol = &dsp->Array_Vol[Vol_CH_Rec_DirVol];
    paras->rec.echoVol = &dsp->Array_Vol[Vol_CH_Rec_EchoVol];
    paras->rec.reverbVol = &dsp->Array_Vol[Vol_CH_Rec_RevVol];
    paras->rec.vol = &dsp->Array_Vol[Vol_CH_RecVol];
    paras->rec.limiter = &dsp->Array_Limiter[Limiter_CH_Rec];
    paras->rec.mute = &dsp->Array_Mute[Mute_CH_Rec];

    //sub
    paras->sub.eq = &dsp->Array_EQ[EQ_CH_Sub];
    for(i=0; i<PEQ_BANDS; i++) {
        paras->sub.peq[i] = &dsp->Array_EQ[EQ_CH_Sub].BandCoef[i];
    }
    paras->sub.hpf = &dsp->Array_HLPF[HLPF_CH_Sub].HLPFCoef[HLPF_Hpf];
    paras->sub.lpf = &dsp->Array_HLPF[HLPF_CH_Sub].HLPFCoef[HLPF_Lpf];
    paras->sub.delay = &dsp->Array_Delay[Delay_CH_Sub_Delay];
    paras->sub.musicVol = &dsp->Array_Vol[Vol_CH_Sub_MusicVol];
    paras->sub.dirVol = &dsp->Array_Vol[Vol_CH_Sub_DirVol];
    paras->sub.echoVol = &dsp->Array_Vol[Vol_CH_Sub_EchoVol];
    paras->sub.reverbVol = &dsp->Array_Vol[Vol_CH_Sub_RevVol];
    paras->sub.vol = &dsp->Array_Vol[Vol_CH_SubVol];
    paras->sub.limiter = &dsp->Array_Limiter[Limiter_CH_Sub];
    paras->sub.mute = &dsp->Array_Mute[Mute_CH_Sub];

    //main
    paras->main.eq = &dsp->Array_EQ[EQ_CH_Main];
    for(i=0; i<PEQ_BANDS; i++) {
        paras->main.peq[i] = &dsp->Array_EQ[EQ_CH_Main].BandCoef[i];
    }
    paras->main.hpf = &dsp->Array_HLPF[HLPF_CH_Main].HLPFCoef[HLPF_Hpf];
    paras->main.lpf = &dsp->Array_HLPF[HLPF_CH_Main].HLPFCoef[HLPF_Lpf];

    paras->main.musicVol = &dsp->Array_Vol[Vol_CH_Main_MusicVol];
    paras->main.dirVol = &dsp->Array_Vol[Vol_CH_Main_DirVol];
    paras->main.echoVol = &dsp->Array_Vol[Vol_CH_Main_EchoVol];
    paras->main.reverbVol = &dsp->Array_Vol[Vol_CH_Main_RevVol];
    paras->main.lVol = &dsp->Array_Vol[Vol_CH_Main_LVol];
    paras->main.rVol = &dsp->Array_Vol[Vol_CH_Main_RVol];
    paras->main.lDelay = &dsp->Array_Delay[Delay_CH_MainL_Delay];
    paras->main.rDelay = &dsp->Array_Delay[Delay_CH_MainR_Delay];
    paras->main.limiter = &dsp->Array_Limiter[Limiter_CH_Main];
    paras->main.L = &dsp->Array_Mute[Mute_CH_MainL];
    paras->main.R = &dsp->Array_Mute[Mute_CH_MainR];

    //echo
    paras->effGain = &dsp->Array_Gain[Gain_CH_Eff];
    paras->echo.eq = &dsp->Array_EQ[EQ_CH_Echo];
    for(i=0; i<3; i++) {
        paras->echo.peq[i] = &dsp->Array_EQ[EQ_CH_Echo].BandCoef[i];
    }
    paras->echo.repeat = &dsp->Array_Vol[Vol_CH_Echo_Repeat];
    paras->echo.effVol = &dsp->Array_Vol[Vol_CH_Echo_EffVol];
    paras->echo.dirVol = &dsp->Array_Vol[Vol_CH_Echo_DirVol];
    paras->echo.hpf = &dsp->Array_HLPF[HLPF_CH_Echo].HLPFCoef[HLPF_Hpf];
    paras->echo.lpf = &dsp->Array_HLPF[HLPF_CH_Echo].HLPFCoef[HLPF_Lpf];
    paras->echo.preDelay = &dsp->Array_Delay[Delay_CH_Echo_PreDelay];
    paras->echo.delay = &dsp->Array_Delay[Delay_CH_Echo_Delay];
    
    //reverb
    paras->reverb.eq = &dsp->Array_EQ[EQ_CH_Rev];
    for(i=0; i<3; i++) {
        paras->reverb.peq[i] = &dsp->Array_EQ[EQ_CH_Rev].BandCoef[i];
    }
    paras->reverb.effVol = &dsp->Array_Vol[Vol_CH_Rev_EffVol];
    paras->reverb.dirVol = &dsp->Array_Vol[Vol_CH_Rev_DirVol];
    paras->reverb.hpf = &dsp->Array_HLPF[HLPF_CH_Rev].HLPFCoef[HLPF_Hpf];
    paras->reverb.lpf = &dsp->Array_HLPF[HLPF_CH_Rev].HLPFCoef[HLPF_Lpf];
    paras->reverb.preDelay = &dsp->Array_Delay[Delay_CH_Rev_PreDelay];
    paras->reverb.time = &dsp->Array_Delay[Delay_CH_Rev_Time];
}


int dsp_update(dsp_data_t *dsp, node_t *node)
{
    int r;
    node_t n;

    lock_on(LOCK_DSP);
    r = get_node(dsp, &n);
    if(r==0) {
        memcpy(n.ptr, dsp->data, n.len);
        *node = n;
    }
    lock_off(LOCK_DSP);

    return r;
}


int dsp_get_struct_len(u8 cmd)
{
    if(cmd<CMD_ID_Gain || cmd>CMD_ID_Input) {
        return -1;
    }

    return STRUCT_LEN[cmd];
}

u8* dsp_get_struct_ptr(u8 cmd)
{
    if(cmd<CMD_ID_Gain || cmd>CMD_ID_Input) {
        return NULL;
    }

    return (u8*)STRUCT_PTR[cmd];
}



u8 dsp_get_ch(u8 cmd, void *p)
{
    u8 ch,*p2=(u8*)p;
    int slen=dsp_get_struct_len(cmd);
    u8 *pstt=dsp_get_struct_ptr(cmd);

    ch = (p2-pstt)/slen;

    return ch;
}


u8 dsp_get_n(u8 cmd, void *p)
{
    u8 ch,*p2=(u8*)p;
    int slen=dsp_get_struct_len(cmd);
    u8 *pstt=dsp_get_struct_ptr(cmd);

    if(cmd==CMD_ID_EQ) {
        
    }
    else if(cmd==CMD_ID_HLPF) {
        
    }
    else {
        return 0;
    }

    ch = (p2-pstt)/slen;

    return ch;
}


int dsp_get_info(u8 key, dsp_info_t *info)
{
    dsp_paras_t *dsp=&uiParams.dsp;

    switch(key) {
        case KEY_MUSIC_UP:      //0~100
        case KEY_MUSIC_DN:
        {
            info->dsp.id = CMD_ID_Gain;
            info->dsp.ch = Gain_CH_Music;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Gain);
            info->index = 0;
            info->title = (char*)"MUSIC";
            info->name = (char*)"GAIN";
            info->fac = (key==KEY_MUSIC_UP)?1:-1;
            info->pdata = (s16*)&dsp->music.gain->Gain;
            info->node.ptr = dsp->music.gain;
            info->node.len = sizeof(TypeS_Gain);
        }
        break;

        case KEY_EFFECT_UP:
        case KEY_EFFECT_DN:
        {
            info->dsp.id = CMD_ID_Gain;
            info->dsp.ch = Gain_CH_Eff;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Gain);
            info->index = 0;
            info->title = (char*)"EFFECT";
            info->name = (char*)"GAIN";
            info->fac = (key==KEY_EFFECT_UP)?1:-1;
            info->pdata = (s16*)&dsp->effGain->Gain;
            info->node.ptr = dsp->effGain;
            info->node.len = sizeof(TypeS_Gain);
        }
        break;

        case KEY_MIC_UP:
        case KEY_MIC_DN:
        {
            info->dsp.id = CMD_ID_Gain;
            info->dsp.ch = Gain_CH_Mic;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Gain);
            info->index = 0;
            info->title = (char*)"MIC";
            info->name = (char*)"GAIN";
            info->fac = (key==KEY_MIC_UP)?1:-1;
            info->pdata = (s16*)&dsp->mic.gain->Gain;
            info->node.ptr = dsp->mic.gain;
            info->node.len = sizeof(TypeS_Gain);
        }
        break;
        
        case KEY_MUSIC_TREBLE_UP:
        case KEY_MUSIC_TREBLE_DN:
        {
            info->dsp.id = CMD_ID_EQ;
            info->dsp.ch = EQ_CH_Music;
            info->dsp.n = 1;
            info->dsp.dlen = sizeof(TypeS_EQBand);
            info->index = 1;
            info->title = (char*)"MUSIC";
            info->name = (char*)"TREBLE";
            info->fac = (key==KEY_MUSIC_TREBLE_UP)?1:-1;
            info->pdata = (s16*)&dsp->music.geq[1]->Gain;
            info->node.ptr = dsp->music.geq[1];
            info->node.len = sizeof(TypeS_EQBand);
        }
        break;
        
        case KEY_MUSIC_BASS_UP:
        case KEY_MUSIC_BASS_DN:
        {
            info->dsp.id = CMD_ID_EQ;
            info->dsp.ch = EQ_CH_Music;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_EQBand);
            info->index = 1;
            info->title = (char*)"MUSIC";
            info->name = (char*)"BASS";
            info->fac = (key==KEY_MUSIC_BASS_UP)?1:-1;
            info->pdata = (s16*)&dsp->music.geq[0]->Gain;
            info->node.ptr = dsp->music.geq[0];
            info->node.len = sizeof(TypeS_EQBand);
        }
        break;
        
        case KEY_MIC_TREBLE_UP:
        case KEY_MIC_TREBLE_DN:
        {
            info->dsp.id = CMD_ID_EQ;
            info->dsp.ch = EQ_CH_Mic;
            info->dsp.n = 2;
            info->dsp.dlen = sizeof(TypeS_EQBand);
            info->index = 1;
            info->title = (char*)"MIC";
            info->name = (char*)"TREBLE";
            info->fac = (key==KEY_MIC_TREBLE_UP)?1:-1;
            info->pdata = (s16*)&dsp->mic.geq[2]->Gain;
            info->node.ptr = dsp->mic.geq[2];
            info->node.len = sizeof(TypeS_EQBand);
        }
        break;

        case KEY_MIC_MIDDLE_UP:
        case KEY_MIC_MIDDLE_DN:
        {
            info->dsp.id = CMD_ID_EQ;
            info->dsp.ch = EQ_CH_Mic;
            info->dsp.n = 1;
            info->dsp.dlen = sizeof(TypeS_EQBand);
            info->index = 1;
            info->title = (char*)"MIC";
            info->name = (char*)"MIDDLE";
            info->fac = (key==KEY_MIC_MIDDLE_UP)?1:-1;
            info->pdata = (s16*)&dsp->mic.geq[1]->Gain;
            info->node.ptr = dsp->mic.geq[1];
            info->node.len = sizeof(TypeS_EQBand);
        }
        break;

        case KEY_MIC_BASS_UP:               //-24~+12dB
        case KEY_MIC_BASS_DN:
        {
            info->dsp.id = CMD_ID_EQ;
            info->dsp.ch = EQ_CH_Mic;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_EQBand);
            info->index = 1;
            info->title = (char*)"MIC";
            info->name = (char*)"BASS";
            info->fac = (key==KEY_MIC_BASS_UP)?1:-1;
            info->pdata = (s16*)&dsp->mic.geq[0]->Gain;
            info->node.ptr = dsp->mic.geq[0];
            info->node.len = sizeof(TypeS_EQBand);
        }
        break;
        
        case KEY_ECHO_LEVEL_UP:
        case KEY_ECHO_LEVEL_DN:
        {
            info->dsp.id = CMD_ID_Vol;
            info->dsp.ch = Vol_CH_Echo_EffVol;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Vol);
            info->index = 0;
            info->title = (char*)"ECHO";
            info->name = (char*)"LEVEL";
            info->fac = (key==KEY_ECHO_LEVEL_UP)?1:-1;
            info->pdata = (s16*)&dsp->echo.effVol->Vol;
            info->node.ptr = dsp->echo.effVol;
            info->node.len = sizeof(TypeS_Vol);
        }
        break;

        case KEY_ECHO_DELAY_UP:
        case KEY_ECHO_DELAY_DN:
        {
            info->dsp.id = CMD_ID_Delay;
            info->dsp.ch = Delay_CH_Echo_Delay;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Delay);
            info->index = 0;
            info->title = (char*)"ECHO";
            info->name = (char*)"DELAY";
            info->fac = (key==KEY_ECHO_DELAY_UP)?1:-1;
            info->pdata = (s16*)&dsp->echo.delay->Delay;
            info->node.ptr = dsp->echo.delay;
            info->node.len = sizeof(TypeS_Delay);
        }
        break;

        case KEY_ECHO_REPEAT_UP:
        case KEY_ECHO_REPEAT_DN:
        {
            info->dsp.id = CMD_ID_Vol;
            info->dsp.ch = Vol_CH_Echo_Repeat;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Vol);
            info->index = 0;
            info->title = (char*)"ECHO";
            info->name = (char*)"REPEAT";
            info->fac = (key==KEY_ECHO_REPEAT_UP)?1:-1;
            info->pdata = (s16*)&dsp->echo.repeat->Vol;
            info->node.ptr = dsp->echo.repeat;
            info->node.len = sizeof(TypeS_Vol);
        }
        break;
        
        case KEY_REVERB_LEVEL_UP:
        case KEY_REVERB_LEVEL_DN:
        {
            info->dsp.id = CMD_ID_Vol;
            info->dsp.ch = Vol_CH_Rev_EffVol;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Vol);
            info->index = 0;
            info->title = (char*)"REVERB";
            info->name = (char*)"LEVEL";
            info->fac = (key==KEY_REVERB_LEVEL_UP)?1:-1;
            info->pdata = (s16*)&dsp->reverb.effVol->Vol;
            info->node.ptr = dsp->reverb.effVol;
            info->node.len = sizeof(TypeS_Vol);
        }
        break;

        case KEY_REVERB_TIME_UP:
        case KEY_REVERB_TIME_DN:
        {
            info->dsp.id = CMD_ID_Delay;
            info->dsp.ch = Delay_CH_Rev_Time;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Delay);
            info->index = 0;
            info->title = (char*)"REVERB";
            info->name = (char*)"TIME";
            info->fac = (key==KEY_REVERB_TIME_UP)?1:-1;
            info->pdata = (s16*)&dsp->reverb.time->Delay;
            info->node.ptr = dsp->reverb.time;
            info->node.len = sizeof(TypeS_Delay);
        }
        break;

        case KEY_b:
        case KEY_0:
        case KEY_SHARP:
        {
            info->dsp.id = CMD_ID_PitchShift;
            info->dsp.ch = 0;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_PitchShift);
            info->index = 0;
            info->fac = 0;
            info->title = (char*)"PITCH";
            info->name = (char*)"";
            info->pdata = (s16*)&dsp->music.pitch->PitchShift;
            info->node.ptr = dsp->music.pitch;
            info->node.len = sizeof(TypeS_PitchShift);
        }
        break;

        case KEY_MODE:
        case KEY_M1:
        case KEY_M2:
        case KEY_M3:
        {
            info->dsp.id = 0;
            info->dsp.ch = 0;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(*uiParams.pre);
            info->index = 0;
            info->fac = 0;
            info->title = (char*)"PRESET";
            info->name = (char*)"";
            info->pdata = (s16*)uiParams.pre;
            info->node.ptr = info->pdata;
            info->node.len = sizeof(*uiParams.pre);
        }
        break;

        case KEY_INPUT:
        {
            info->dsp.id = CMD_ID_Input;
            info->dsp.ch = 0;
            info->dsp.n = 0;
            info->dsp.dlen = sizeof(TypeS_Input);
            info->index = 0;
            info->fac = 0;
            info->title = (char*)"INPUT";
            info->name = (char*)"GAIN";
            info->pdata = (s16*)&dsp->music.input->input;
            info->node.ptr = dsp->music.input;
            info->node.len = sizeof(TypeS_Input);
        }
        break;

        case KEY_MUTE:
        {
            info->dsp.id = CMD_ID_Mute;
            info->index = 0;
            info->title = (char*)"MUTE";
            info->name = (char*)"";
            //info->pdata = NULL;
            //info->ptr = NULL;
            //info->len = sizeof(TypeS_Mute);
            //info->fac = 0;
            //info->channel = 0;
        }
        break;
    }

    return 0;
}


#include "key.h"
static int val_adjust(s16 *ptr, s8 fac, int val, int min, int max, u8 step)
{
    int ost=val*step;

    if(fac==1) {
        if(*ptr==max) {
            return -1;
        }
        else if(*ptr+ost > max) {
            *ptr = max;
        }
        else {
            *ptr += ost;
        }
    }
    else {
        if(*ptr==min) {
            return -1;
        }
        else if(*ptr+ost < min) {
            *ptr = min;
        }
        else {
            *ptr += ost;
        }
    }

    return 0;
}
int dsp_set_gain(u8 key, u16 times, dsp_info_t *info)
{
    int r,val;
    para_info_t *pinf;

    val = info->fac*times;
    if(info->dsp.id==CMD_ID_Delay) {
        pinf = (para_info_t*)&DELAY_INFO[info->dsp.ch];
    }
    else {
        pinf = (para_info_t*)&PARA_INFO[info->dsp.id].info[info->index];
    }

    r = val_adjust(info->pdata, info->fac, val, pinf->min, pinf->max, pinf->step);
    if(r) {
        return -1;
    }

    r = dsp_send(&info->dsp);

    return r;
}


int dsp_set_pitch(u8 key, dsp_info_t *info)
{
    int r;

    if(key==KEY_SHARP) {
        *info->pdata = (*info->pdata+1)%5;
    }
    else if(key==KEY_0) {
        *info->pdata = 0;
    }
    else {
        *info->pdata = (*info->pdata-1)%5;
    }

    r = dsp_send(&info->dsp);

    return r;
}


int dsp_set_preset(u8 key, dsp_info_t *info)
{
    int r;
    Dsp_Paras *dsp=&gParams.dsp;

    if(key==KEY_MODE) {
        *info->pdata = (*info->pdata+1)%PRESET_MAX;
    }
    else {
        *info->pdata = key-KEY_M1;
    }
    
    paras_read_preset(*info->pdata, dsp);
    r = dsp_download();

    return r;
}


int dsp_set_input(dsp_info_t *info)
{
    int r;

    *info->pdata = (*info->pdata+1)%INPUT_MAX;
    sys_set_input(*info->pdata);

    r = dsp_send(&info->dsp);

    return r;
}

int dsp_set_mute(dsp_info_t *info, u8 *m)
{
    u8 mute;
    int r,i;
    dsp_data_t dd={0};
    Dsp_Paras *dsp=&gParams.dsp;

    if((dsp->Array_Gain[Gain_CH_Music].Mute && dsp->Array_Gain[Gain_CH_Mic].Mute) || 
       (dsp->Array_Gain[Gain_CH_Music].Gain==0 && dsp->Array_Gain[Gain_CH_Mic].Gain==0)) {
        mute = 0;
    }
    else {
        mute = 1;
    }

    dd.id = CMD_ID_Gain;
    for(i=0; i<Gain_CH_Eff; i++) {
        dd.ch = i;
        dsp->Array_Gain[i].Mute = mute;
        dd.dlen = sizeof(TypeS_Gain);
        r = dsp_send(&dd);
    }
    if(r==0) {
        info->node.ptr = &dsp->Array_Gain;
        info->node.len = sizeof(dsp->Array_Gain);
        if(m) {
            *m = mute;
        }
    }
    
    return r;
}

/////////////////////////////////////////////////////////


void dsp_test(void)
{
    while(1) {
        //dsp_send(CMD_ID_DSPVer, 0, 0);
        //memset(&ver, 0, sizeof(ver));
        //usart_read(DSP_UART, (u8*)&ver, sizeof(ver));
        //delay_ms(1000);
    }
}



