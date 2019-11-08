#include "device.h"
#include "sys.h"
#include "usb_desc.h"
#include "usb_lib.h"

#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE   REPORT_COUNT

u8 usbRxBuf[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
u8 usbTxBuf[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
packet_t *pRx=(packet_t*)usbRxBuf;
packet_t *pTx=(packet_t*)usbTxBuf;

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
    //u32 tx_status = 0 ;
	//u32 tx_count  = 0 ;
	//u32 i = 0;
	//SetEPTxCount(ENDP2,0);

    UserToPMABufferCopy(data, ENDP2_TXADDR, len);
    SetEPTxCount(ENDP2, REPORT_COUNT);
    SetEPTxValid(ENDP2);

#if 0
    while(1) {
		tx_status = GetEPTxStatus(ENDP2);
		tx_count  = GetEPTxCount(ENDP2) ;
		
		if( (tx_status == 0x00000020) && (tx_count == 0x00000040)	)
			return 0 ;
		else 
			i++ ; 
		if(i>2000)
			return -1 ;
	}
#endif

    return 0;
}




int usbd_recv(void)
{
    if(!usbRxFlag) {
        PMAToUserBufferCopy(usbRxBuf, ENDP1_RXADDR, REPORT_COUNT);
        usbRxFlag = 1;
    }
    SetEPRxStatus(ENDP1, EP_RX_VALID);

#if 0//def APP
{
    evt_t e;

    e.evt = EVT_HID;
    e.ptr = pRx;

    if(is_multipkts(pRx)) {
        com_send_evt(&e);
    }
    else {
        dev_send_evt(&e);
    }
}
#endif
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

    usb_send(usbTxBuf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

    return 0;
}


int usbd_send_pkt(u8 type, void *data, u16 len, u16 pkts, u16 pid, u8 nck)
{
    pTx->type = type;
    pTx->nck  = nck;
    pTx->pkts  = pkts;
    pTx->pid  = pid;
    pTx->dlen = len;
    if (data && len<=USBD_CUSTOMHID_OUTREPORT_BUF_SIZE-sizeof(packet_t)) {
		memcpy(pTx->data, data, len);
	}

    return usb_send(usbTxBuf, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
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

    return 0;
}


