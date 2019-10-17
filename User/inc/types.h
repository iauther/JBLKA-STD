#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define s8      int8_t
#define u8      uint8_t     
#define s16     int16_t     
#define u16     uint16_t    
#define s32     int32_t     
#define u32     uint32_t
#define s64     int64_t
#define u64     uint64_t
#define f32     float
#define f64     double


typedef enum {
    MODE_IT,
    MODE_DMA,
    MODE_POLL,
}eMODE;


#ifdef __cplusplus
}
#endif

#endif

