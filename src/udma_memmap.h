#ifndef UDMA_MEMMAP_H
#define UDMA_MEMMAP_H

#include <inttypes.h>

#define SYSCTRL_BASE 0x400FE000
#define RCGCDMA_OFFSET 0x60C
#define PRDMA_OFFSET 0xA0C

#define UDMA_BASE 0x400FF000

#define UDMA ((volatile UdmaHandle_t*)UDMA_BASE)
#define RCGCDMA *((volatile uint32_t*)(SYSCTRL_BASE+RCGCDMA_OFFSET))
#define PRDMA *((volatile uint32_t*)(SYSCTRL_BASE+PRDMA_OFFSET))

typedef struct {
  uint32_t STAT;
  uint32_t CFG;
  uint32_t CTLBASE;
  uint32_t ALTBASE;
  uint32_t WAITSTAT;
  uint32_t SWREQ;
  uint32_t USEBURSTSET;
  uint32_t USEBURSTCLR;
  uint32_t REQMASKSET;
  uint32_t REQMASKCLR;
  uint32_t ENASET;
  uint32_t ENACLR;
  uint32_t ALTSET;
  uint32_t ALTCLR;
  uint32_t PRIOSET;
  uint32_t PRIOCLR;
  uint32_t ERRCLR;
  uint32_t RESERVED[0x12C];
  uint32_t CHASGN;
  uint32_t CHMAP[4];
} UdmaHandle_t;

#endif
