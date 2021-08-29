//includes
#include "udma_cfg.h"

const UdmaConfig_t UdmaConfig[] = 
{
  {
    .Channel = UDMA_CHANNEL_8,
    .Encoding = UDMA_ENCODING_0,
    .Priority = UDMA_PRIORITY_DEFAULT,
    .Mode = UDMA_MODE_BASIC,
    .RequestType = UDMA_REQ_SINGLE_BURST,
    .ArbSize = UDMA_ARB_SIZE_8
  },
  {
    .Channel = UDMA_CHANNEL_9,
    .Encoding = UDMA_ENCODING_0,
    .Priority = UDMA_PRIORITY_DEFAULT,
    .Mode = UDMA_MODE_BASIC,
    .RequestType = UDMA_REQ_SINGLE_BURST,
    .ArbSize = UDMA_ARB_SIZE_8
  }
}; 

const UdmaInitConfig_t UdmaInitConfig = 
{
  sizeof(UdmaConfig)/sizeof(UdmaConfig_t),
  UdmaConfig
};

const UdmaInitConfig_t* 
Udma_GetInitConfig(void)
{
  return &UdmaInitConfig;
}
