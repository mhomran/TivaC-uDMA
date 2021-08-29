#ifndef UDMA_H
#define UDMA_H
//includes
#include "udma_cfg.h"

//functions prototypes
void Udma_Init(const UdmaInitConfig_t* const Config);
void Udma_ConfigDst(UdmaChannel_t Channel, 
                    uint32_t DstEndAddress,
                    UdmaItemSize_t DstItemSize, 
                    UdmaInc_t DstInc);
void Udma_ConfigSrc(UdmaChannel_t Channel, 
                    uint32_t SrcEndAddress,
                    UdmaItemSize_t SrcItemSize, 
                    UdmaInc_t SrcInc);
void Udma_StartTransfer(UdmaChannel_t Channel, uint32_t DataSize);

#endif
