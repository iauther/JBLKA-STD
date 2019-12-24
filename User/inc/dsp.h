#ifndef __DSP_H__
#define __DSP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

#define MaxEQBand					10
#define PRESET_MAX                  8
#define PEQ_BANDS                   7
#define DSP_MAX_PKT_SIZE            256

enum CMD_ID//命令ID
{
    CMD_ID_Gain = 1,//ID从1开始
    CMD_ID_Vol,
    CMD_ID_EQ,
    CMD_ID_HLPF,//高低通
    CMD_ID_Delay,
    CMD_ID_FeedBack,//反馈
    CMD_ID_Limiter,
    CMD_ID_PitchShift,//变调
    CMD_ID_Mute,
    CMD_ID_NoiseGate,//噪声门
    CMD_ID_Input,
    CMD_ID_DSPVer,

    CMD_ID_UpdataDSP = 0x10, //DSP升级, 兼容CBOX机型的升级指令
    CMD_ID_Upload,//上传数据
    CMD_ID_Download,//下载数据

    CMD_ID_NUM,//计算元素个数
};

enum Gain_CH//三个主音量
{
    Gain_CH_Music,
    Gain_CH_Mic,
    Gain_CH_Eff,

    Gain_CH_NUM,//计算元素个数
};

enum Vol_CH
{
    Vol_CH_Echo_Repeat,//回声重复比例0-90

    Vol_CH_Echo_EffVol,//回声效果音量  0-100
    Vol_CH_Echo_DirVol,//回声直达声音量0-100

    Vol_CH_Rev_EffVol,//混响效果音量0-100
    Vol_CH_Rev_DirVol,//混响直达声音量0-100

    Vol_CH_Main_MusicVol, //主输出音乐音量0-200
    Vol_CH_Main_DirVol, //主输出直达声音量0-200
    Vol_CH_Main_EchoVol,//0-100
    Vol_CH_Main_RevVol,//0-100

    Vol_CH_Sub_MusicVol,//0-200
    Vol_CH_Sub_DirVol,//0-200
    Vol_CH_Sub_EchoVol,//0-100
    Vol_CH_Sub_RevVol,//0-100

    Vol_CH_Rec_MusicVol,//0-200
    Vol_CH_Rec_DirVol,//0-200
    Vol_CH_Rec_EchoVol,//0-100
    Vol_CH_Rec_RevVol,//0-100

    Vol_CH_Main_LVol,//0-200
    Vol_CH_Main_RVol,//0-200
    Vol_CH_SubVol,//0-200
    Vol_CH_RecVol,//0-200

    Vol_CH_NUM,//计算元素个数
};

enum EQ_CH
{
    EQ_CH_Music,
    EQ_CH_Mic,
    EQ_CH_Echo,
    EQ_CH_Rev,

    EQ_CH_Main,
    EQ_CH_Sub,
    EQ_CH_Rec,

    EQ_CH_NUM,//计算元素个数
};


enum HLPF_CH
{
    HLPF_CH_Music,//音乐高低通
    HLPF_CH_Mic,
    HLPF_CH_Echo,
    HLPF_CH_Rev,

    HLPF_CH_Main,
    HLPF_CH_Sub,
    HLPF_CH_Rec,

    HLPF_CH_NUM,//计算元素个数
};

enum Delay_CH
{
    Delay_CH_Echo_PreDelay,//左回声预延时  0~4800 :    0~100ms
    Delay_CH_Echo_Delay,//左回声延时 //0~14400 :    0~300ms

    Delay_CH_Rev_PreDelay,//混响预延时 //0~4800 :    0~100ms
    Delay_CH_Rev_Time,  //混响时间0~8000 : 0~8000ms
    //以上 step=1

    //以下 step=0.1
    Delay_CH_MainL_Delay,//主输出左声道 //0~1920 :    0~40ms
    Delay_CH_MainR_Delay,//0~1920 :    0~40ms
    Delay_CH_Sub_Delay, //0~1920 :    0~40ms

    Delay_CH_NUM,//计算元素个数
};

enum FeedBack_CH
{
    FeedBack_CH_Mic,

    FeedBack_CH_NUM,//计算元素个数
};

enum Limiter_CH
{
    Limiter_CH_Mic,

    Limiter_CH_Main,
    Limiter_CH_Sub,
    Limiter_CH_Rec,

    Limiter_CH_NUM,//计算元素个数
};

enum EQ_Type
{
    EQ_Type_PEQ,
    EQ_Type_LS,
    EQ_Type_HS,

    EQ_Type_NUM,//计算元素个数
};

enum HLPF_Type
{
    HLPF_Type_Bypass,

    HLPF_Type_12dBButterworth,
    HLPF_Type_12dBBessel,
    HLPF_Type_12dBLinkRiley,

    HLPF_Type_18dBButterworth,
    HLPF_Type_18dBBessel,

    HLPF_Type_24dBButterworth,
    HLPF_Type_24dBBessel,
    HLPF_Type_24dBLinkRiley,

    HLPF_Type_NUM,//计算元素个数
};


enum HLPF_Hpf_Lpf
{
    HLPF_Hpf,
    HLPF_Lpf,

    HLPF_Hpf_Lpf_NUM,//计算元素个数
};

enum Mute_CH
{
    Mute_CH_MainL,
    Mute_CH_MainR,
    Mute_CH_Sub,
    Mute_CH_Rec,

    Mute_CH_NUM,//计算元素个数
};

enum NoiseGate_CH
{
    NoiseGate_CH_Music,
    NoiseGate_CH_Mic,

    NoiseGate_CH_NUM,//计算元素个数
};

//-----------------------------------------------------------------------------
//-----------------------------参数结构体定义------------------------------------
//-----------------------------------------------------------------------------
//#pragma pack(1)
typedef struct
{
    u16 Len;
    u16 ID;
    u16 Ch;
    u16 No;
    u16 *DataPtr;
}TypeS_CmdSt;


typedef struct
{
    u16  Gain;  //0~100
    u16  Mute;// 1:mute;  0:no mute
}TypeS_Gain;


typedef struct
{
    u16 Vol;    //0~200 step=1
    u16 Phase;//1:反相;    0:不反相
}TypeS_Vol;


typedef struct
{
    u16  Freq;//20~20000 :   20~20k     (step=1Hz)
    s16  Gain;//-24~+12dB          (step=0.1dB)
    u16  Q;   //1~1280 :   0.1~128.0  (step=0.1)
    u16  Type;  //0/1/2 :   PEQ / LS / HS
    u16  Bypass;// 0/1 : no bypass / bypass
}TypeS_EQBand;

typedef struct
{
    TypeS_EQBand BandCoef[MaxEQBand];
}TypeS_EQ;

typedef struct
{
    u16  Freq;//20~21000 :   20~21k     (step=1Hz)  小于20 或 高于20k 表示bypass高低通
    u16  Type;  //看HLPF_Type
}TypeS_HLPFCoef;

typedef struct
{
    TypeS_HLPFCoef HLPFCoef[2];
}TypeS_HLPF;

typedef struct
{
    u16 Delay;
}TypeS_Delay;

typedef struct
{
    u16  FeedBack;//移频量 0:off,  1~3:4Hz/6Hz/8Hz
}TypeS_FeedBack;

typedef struct
{
    s16  threshold;//启动电平 -34~+0dB
    u16  attackTime;//启动时间 0~900: 0~900ms     step=1
    u16  releaseTime;//释放时间0~9000: 0~9000ms   step=1
    u16  ratio;//压限比例 0~1000: 0~100.0  step=0.1
}TypeS_Limiter;


typedef struct
{
    s16  PitchShift;//变调 -5~+5
}TypeS_PitchShift;


typedef struct
{
    u16 Mute;   //0/1 : mute/unMute
}TypeS_Mute;

typedef struct
{
    s16  threshold;//启动阀值  -60~0dB

}TypeS_NoiseGate;

typedef struct
{
    u16  input;//0~2：ANALOG/HDMI_SPDIF/OPTICAL
}TypeS_Input;

typedef struct
{
    TypeS_Gain          Array_Gain[Gain_CH_NUM];
    TypeS_Vol           Array_Vol[Vol_CH_NUM];
    TypeS_EQ            Array_EQ[EQ_CH_NUM];
    TypeS_HLPF          Array_HLPF[HLPF_CH_NUM];
    TypeS_Delay         Array_Delay[Delay_CH_NUM];
    TypeS_FeedBack      Array_FeedBack[FeedBack_CH_NUM];
    TypeS_Limiter       Array_Limiter[Limiter_CH_NUM];
    TypeS_PitchShift    Array_PitchShift;
    TypeS_Mute          Array_Mute[Mute_CH_NUM];
    TypeS_NoiseGate     Array_NoiseGate[NoiseGate_CH_NUM];
    TypeS_Input         Array_Input;             
}Dsp_Paras;

typedef struct {
    Dsp_Paras           paras[PRESET_MAX];
}Pre_Paras;

////////////////////////new add by gh ////////////////////////////////

typedef struct {
    u16  hdr;
    u16  len;
    u16  id;
    u16  err;
    u16  crc;
}dsp_ack_t;

typedef struct {
    u16  hdr;
    u16  len;
    u16  id;
    u16  ver;
    u16  chr1;
    u16  chr2;
    u16  crc;
}dsp_version_t;

typedef struct {
    u16  hdr;
    u16  len;
    u16  id;
    u16  index;
    u16  flag;
    //data...
    //u16  crc;
}dsp_upgrade_t;


#pragma pack(1)
typedef struct {
    u8              id;                //refer to CMD_ID_XXX
    u8              ch;                //refer to XXX_CH
    u8              n;                 //only when id is EQ or HPLF, n is used(id==EQ, n: band, id==HPLF, 0:hpf,1:lpf)
    u16             dlen;
    u8              data[]; 
}dsp_data_t;

typedef struct {
    u8              ch;                 //eq channel, refer to EQ_CH
    TypeS_EQ        eq[];
}eq_reset_t;
#pragma pack()


typedef struct {
    TypeS_Gain          *gain;
    TypeS_EQ            *eq;
    TypeS_EQBand        *geq[3];
    TypeS_EQBand        *peq[7];
    TypeS_HLPFCoef      *hpf;
    TypeS_HLPFCoef      *lpf;
    TypeS_FeedBack      *feedback;
    TypeS_Limiter       *limiter;
    TypeS_NoiseGate     *noiseGate;
}mic_t;

typedef struct {
    TypeS_Gain          *gain;
    TypeS_EQ            *eq;
    TypeS_EQBand        *geq[2];
    TypeS_EQBand        *peq[7];
    TypeS_HLPFCoef      *hpf;
    TypeS_HLPFCoef      *lpf;
    TypeS_NoiseGate     *noiseGate;

    TypeS_Input         *input;
    TypeS_PitchShift    *pitch;
}music_t;


//////////////////////////////////////////
typedef struct {
    TypeS_EQ            *eq;
    TypeS_EQBand        *peq[3];

    TypeS_Vol           *repeat;
    TypeS_Vol           *effVol;
    TypeS_Vol           *dirVol;

    TypeS_HLPFCoef      *hpf;
    TypeS_HLPFCoef      *lpf;

    //以下step=1
    TypeS_Delay         *preDelay;      //0~4800 :    0~100ms
    TypeS_Delay         *delay;         //0~14400 :    0~300ms
}echo_t;

typedef struct {
    TypeS_EQ            *eq;
    TypeS_EQBand        *peq[3];

    TypeS_Vol           *effVol;
    TypeS_Vol           *dirVol;

    TypeS_HLPFCoef      *hpf;
    TypeS_HLPFCoef      *lpf;

    TypeS_Delay         *preDelay;       //混响预延时 //0~4800 :    0~100ms
    TypeS_Delay         *time;           //混响时间0~80 : 0~8000ms
}reverb_t;

/////////////////////////////////////////
typedef struct {
    TypeS_EQ            *eq;
    TypeS_EQBand        *peq[7];

    TypeS_HLPFCoef      *hpf;
    TypeS_HLPFCoef      *lpf;

    TypeS_Vol           *musicVol;
    TypeS_Vol           *dirVol;
    TypeS_Vol           *echoVol;
    TypeS_Vol           *reverbVol;
    TypeS_Vol           *vol;

    TypeS_Limiter       *limiter;
    TypeS_Mute          *mute;
}rec_t;

typedef struct {
    TypeS_EQ            *eq;
    TypeS_EQBand        *peq[7];

    TypeS_HLPFCoef      *hpf;
    TypeS_HLPFCoef      *lpf;

    TypeS_Delay         *delay;          //0~1920 :    0~40ms

    TypeS_Vol           *musicVol;
    TypeS_Vol           *dirVol;
    TypeS_Vol           *echoVol;
    TypeS_Vol           *reverbVol;
    TypeS_Vol           *vol;

    TypeS_Limiter       *limiter;
    TypeS_Mute          *mute;
}sub_t;

typedef struct {
    TypeS_EQ            *eq;
    TypeS_EQBand        *peq[7];

    TypeS_HLPFCoef      *hpf;
    TypeS_HLPFCoef      *lpf;

    TypeS_Vol           *musicVol;
    TypeS_Vol           *dirVol;
    TypeS_Vol           *echoVol;
    TypeS_Vol           *reverbVol;

    TypeS_Vol           *lVol;
    TypeS_Vol           *rVol;

    //以下step=0.1
    TypeS_Delay         *lDelay;         //主输出左声道 //0~1920 :    0~40ms
    TypeS_Delay         *rDelay;         //0~1920 :    0~40ms

    TypeS_Limiter       *limiter;
    TypeS_Mute          *L;
    TypeS_Mute          *R;
}main_t;


////////////////////////////////////////
typedef struct {
    music_t             music;
    mic_t               mic;

    TypeS_Gain          *effGain;
    echo_t              echo;
    reverb_t            reverb;
    
    sub_t               sub;
    rec_t               rec;
    main_t              main;

    u8                  *pIdx;     //used preset index
}dsp_paras_t;


typedef struct {
    s8          fac;
    u8          index;       //在结构体中的序号
    node_t      node;
    s16         *pdata;
    char        *title;
    char        *name;
    dsp_data_t  dsp;
}dsp_info_t;



//#pragma pack()

//////////////////////////////////////////////////////////////////
#define StructLen sizeof(Dsp_Paras)

void dsp_reset(void);

int dsp_init(void);

int dsp_reinit(void);

int dsp_reset_peq(eq_reset_t *rst, node_t *n);

int dsp_set_started(void);

int dsp_is_started(void);

int dsp_eq_reset(eq_reset_t *rst);

int dsp_default(Dsp_Paras *dsp);

int dsp_send(dsp_data_t *dsp);

int dsp_upload(void *data, u16 len);

int dsp_download(void);

int dsp_version(void);

int dsp_upgrade(u16 index, u8 *data, u16 len, u8 last);

void dsp_remap(dsp_paras_t *paras, Dsp_Paras *dsp);

int dsp_update(dsp_data_t *dsp, node_t *node);

int dsp_get_struct_len(u8 cmd);

u8* dsp_get_struct_ptr(u8 cmd);

u8 dsp_get_ch(u8 ch, void *p);

int dsp_get_info(u8 key, dsp_info_t *info);

int dsp_set_gain(u8 key, u16 times, dsp_info_t *info);

int dsp_set_pitch(u8 key, dsp_info_t *info);

int dsp_set_preset(u8 key, dsp_info_t *info);

int dsp_set_input(dsp_info_t *info);

int dsp_set_mute(dsp_info_t *info, u8 *m);

void dsp_test(void);

#ifdef __cplusplus
}
#endif


#endif



