#ifndef __PACK_H__
#define __PACK_H__

#include "types.h"

#define UPG_MAGIC1           0xFF88AA99
#define UPG_MAGIC2           (~UPG_MAGIC1)

enum {
	FILE_BOOT = 0,
	FILE_DSP,
	FILE_MCU,
};

#pragma pack(1)
typedef struct {
	u32			magic1;      //UPG_MAGIC1
	u32			magic2;
	u8			ftype;      //file type
}head_t;
typedef struct {
	u8			md5[16];
	head_t      head;
	u8			data[];
}hdr_t;
#pragma pack()


int pack_make(char *type, char *path);

int pack_check(char *path);

#endif

