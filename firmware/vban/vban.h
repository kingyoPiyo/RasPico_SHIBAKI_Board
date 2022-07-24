/********************************************************
* Title    : VBAN.h
* Date     : 2022/07/25
* Design   : kingyo
* Ref      : https://vb-audio.com/Voicemeeter/VBANProtocol_Specifications.pdf
********************************************************/
#ifndef __VBAN_H__
#define __VBAN_H__

#include <stdint.h>

#define DEF_VBAN_HEAD_SIZE      (28)
#define DEF_VBAN_PCM_SIZE       (512)
#define DEF_VBAN_PORTNUM        (6980)

struct tagVBAN_HEADER
{
    uint32_t vban;          // contains 'V' 'B', 'A', 'N'
    uint8_t format_SR;      // SR index (see SRList above)
    uint8_t format_nbs;     // nb sample per frame (1 to 256)
    uint8_t format_nbc;     // nb channel (1 to 256)
    uint8_t format_bit;     // mask = 0x07 (see DATATYPE table below)
    char streamname[16];    // stream name
    uint32_t nuFrame;       // growing frame number
} __attribute__((packed));

typedef struct tagVBAN_HEADER T_VBAN_HEADER;

void vban_init(void);
void vban_main(void);

#endif //__VBAN_H__
