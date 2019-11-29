#ifndef __SYS_H__
#define __SYS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "font.h"
#include "rbuf.h"
#include "msg.h"
#include "lock.h"
#include "default.h"
#include "queue.h"
#include "delay.h"
#include "paras.h"
#include "packet.h"
#include "gbl.h"
#include "config.h"

enum {
    STATE_PC_TUNING,
    STATE_UPGRADING,
};


int sys_init(void);

int sys_audio_init(void);

int sys_set_input(u16 input);

int sys_set_iodat(io_data_t *io);

int sys_mute(u8 on);

int sys_standby(u8 on);

void sys_reboot(void);

#endif
