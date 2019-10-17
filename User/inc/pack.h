#ifndef __PACK_H__
#define __PACK_H__

#include "types.h"

#define UPG_FILE			 "upg.bin"
#define UPG_MAGIC1           0xFF88AA99
#define UPG_MAGIC2           (~UPG_MAGIC1)

enum {
	FILE_BOOT = 0,
	FILE_DSP,
	FILE_APP,
};

#pragma pack(1)
typedef struct {
	u8			chr[16];
}md5_t;
typedef struct {
	u32			magic1;      //UPG_MAGIC1
	u32			magic2;
	u8			ftype;      //file type
	u32         dlen;
	u8			data[];
}upg_t;
typedef struct {
	md5_t		md5;
	u32         dlen;
	u8			data[];		//upg_hdr_t
}hdr_t;
#pragma pack()


int pack_make(char *type, char *path);

int pack_check(char *path);

#endif

