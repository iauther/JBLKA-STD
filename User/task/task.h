#ifndef __TASK_H__
#define __TASK_H__

#include "device.h"
#include "sys.h"
#include "evt.h"
#include "msg.h"
#include "cmsis_os2.h"

void gui_task(void *arg);
void gui_post_evt(evt_gui_t *e);
void gui_post_refresh(void);

void dev_task(void *arg);
void dev_post_evt(evt_dev_t *e);

void com_task(void *arg);
void com_post_evt(evt_com_t *e);

void eep_task(void *arg);
void eep_post_evt(evt_eep_t *e);

#endif
