#ifndef __SYS_H__
#define __SYS_H__

#include "types.h"
#include "packet.h"

int sys_init(void);

int sys_config(void);

int sys_set_input(u16 input);

int sys_set_iodat(io_data_t *io);

int sys_set_default(void);


#endif
