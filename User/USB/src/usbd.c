#include <string.h>
#include "types.h"
#include "packet.h"
#include "usbd.h"
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
       pkt->type==TYPE_UPGRADE) {
        return 1;
    }

    if(pkt->type==TYPE_ACK) {
        ack_data_t *ack=(ack_data_t*)pkt->data;
        if(ack->type==TYPE_PRESET ||
            ack->type==TYPE_PARAS   || 
            ack->type==TYPE_UPGRADE) {
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
        SetEPRxStatus(ENDP1, EP_RX_VALID);
        usbRxFlag = 1;
    }

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

    *pTx = *p;
    pTx->type = TYPE_ACK;
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

