#ifndef __TASK_H__
#define __TASK_H__

#include "device.h"
#include "sys.h"
#include "evt.h"
#include "cmsis_os2.h"

void ui_task(void *arg);
void ui_post_evt(evt_ui_t *e);

void dev_task(void *arg);
void dev_post_evt(evt_dev_t *e);

void com_task(void *arg);
void com_post_evt(evt_com_t *e);

#endif
