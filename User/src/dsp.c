#include <string.h>
#include "dsp.h"
#include "e2p.h"
#include "usbd.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
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
    u16         buf[300];
    u16         ver;
}dsp_buf_t;


#define RX_BUF_LEN 100
u8 dspRxBuf[RX_BUF_LEN];


u8 dspBuf[40];
u8 dspStarted=0;
u8 upgrade_ack_flag=0;
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
    u16 sum;

    sum = 0;
    for (i = 0; i < length; i++) {
        sum += data[i];
    }

    return (sum & 0xFFFF);
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


static int dsp_write(dsp_buf_t *db)
{
    u16 i, crc;
    u16 *ptr = (u16*)db->cmd.DataPtr;

    db->buf[0] = 0xFF55;
    db->buf[1] = db->cmd.Len+ComHeadLen;
    db->buf[2] = db->cmd.ID;
    db->buf[3] = db->cmd.Ch;
    db->buf[4] = db->cmd.No;

    for(i=0; i<db->cmd.Len; i++)
        db->buf[5 + i] = *(ptr+i);

    crc = crc_calc(&db->buf[1], db->cmd.Len+ComHeadLen);
    db->buf[5 + db->cmd.Len] = crc;

    return usart_write(DSP_UART, (u8*)db->buf, sizeof(u16)*(6+db->cmd.Len));
}


static int dsp_read(dsp_buf_t *db)
{
    return 0;
}

/*
case CMD_ID_UpdataDSP:
case CMD_ID_Download:
cmd.Len = 128;//每次下载128个双字节，即256字节
cmd.No = No;//表示发送了第几个128个双字节
//cmd.DataPtr = buf;//buf用来存储128个双字节
*/
static int do_download(void *data, u16 len)
{
    u32 i,left,times;
    u16 *ptr = (u16*)data;

    left = len%DOWNLOAD_SIZE;
    times = len/DOWNLOAD_SIZE;

    gDspBuf.cmd.ID = CMD_ID_Download;
    gDspBuf.cmd.Len = DOWNLOAD_SIZE;
    for(i=0; i<times; i++) {
        gDspBuf.cmd.No = i;
        gDspBuf.cmd.DataPtr = ptr+i*DOWNLOAD_SIZE;
        dsp_write(&gDspBuf);
    }

    if(left>0) {
        gDspBuf.cmd.No = i;
        gDspBuf.cmd.Len = left;
        gDspBuf.cmd.DataPtr = ptr+i*DOWNLOAD_SIZE;
        dsp_write(&gDspBuf);
    }

    return 0;
}


void dsp_reset(void)
{
    GPIO_InitTypeDef init={0};
    
    dspStarted = 0;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_8;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &init);

    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
    delay_ms(10);
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
    delay_ms(10);

    
}

static void save_version(dsp_version_t *ver)
{
    u8 tmp[20];
    
    sprintf((char*)tmp, "V%04d", ver->ver);
    if(strcmp((char*)tmp, (char*)gParams.fw.dsp)) {
        strcpy((char*)gParams.fw.dsp, (char*)tmp);
        e2p_write(0, (u8*)&gParams.fw, sizeof(gParams.fw));
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
        upgrade_ack_flag = 1;
    }
}


int dsp_init(void)
{
    uart_paras_t para={dsp_rx_cb, dspRxBuf, RX_BUF_LEN};
    
    dspStarted = 0;
    dsp_reset();
    usart_init(DSP_UART, &para);
    do_download(&gParams.dsp, sizeof(gParams.dsp));

    return 0;
}


int dsp_download(void)
{
    do_download(&gParams.dsp, sizeof(gParams.dsp));

    return 0;
}

int dsp_reset_peq(eq_reset_t *rst)
{
    u8 i;
    int r;
    dsp_data_t dsp={0};

    r = paras_reset_peq(rst);
    if(r) {
        return -1;
    }

    dsp.ch = rst->ch;
    dsp.id = CMD_ID_EQ;
    for(i=0; i<MaxEQBand; i++) {
        dsp_send(&dsp);
    }

    return 0;
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

    r = get_node(dsp, &n);
    if(r) {
        return r;
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

    return dsp_write(&gDspBuf);
}


int dsp_upload(void *data, u16 len)
{
    return 0;
}





int dsp_version(void)
{
    dsp_data_t dsp={0};
    dsp.id = CMD_ID_DSPVer;
    dsp.dlen = 1;
    return dsp_send(&dsp);
}


int dsp_upgrade(u16 index, u8 *data, u16 len)
{
    if(len>DOWNLOAD_SIZE || len%2) {
        return -1;
    }

    gDspBuf.cmd.ID = CMD_ID_UpdataDSP;
    gDspBuf.cmd.Len = len;

    gDspBuf.cmd.No = index;
    gDspBuf.cmd.DataPtr = (u16*)data;

    upgrade_ack_flag = 0;
    dsp_write(&gDspBuf);
    while(upgrade_ack_flag==0);
    
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

    r = get_node(dsp, &n);
    if(r==0) {
        memcpy(n.ptr, dsp->data, n.len);
        *node = n;
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



