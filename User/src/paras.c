#include <string.h>
#include "paras.h"
#include "e2p.h"
#include "default.h"
#include "config.h"

paras_data_t gParams;
paras_ui_t uiParams;
static void set_to_default(paras_ui_t *ui, paras_data_t *gb, const default_t *df)
{
    u8 i,j;
    
    //memset(gb, 0, sizeof(paras_data_t));
    for(i=0; i<Gain_CH_NUM; i++) {
        gb->dsp.Array_Gain[i] = df->gain;
    }

    for(i=0; i<Vol_CH_NUM; i++) {
        gb->dsp.Array_Vol[i] = df->vol;
    }
    
    for(i=0; i<EQ_CH_NUM; i++) {
        for(j=0; j<MaxEQBand; j++) {
            gb->dsp.Array_EQ[i].BandCoef[j] = df->eq;
        }
    }

    for(i=0; i<HLPF_CH_NUM; i++) {
        gb->dsp.Array_HLPF[i].HLPFCoef[0] = df->hlpf;
        gb->dsp.Array_HLPF[i].HLPFCoef[1] = df->hlpf;
    }
    
    for(i=0; i<Delay_CH_NUM; i++) {
        gb->dsp.Array_Delay[i] = df->delay;
    }
    
    for(i=0; i<FeedBack_CH_NUM; i++) {
        gb->dsp.Array_FeedBack[i] = df->feedback;
    }
    
    for(i=0; i<Limiter_CH_NUM; i++) {
        gb->dsp.Array_Limiter[i] = df->limiter;
    }

    for(i=0; i<Mute_CH_NUM; i++) {
        gb->dsp.Array_Mute[i] = df->mute;
    }

    for(i=0; i<NoiseGate_CH_NUM; i++) {
        gb->dsp.Array_NoiseGate[i] = df->noisegate;
    }
    
    //2GEQ
    for(i=0; i<2; i++) {
        ui->dsp.in.music.geq[i]->Freq = GEQ2_FREQ[i];
    }

    //3GEQ
    for(i=0; i<3; i++) {
        ui->dsp.in.mic.geq[i]->Freq = GEQ3_FREQ[i];
    }
    
    //3PEQ
    for(i=0; i<3; i++) {
        ui->dsp.eff.echo.peq[i]->Freq = PEQ3_FREQ[i];
        ui->dsp.eff.reverb.peq[i]->Freq = PEQ3_FREQ[i];
    }

    //7PEQ
    for(i=0; i<PEQ_BANDS; i++) {
        ui->dsp.in.music.peq[i]->Freq = PEQ7_FREQ[i];
        ui->dsp.in.mic.peq[i]->Freq = PEQ7_FREQ[i];
        ui->dsp.out.rec.peq[i]->Freq = PEQ7_FREQ[i];
        ui->dsp.out.sub.peq[i]->Freq = PEQ7_FREQ[i];
        ui->dsp.out.main.peq[i]->Freq = PEQ7_FREQ[i];
    }

    gb->dsp.Array_PitchShift = df->pitch;
    gb->dsp.Array_Input = df->input;
    gb->fw = FW_INFO;
    gb->iodat = IO_DATA;
    gb->pre = 0;
    
    
}


//0: write default paras to e2prom
//1: directly read paras form e2prom
static int check_version(void)
{
    fw_info_t fw;
    char *pver = (char*)&fw.ver1;
//return 0;    
    e2p_read(0, (u8*)&fw, sizeof(fw_info_t));
    pver[13] = 0;
    if(!strstr(pver, "KA-V") || strlen(pver)!=strlen(VERSION) || strcmp(pver, VERSION)<0) {
        return 0;
    }
    return 1;
}

static void paras_remap()
{
    dsp_remap(&uiParams.dsp, &gParams.dsp);
    uiParams.pfw = &gParams.fw;
    uiParams.pio = &gParams.iodat;
    uiParams.pre = &gParams.pre;
}


int paras_init(void)
{
    int r;
    e2p_init();

    paras_remap();
    if(!check_version()) {
        set_to_default(&uiParams, &gParams, &gDefault);
        r = e2p_write(0, (u8*)&gParams, sizeof(gParams));

        //the following used for test
        //memset(&gParams, 0, sizeof(gParams));
        //r = e2p_read(0, (u8*)&gParams, sizeof(gParams));
        //e2p_write preset data...  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    }
    else {
        r = e2p_read(0, (u8*)&gParams, sizeof(gParams));
    }

    return r;
}

static int reset_peq(u8 ch)
{
    int i;
    paras_ui_t *ui=&uiParams;

    switch(ch) {
        case EQ_CH_Music:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.in.music.peq[i] = gDefault.eq;
            ui->dsp.in.music.peq[i]->Freq = PEQ7_FREQ[i];
        }
        break;

        case EQ_CH_Mic:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.in.mic.peq[i] = gDefault.eq;
            ui->dsp.in.mic.peq[i]->Freq = PEQ7_FREQ[i];
        }
        break;

        case EQ_CH_Echo:
        for(i=0; i<3; i++) {
            *ui->dsp.eff.echo.peq[i] = gDefault.eq;
            ui->dsp.eff.echo.peq[i]->Freq = PEQ3_FREQ[i];
        }
        break;

        case EQ_CH_Rev:
        for(i=0; i<3; i++) {
            *ui->dsp.eff.reverb.peq[i] = gDefault.eq;
            ui->dsp.eff.reverb.peq[i]->Freq = PEQ3_FREQ[i];
        }
        break;

        case EQ_CH_Main:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.out.main.peq[i] = gDefault.eq;
            ui->dsp.out.main.peq[i]->Freq = PEQ7_FREQ[i];
        }
        break;

        case EQ_CH_Sub:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.out.sub.peq[i] = gDefault.eq;
            ui->dsp.out.sub.peq[i]->Freq = PEQ7_FREQ[i];
        }
        break;

        case EQ_CH_Rec:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.out.rec.peq[i] = gDefault.eq;
            ui->dsp.out.rec.peq[i]->Freq = PEQ7_FREQ[i];
        }
        break;

        default:
        return -1;
    }
    return 0;
}

int paras_reset_peq(eq_reset_t *rst)
{
    if(!rst) {
        return -1;
    }
    reset_peq(rst->ch);
    
    return 0;
}


int paras_default(void)
{
    int r;
    set_to_default(&uiParams, &gParams, &gDefault);
    r = e2p_write(0, (u8*)&gParams, sizeof(gParams));
    return r;
}


int paras_update(packet_t *pkt, node_t *node)
{
    int r=-1;
    node_t n={0};

    switch(pkt->type) {
        case TYPE_DSP:
        r = dsp_update((dsp_data_t*)pkt->data, &n);
        break;

        case TYPE_IODAT:
        n.ptr = &gParams.iodat;
        n.len = sizeof(gParams.iodat);
        if(pkt->dlen != n.len) return -1;
        memcpy(n.ptr, pkt->data, n.len);
        break;

        case TYPE_PARAS:
        n.ptr = &gParams;
        n.len = sizeof(gParams);
        if(pkt->dlen != n.len) return -1;
        memcpy(n.ptr, pkt->data, n.len);
        break;

        case TYPE_PRESET:
        //n.ptr = &gPreset;
        //n.len = sizeof(preset_t);
        if(pkt->dlen != n.len) return -1;
        memcpy(n.ptr, pkt->data, n.len);
        break;
        
        case TYPE_DEFAULT:
        r = 0;
        break;

        default:
        return -1;
    }
    
    if(r==0 && node) {
        *node = n;
    }

    return r;
}


int paras_read(void *p, int len)
{
    u32 offset=(u32)p-(u32)(&gParams);
    return e2p_read(offset, p, len);
}


int paras_write(void *p, int len)
{
    u32 offset=(u32)p-(u32)(&gParams);
    return e2p_write(offset, p, len);
}


int paras_reset(void)
{
    set_to_default(&uiParams, &gParams, &gDefault);
    return e2p_write(0, (u8*)&gParams, sizeof(paras_data_t));
}


int paras_save_preset(u8 index)
{
    u32 offset=sizeof(paras_data_t)+sizeof(u8)+sizeof(Dsp_Paras)*(index+1);
    return e2p_write(offset, (u8*)&gParams, sizeof(paras_data_t));
}

