#include "device.h"
#include "sys.h"
#include "task.h"
#include "usb_desc.h"
#include "usb_lib.h"

#define UQ_MAX      10


u8 usbRxBuf[REPORT_COUNT];
u8 usbTxBuf[REPORT_COUNT];
packet_t *pRx=(packet_t*)usbRxBuf;
packet_t *pTx=(packet_t*)usbTxBuf;
queue_t *uq=NULL;

u8 usbRxFlag=0;
int is_multipkts(packet_t *pkt)
{
    if(pkt->type==TYPE_PRESET  || 
       pkt->type==TYPE_PARAS   || 
       pkt->type==TYPE_UPGRADE ||
       pkt->type==TYPE_EQRESET) {
        return 1;
    }
    else if(pkt->type==TYPE_ACK) {
        ack_data_t *ack=(ack_data_t*)pkt->data;
        if(ack->type==TYPE_PRESET ||
            ack->type==TYPE_PARAS   || 
            ack->type==TYPE_UPGRADE ||
            ack->type==TYPE_EQRESET) {
            return 1;
        }
    }
    
    return 0;
}

static int usb_send(u8 *data, u16 len)
{
    UserToPMABufferCopy(data, ENDP2_TXADDR, len);
    SetEPTxCount(ENDP2, REPORT_COUNT);
    SetEPTxValid(ENDP2);

    return 0;
}


static int ufind(queue_t *q, int index, void *p1, void *p2)
{
    evt_com_t *e1=(evt_com_t*)p1;
    evt_com_t *e2=(evt_com_t*)p2;
    packet_t *pkt1=(packet_t*)e1->param.hid;
    packet_t *pkt2=(packet_t*)e2->param.hid;

    if(memcmp(pkt1, pkt2, sizeof(packet_t))==0) {
        return index;
    }

    return -1;
}

int usbd_recv(void)
{
#ifdef RTX
    {
        node_t n;
        evt_com_t e;
        e.evt = EVT_HID;
        PMAToUserBufferCopy(e.param.hid, ENDP1_RXADDR, REPORT_COUNT);
        n.ptr = &e;
        n.len = sizeof(e);
        queue_put(uq, &n, ufind);
    }
#else
    if(!usbRxFlag) {
        PMAToUserBufferCopy(usbRxBuf, ENDP1_RXADDR, REPORT_COUNT);
        usbRxFlag = 1;
    }
#endif
    SetEPRxStatus(ENDP1, EP_RX_VALID);

    return 0;
}

int usbd_send_ack(packet_t *p, int r)
{
    ack_data_t *ack=(ack_data_t*)pTx->data;
    
    pTx->type = TYPE_ACK;
    pTx->pkts = 1;
    pTx->pid  = p->pid;
    pTx->nck  = 0;
    pTx->dlen = sizeof(ack_data_t);
    ack->type = p->type;
    ack->err  = r;

    usb_send(usbTxBuf, REPORT_COUNT);

    return 0;
}


int usbd_send_pkt(u8 type, void *data, u16 len, u16 pkts, u16 pid, u8 nck)
{
    pTx->type = type;
    pTx->nck  = nck;
    pTx->pkts  = pkts;
    pTx->pid  = pid;
    pTx->dlen = len;
    if (data && len<=REPORT_COUNT-sizeof(packet_t)) {
		memcpy(pTx->data, data, len);
	}

    return usb_send(usbTxBuf, REPORT_COUNT);
}



static void usb_dp_reset(void)
{
    GPIO_InitTypeDef init={0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_12;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &init);

    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
    delay_ms(10);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    delay_ms(10);
}


static void usb_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Enable the USB interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
    /* Enable the USB clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

int usbd_init(void)
{
    usb_config();
    usb_dp_reset();
	USB_Init();

#ifdef RTX
    uq = queue_init(UQ_MAX, sizeof(evt_com_t));
#endif

    return 0;
}

int usbd_is_plugin(void)
{
    extern __IO uint32_t bDeviceState;
    return (bDeviceState==5);   //CONFIGURED
}

void usbd_tmr_cb(void)
{
#ifdef RTX
    {
        int r;
        node_t n;
        evt_com_t e;

        n.ptr = &e;
        n.len = sizeof(e);
        r = queue_get(uq, &n, NULL);
        if(r==0) {
            com_post_evt(&e);
        }
    }
#endif
}

