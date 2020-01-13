#ifndef __TASK_H__
#define __TASK_H__

#include "device.h"
#include "sys.h"
#include "evt.h"
#include "msg.h"
#include "cmsis_os2.h"

typedef void *handle_t;
typedef void (*thread_fn) (void *arg);

void gui_task(void *arg);
int gui_post_evt(evt_gui_t *e);
int gui_post_refresh(void);

void dev_task(void *arg);
int dev_post_evt(evt_dev_t *e);

void com_task(void *arg);
int com_post_evt(evt_com_t *e);

void eep_task(void *arg);
int eep_post_evt(evt_eep_t *e);

int task_init(void);
handle_t task_new(thread_fn fn, int stack_size);
void task_start(void);
#endif
