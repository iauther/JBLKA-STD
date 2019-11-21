#include <string.h>
#include "paras.h"
#include "e2p.h"
#include "default.h"
#include "config.h"

paras_data_t gParams;
paras_ui_t uiParams;
static void set_to_default(paras_ui_t *ui, const def_dsp_t *def)
{
    fw_info_t fw;
    
    e2p_read(0, (u8*)&fw, sizeof(fw_info_t));
    strcpy((char*)fw.mcu, (char*)FW_INFO.mcu);
    strcpy((char*)fw.model, (char*)FW_INFO.model);

    *ui->dsp.music.gain = def->music.gain;
    *ui->dsp.music.eq = def->music.eq;
    *ui->dsp.music.hpf = def->music.hpf;
    *ui->dsp.music.lpf = def->music.lpf;
    *ui->dsp.music.noiseGate = def->music.noiseGate;
    *ui->dsp.music.pitch = def->music.pitch;
    *ui->dsp.music.input = def->music.input;

    *ui->dsp.mic.gain = def->mic.gain;
    *ui->dsp.mic.eq = def->mic.eq;
    *ui->dsp.mic.hpf = def->mic.hpf;
    *ui->dsp.mic.lpf = def->mic.lpf;
    *ui->dsp.mic.feedback = def->mic.feedback;
    *ui->dsp.mic.limiter = def->mic.limiter;
    *ui->dsp.mic.noiseGate = def->mic.noiseGate;

    *ui->dsp.effGain = def->effGain;
    *ui->dsp.echo.eq = def->echo.eq;
    *ui->dsp.echo.hpf = def->echo.hpf;
    *ui->dsp.echo.lpf = def->echo.lpf;
    *ui->dsp.echo.repeat = def->echo.repeat;
    *ui->dsp.echo.effVol = def->echo.effVol;
    *ui->dsp.echo.dirVol = def->echo.dirVol;
    *ui->dsp.echo.preDelay = def->echo.preDelay;
    *ui->dsp.echo.delay = def->echo.delay;

    *ui->dsp.reverb.eq = def->reverb.eq;
    *ui->dsp.reverb.hpf = def->reverb.hpf;
    *ui->dsp.reverb.lpf = def->reverb.lpf;
    *ui->dsp.reverb.effVol = def->reverb.effVol;
    *ui->dsp.reverb.dirVol = def->reverb.dirVol;
    *ui->dsp.reverb.preDelay = def->reverb.preDelay;
    *ui->dsp.reverb.time = def->reverb.time;
    
    *ui->dsp.sub.eq = def->sub.eq;
    *ui->dsp.sub.hpf = def->sub.hpf;
    *ui->dsp.sub.lpf = def->sub.lpf;
    *ui->dsp.sub.musicVol = def->sub.musicVol;
    *ui->dsp.sub.dirVol = def->sub.dirVol;
    *ui->dsp.sub.echoVol = def->sub.echoVol;
    *ui->dsp.sub.reverbVol = def->sub.reverbVol;
    *ui->dsp.sub.vol = def->sub.vol;
    *ui->dsp.sub.limiter = def->sub.limiter;
    *ui->dsp.sub.delay = def->sub.delay;

    *ui->dsp.rec.eq = def->rec.eq;
    *ui->dsp.rec.hpf = def->rec.hpf;
    *ui->dsp.rec.lpf = def->rec.lpf;
    *ui->dsp.rec.musicVol = def->rec.musicVol;
    *ui->dsp.rec.dirVol = def->rec.dirVol;
    *ui->dsp.rec.echoVol = def->rec.echoVol;
    *ui->dsp.rec.reverbVol = def->rec.reverbVol;
    *ui->dsp.rec.vol = def->rec.vol;
    *ui->dsp.rec.limiter = def->rec.limiter;

    *ui->dsp.main.eq = def->main.eq;
    *ui->dsp.main.hpf = def->main.hpf;
    *ui->dsp.main.lpf = def->main.lpf;
    *ui->dsp.main.musicVol = def->main.musicVol;
    *ui->dsp.main.dirVol = def->main.dirVol;
    *ui->dsp.main.echoVol = def->main.echoVol;
    *ui->dsp.main.reverbVol = def->main.reverbVol;
    *ui->dsp.main.lVol = def->main.lVol;
    *ui->dsp.main.rVol = def->main.rVol;
    *ui->dsp.main.lDelay = def->main.lDelay;
    *ui->dsp.main.rDelay = def->main.rDelay;
    *ui->dsp.main.limiter = def->main.limiter;
    *ui->dsp.main.L = def->main.L;
    *ui->dsp.main.R = def->main.R;

    *ui->pfw = fw;
    *ui->pio = IO_DATA;
    *ui->pre = 0;
}


//0: write default paras to e2prom
//1: directly read paras form e2prom
static int check_version(void)
{
    fw_info_t fw;
    
    e2p_read(0, (u8*)&fw, sizeof(fw_info_t));
    fw.mcu[9] = 0;
    if(strcmp((char*)fw.mcu, MCU_VERSION)) {     //版本不同，需要重置参数
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
        set_to_default(&uiParams, &dspDefault);
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
            *ui->dsp.music.peq[i] = dspDefault.music.eq.BandCoef[2+i];
        }
        break;

        case EQ_CH_Mic:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.mic.peq[i] = dspDefault.mic.eq.BandCoef[3+i];
        }
        break;

        case EQ_CH_Echo:
        for(i=0; i<3; i++) {
            *ui->dsp.echo.peq[i] = dspDefault.echo.eq.BandCoef[i];
        }
        break;

        case EQ_CH_Rev:
        for(i=0; i<3; i++) {
            *ui->dsp.reverb.peq[i] = dspDefault.reverb.eq.BandCoef[i];
        }
        break;

        case EQ_CH_Main:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.main.peq[i] = dspDefault.main.eq.BandCoef[i];
        }
        break;

        case EQ_CH_Sub:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.sub.peq[i] = dspDefault.sub.eq.BandCoef[i];
        }
        break;

        case EQ_CH_Rec:
        for(i=0; i<PEQ_BANDS; i++) {
            *ui->dsp.rec.peq[i] = dspDefault.rec.eq.BandCoef[i];
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
    set_to_default(&uiParams, &dspDefault);
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
        //if(pkt->dlen != n.len) return -1;
        memcpy(n.ptr, pkt->data, n.len);
        break;
        
        default:
        r = 0;
        break;
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
    set_to_default(&uiParams, &dspDefault);
    return e2p_write(0, (u8*)&gParams, sizeof(paras_data_t));
}


int paras_read_preset(u8 index, Dsp_Paras *dsp)
{
    if(index>=PRESET_MAX || !dsp) {
        return -1;
    }

    u32 offset=sizeof(paras_data_t)+index*sizeof(Dsp_Paras);
    return e2p_read(offset, (u8*)dsp, sizeof(Dsp_Paras));
}


int paras_write_preset(u8 index, Dsp_Paras *dsp)
{
    if(index>=PRESET_MAX || !dsp) {
        return -1;
    }

    u32 offset=sizeof(paras_data_t)+index*sizeof(Dsp_Paras);
    return e2p_write(offset, (u8*)dsp, sizeof(Dsp_Paras));
}

