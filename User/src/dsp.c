#include <string.h>
#include "dsp.h"
#include "usbd.h"
#include "usart.h"
#include "delay.h"
#include "packet.h"
#include "config.h"
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


u8 dspBuf[40];
u8 dspStarted=0;
extern paras_data_t gParams;;

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

    return usart4_write((u8*)db->buf, sizeof(u16)*(6+db->cmd.Len));
}


static int dsp_read(dsp_buf_t *db)
{
    return 0;
}


void dsp_reset(void)
{
    GPIO_InitTypeDef init={0};
	init.GPIO_Mode = GPIO_Mode_Out_PP;
	init.GPIO_Pin = GPIO_Pin_8;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &init);

    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
    delay_ms(10);
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
    delay_ms(10);

    dsp_download(&gParams.dsp, sizeof(gParams.dsp));
}


static int dsp_rx_cb(u8 *data, u16 len)
{
    if(len==4) {
        if(*(u32*)data==STARTUP_CODE) {
            dspStarted = 1;
        }
    }
    else if(len==sizeof(dsp_version_t)) {
        dsp_version_t *ver=(dsp_version_t*)data;
        
    }
    else if(len==sizeof(dsp_ack_t)){
        dsp_ack_t *ack=(dsp_ack_t*)data;
        
    }
    return 0;
}

int dsp_init(void)
{
    usart4_init(dsp_rx_cb);
    dsp_reset();

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
int dsp_send(u16 ID, u16 Ch, u16 No)
{
    Dsp_Paras *dsp=&gParams.dsp;
    gDspBuf.cmd.ID = ID;
    gDspBuf.cmd.Ch = Ch;
    switch(ID) {
        case CMD_ID_Gain:
        gDspBuf.cmd.Len = sizeof(TypeS_Gain);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_Gain[Ch];
        break;

        case CMD_ID_Amp:
        gDspBuf.cmd.Len = sizeof(TypeS_Vol);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_Vol[Ch];
        break;

        case CMD_ID_EQ:
        gDspBuf.cmd.Len = sizeof(TypeS_EQBand);
        gDspBuf.cmd.No = No;
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_EQ[Ch].BandCoef[ No ];
        break;

        case CMD_ID_HLPF:
        gDspBuf.cmd.Len = sizeof(TypeS_HLPFCoef);
        gDspBuf.cmd.No = No;
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_HLPF[Ch].HLPFCoef[ No ];
        break;

        case CMD_ID_Delay:
        gDspBuf.cmd.Len = sizeof(TypeS_Delay);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_Delay[Ch];
        break;

        case CMD_ID_FeedBack:
        gDspBuf.cmd.Len = sizeof(TypeS_FeedBack);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_FeedBack[Ch];
        break;

        case CMD_ID_Limiter:
        gDspBuf.cmd.Len = sizeof(TypeS_Limiter);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_Limiter[Ch];
        break;

        case CMD_ID_PitchShift:
        gDspBuf.cmd.Len = sizeof(TypeS_PitchShift);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_PitchShift;
        break;

        case CMD_ID_Mute:
        gDspBuf.cmd.Len = sizeof(TypeS_Mute);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_Mute[Ch];
        break;

        case CMD_ID_NoiseGate:
        gDspBuf.cmd.Len = sizeof(TypeS_NoiseGate);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_NoiseGate[Ch];
        break;

        case CMD_ID_Input:
        gDspBuf.cmd.Len = sizeof(TypeS_Input);
        gDspBuf.cmd.DataPtr = (u16 *)&dsp->Array_Input;
        break;

        default:
        return -1;
    }

    return dsp_write(&gDspBuf);
}


int dsp_upload(void *data, u16 len)
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
int dsp_download(void *data, u16 len)
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


int dsp_upgrade(u16 index, u8 *data, u16 len)
{
    if(len>DOWNLOAD_SIZE || len%2) {
        return -1;  //even length || len<=256
    }

    gDspBuf.cmd.ID = CMD_ID_UpdataDSP;
    gDspBuf.cmd.Len = len;

    gDspBuf.cmd.No = index;
    gDspBuf.cmd.DataPtr = (u16*)data;
    dsp_write(&gDspBuf);
    
    return 0;
}


void dsp_remap(dsp_paras_t *paras, Dsp_Paras *dsp)
{
    u8 i;
    
    paras->in.music.gain = &dsp->Array_Gain[Gain_CH_Music];
    for(i=0; i<2; i++) {
        paras->in.music.geq[i] = &dsp->Array_EQ[EQ_CH_Music].BandCoef[i];
    }
    for(i=2; i<PEQ_BANDS+2; i++) {
        paras->in.music.peq[i-2] = &dsp->Array_EQ[EQ_CH_Music].BandCoef[i];
    }
    paras->in.music.hpf = &dsp->Array_HLPF[HLPF_CH_Music].HLPFCoef[HLPF_Hpf];
    paras->in.music.lpf = &dsp->Array_HLPF[HLPF_CH_Music].HLPFCoef[HLPF_Lpf];
    paras->in.music.noiseGate = &dsp->Array_NoiseGate[NoiseGate_CH_Music];
    paras->in.music.input = &dsp->Array_Input;
    paras->in.music.pitch = &dsp->Array_PitchShift;

    paras->in.mic.gain = &dsp->Array_Gain[Gain_CH_Mic];
    for(i=0; i<3; i++) {
        paras->in.mic.geq[i] = &dsp->Array_EQ[EQ_CH_Mic].BandCoef[i];
    }
    for(i=3; i<PEQ_BANDS+3; i++) {
        paras->in.mic.peq[i-3] = &dsp->Array_EQ[EQ_CH_Mic].BandCoef[i];
    }
    paras->in.mic.hpf = &dsp->Array_HLPF[HLPF_CH_Mic].HLPFCoef[HLPF_Hpf];
    paras->in.mic.lpf = &dsp->Array_HLPF[HLPF_CH_Mic].HLPFCoef[HLPF_Lpf];
    paras->in.mic.feedback = &dsp->Array_FeedBack[FeedBack_CH_Mic];
    paras->in.mic.limiter = &dsp->Array_Limiter[Limiter_CH_Mic];
    paras->in.mic.noiseGate = &dsp->Array_NoiseGate[NoiseGate_CH_Mic];
    
    

    for(i=0; i<PEQ_BANDS; i++) {
        paras->out.rec.peq[i] = &dsp->Array_EQ[EQ_CH_Rec].BandCoef[i];
    }
    paras->out.rec.musicVol = &dsp->Array_Vol[Vol_CH_Rec_MusicVol];
    paras->out.rec.dirVol = &dsp->Array_Vol[Vol_CH_Rec_DirVol];
    paras->out.rec.echoVol = &dsp->Array_Vol[Vol_CH_Rec_EchoVol];
    paras->out.rec.reverbVol = &dsp->Array_Vol[Vol_CH_Rec_RevVol];
    paras->out.rec.limiter = &dsp->Array_Limiter[Limiter_CH_Rec];
    paras->out.rec.mute = &dsp->Array_Mute[Mute_CH_Rec];

    for(i=0; i<PEQ_BANDS; i++) {
        paras->out.sub.peq[i] = &dsp->Array_EQ[EQ_CH_Sub].BandCoef[i];
    }
    paras->out.sub.delay = &dsp->Array_Delay[Delay_CH_Sub_Delay];
    paras->out.sub.musicVol = &dsp->Array_Vol[Vol_CH_Sub_MusicVol];
    paras->out.sub.dirVol = &dsp->Array_Vol[Vol_CH_Sub_DirVol];
    paras->out.sub.echoVol = &dsp->Array_Vol[Vol_CH_Sub_EchoVol];
    paras->out.sub.reverbVol = &dsp->Array_Vol[Vol_CH_Sub_RevVol];
    paras->out.sub.limiter = &dsp->Array_Limiter[Limiter_CH_Sub];
    paras->out.sub.mute = &dsp->Array_Mute[Mute_CH_Sub];

    for(i=0; i<PEQ_BANDS; i++) {
        paras->out.main.peq[i] = &dsp->Array_EQ[EQ_CH_Main].BandCoef[i];
    }
    paras->out.main.musicVol = &dsp->Array_Vol[Vol_CH_Main_MusicVol];
    paras->out.main.dirVol = &dsp->Array_Vol[Vol_CH_Main_DirVol];
    paras->out.main.echoVol = &dsp->Array_Vol[Vol_CH_Main_EchoVol];
    paras->out.main.reverbVol = &dsp->Array_Vol[Vol_CH_Main_RevVol];
    paras->out.main.lVol = &dsp->Array_Vol[Vol_CH_Main_LVol];
    paras->out.main.rVol = &dsp->Array_Vol[Vol_CH_Main_RVol];
    paras->out.main.lDelay = &dsp->Array_Delay[Delay_CH_MainL_Delay];
    paras->out.main.rDelay = &dsp->Array_Delay[Delay_CH_MainR_Delay];
    paras->out.main.limiter = &dsp->Array_Limiter[Limiter_CH_Main];
    paras->out.main.L = &dsp->Array_Mute[Mute_CH_MainL];
    paras->out.main.R = &dsp->Array_Mute[Mute_CH_MainR];

    paras->eff.gain = &dsp->Array_Gain[Gain_CH_Eff];
    for(i=0; i<3; i++) {
        paras->eff.echo.peq[i] = &dsp->Array_EQ[EQ_CH_Echo].BandCoef[i];
    }
    paras->eff.echo.repeat = &dsp->Array_Vol[Vol_CH_Echo_Repeat];
    paras->eff.echo.effVol = &dsp->Array_Vol[Vol_CH_Echo_EffVol];
    paras->eff.echo.dirVol = &dsp->Array_Vol[Vol_CH_Echo_DirVol];
    paras->eff.echo.hpf = &dsp->Array_HLPF[HLPF_CH_Echo].HLPFCoef[HLPF_Hpf];
    paras->eff.echo.lpf = &dsp->Array_HLPF[HLPF_CH_Echo].HLPFCoef[HLPF_Lpf];
    paras->eff.echo.preDelay = &dsp->Array_Delay[Delay_CH_Echo_PreDelay];
    paras->eff.echo.delay = &dsp->Array_Delay[Delay_CH_Echo_Delay];
    
    for(i=0; i<3; i++) {
        paras->eff.reverb.peq[i] = &dsp->Array_EQ[EQ_CH_Rev].BandCoef[i];
    }
    paras->eff.reverb.effVol = &dsp->Array_Vol[Vol_CH_Rev_EffVol];
    paras->eff.reverb.dirVol = &dsp->Array_Vol[Vol_CH_Rev_EffVol];
    paras->eff.echo.hpf = &dsp->Array_HLPF[HLPF_CH_Rev].HLPFCoef[HLPF_Hpf];
    paras->eff.echo.lpf = &dsp->Array_HLPF[HLPF_CH_Rev].HLPFCoef[HLPF_Lpf];
    paras->eff.reverb.preDelay = &dsp->Array_Delay[Delay_CH_Rev_PreDelay];
    paras->eff.reverb.time = &dsp->Array_Delay[Delay_CH_Rev_Time];
}

/////////////////////////////////////////////////////////

dsp_version_t ver;
void dsp_test(void)
{
    while(1) {
        dsp_send(CMD_ID_DSPVer, 0, 0);
        //memset(&ver, 0, sizeof(ver));
        //usart4_read((u8*)&ver, sizeof(ver));
        delay_ms(1000);
    }
}



