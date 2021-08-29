//includes
#include "udma.h"
#include "udma_memmap.h"

//defines
#define UDMA_CHCTL_XFERMODE_Pos 0
#define UDMA_CHCTL_XFERMODE_Msk (0x7 << UDMA_CHCTL_XFERMODE_Pos)
#define UDMA_CHCTL_NXTUSEBURST_Pos 3
#define UDMA_CHCTL_XFERSIZE_Pos 4
#define UDMA_CHCTL_XFERSIZE_Msk (0x3FF << UDMA_CHCTL_XFERSIZE_Pos)
#define UDMA_CHCTL_ARBSIZE_Pos 14
#define UDMA_CHCTL_ARBSIZE_Msk (0xF << UDMA_CHCTL_ARBSIZE_Pos)
#define UDMA_CHCTL_SRCPROT0_Pos 18
#define UDMA_CHCTL_DSTPROT0_Pos 21
#define UDMA_CHCTL_SRCSIZE_Pos 24
#define UDMA_CHCTL_SRCSIZE_Msk (0x3 << UDMA_CHCTL_SRCSIZE_Pos)
#define UDMA_CHCTL_SRCINC_Pos 26
#define UDMA_CHCTL_SRCINC_Msk (0x3 << UDMA_CHCTL_SRCINC_Pos)
#define UDMA_CHCTL_DSTSIZE_Pos 28
#define UDMA_CHCTL_DSTSIZE_Msk (0x3 << UDMA_CHCTL_DSTSIZE_Pos)
#define UDMA_CHCTL_DSTINC_Pos 30
#define UDMA_CHCTL_DSTINC_Msk (0x3 << UDMA_CHCTL_DSTINC_Pos)

//typedefs
typedef struct {
  uint32_t SRCENDP;
  uint32_t DSTENDP;
  uint32_t CHCTL;
  uint32_t RESERVED;
} ChCtrlTableEntry_t;

//Module variables
static uint8_t gChCtrlTableBuff[1024 * 2];
static ChCtrlTableEntry_t* gChCtrlTable;
static const UdmaInitConfig_t* gUdmaInitConfig;

//prototypesg
inline static void Udma_EnableClk(void);
inline static void Udma_SetupChCtrlTable(void);
inline static void Udma_SetEncoding(UdmaChannel_t Channel, UdmaEncoding_t Encoding);
inline static void Udma_SetPriority(UdmaChannel_t Channel, UdmaPriority_t Priority);
inline static void Udma_SetMode(UdmaChannel_t Channel, UdmaMode_t Mode);
inline static void Udma_SetRequestType(UdmaChannel_t Channel, UdmaReq_t RequestType);
inline static void Udma_SetArbSize(UdmaChannel_t Channel, UdmaArbSize_t ArbSize);
inline static void Udma_EnableChannel(UdmaChannel_t Channel);
inline static void Udma_SetXferSize(UdmaChannel_t Channel, uint32_t DataSize);

//definitions
void 
Udma_Init(const UdmaInitConfig_t* const Config)
{
  uint8_t UdmaConfigSize = Config->UdmaConfigSize;
  const UdmaConfig_t* Channels = Config->UdmaConfig;
  uint8_t i;

  gUdmaInitConfig = Config;

  Udma_EnableClk();
  UDMA->CFG |= 1;
  Udma_SetupChCtrlTable();

  for(i = 0; i < UdmaConfigSize; i++)
    {
      Udma_SetEncoding(Channels[i].Channel, Channels[i].Encoding);
      Udma_SetPriority(Channels[i].Channel, Channels[i].Priority);
      Udma_SetMode(Channels[i].Channel, Channels[i].Mode);
      Udma_SetRequestType(Channels[i].Channel, Channels[i].RequestType);
      Udma_SetArbSize(Channels[i].Channel, Channels[i].ArbSize);
      Udma_EnableChannel(Channels[i].Channel);
    }
}

/**
 * @brief Power up the uDMA peripheral
 * 
 */
inline static void 
Udma_EnableClk(void)
{
  uint16_t delay;
  if((RCGCDMA & 0x1) == 0) 
    {
      RCGCDMA |= 1;
    }
  for(delay = 0xFFFF; delay != 0; delay--);
}

/**
 * @brief Setup the channel control table that hold the transfer settings
 * 
 */
inline static void 
Udma_SetupChCtrlTable(void)
{
  uint16_t i;
  for(i = 0; i < 1024; i++)
    {
      uint32_t address = ((uint32_t)(gChCtrlTableBuff + i));
      if((address % 1024) == 0)
        {
          break;
        }
    }
  gChCtrlTable = (ChCtrlTableEntry_t*)(gChCtrlTableBuff + i);

  UDMA->CTLBASE = (uint32_t)gChCtrlTable;
}

/**
 * @brief Map the channel to a a peripheral or Software
 * 
 * @param Channel 
 * @param Encoding 
 */
inline static void 
Udma_SetEncoding(UdmaChannel_t Channel, UdmaEncoding_t Encoding)
{
  uint8_t RegNo = Channel / 4;
  uint8_t Position = Channel % 4;
  UDMA->CHMAP[RegNo] |= Encoding << Position;
}

/**
 * @brief Set the channel priority as default or high
 * 
 * @param Channel 
 * @param Priority 
 */
inline static void 
Udma_SetPriority(UdmaChannel_t Channel, UdmaPriority_t Priority)
{
  if(Priority == UDMA_PRIORITY_DEFAULT)
    {
      UDMA->PRIOCLR = 1 << Channel;
    }
  else 
    {
      UDMA->PRIOSET = 1 << Channel;
    }
}

/**
 * @brief Setup the transfer mode (basic, ping pong, etc.)
 * 
 * @param Channel 
 * @param Mode 
 */
inline static void 
Udma_SetMode(UdmaChannel_t Channel, UdmaMode_t Mode)
{
  gChCtrlTable[Channel].CHCTL &= ~(UDMA_CHCTL_XFERMODE_Msk);
  gChCtrlTable[Channel].CHCTL |= (Mode << UDMA_CHCTL_XFERMODE_Pos);

  if(Mode == UDMA_MODE_STOP || Mode == UDMA_MODE_BASIC || Mode == UDMA_MODE_AUTO)
    {
      UDMA->ALTCLR = 1 << Channel;
    } 
  else
    {
      UDMA->ALTSET = 1 << Channel;
    }
}

/**
 * @brief Set the type of request (single, burst)
 * 
 * @param Channel 
 * @param RequestType 
 */
inline static void 
Udma_SetRequestType(UdmaChannel_t Channel, UdmaReq_t RequestType)
{
  if(RequestType == UDMA_REQ_BURST_ONLY)
    {
      UDMA->USEBURSTSET = 1 << Channel; 
    }
  else
    {
      UDMA->USEBURSTCLR = 1 << Channel; 
    }
}

/**
 * @brief Set the arbitration size (after how many items should the arbitration
 * happens). It's also can be thought of as burst size.
 * @param Channel 
 * @param ArbSize 
 */
inline static void 
Udma_SetArbSize(UdmaChannel_t Channel, UdmaArbSize_t ArbSize)
{
  gChCtrlTable[Channel].CHCTL &= ~(UDMA_CHCTL_ARBSIZE_Msk);
  gChCtrlTable[Channel].CHCTL |= (ArbSize << UDMA_CHCTL_ARBSIZE_Pos);
}

/**
 * @brief Setup destination address and item size and increment
 * 
 * @param Channel 
 * @param DstEndAddress 
 * @param DstItemSize 
 * @param DstInc 
 */
void 
Udma_ConfigDst(UdmaChannel_t Channel, 
               uint32_t DstEndAddress,
               UdmaItemSize_t DstItemSize, 
               UdmaInc_t DstInc)
{
  gChCtrlTable[Channel].DSTENDP = DstEndAddress;
  gChCtrlTable[Channel].CHCTL &= ~(UDMA_CHCTL_DSTSIZE_Msk);
  gChCtrlTable[Channel].CHCTL |= (DstItemSize << UDMA_CHCTL_DSTSIZE_Pos);
  gChCtrlTable[Channel].CHCTL &= ~(UDMA_CHCTL_DSTINC_Msk);
  gChCtrlTable[Channel].CHCTL |= (DstInc << UDMA_CHCTL_DSTINC_Pos);
}

/**
 * @brief Setup source address and item size and increment
 * 
 * @param Channel 
 * @param SrcEndAddress 
 * @param SrcItemSize 
 * @param SrcInc 
 */
void 
Udma_ConfigSrc(UdmaChannel_t Channel, 
               uint32_t SrcEndAddress,
               UdmaItemSize_t SrcItemSize, 
               UdmaInc_t SrcInc)
{
  gChCtrlTable[Channel].SRCENDP = SrcEndAddress;
  gChCtrlTable[Channel].CHCTL &= ~(UDMA_CHCTL_SRCSIZE_Msk);
  gChCtrlTable[Channel].CHCTL |= (SrcItemSize << UDMA_CHCTL_SRCSIZE_Pos);
  gChCtrlTable[Channel].CHCTL &= ~(UDMA_CHCTL_SRCINC_Msk);
  gChCtrlTable[Channel].CHCTL |= (SrcInc << UDMA_CHCTL_SRCINC_Pos);
}

inline static void 
Udma_EnableChannel(UdmaChannel_t Channel)
{
  UDMA->REQMASKCLR = 1 << Channel;
}

void 
Udma_StartTransfer(UdmaChannel_t Channel, uint32_t DataSize)
{
  uint8_t UdmaConfigSize = gUdmaInitConfig->UdmaConfigSize;
  const UdmaConfig_t* Channels = gUdmaInitConfig->UdmaConfig;
  uint8_t i;

  for(i = 0; i < UdmaConfigSize; i++)
    {
      if(Channels[i].Channel == Channel)
        {
          Udma_SetMode(Channel, Channels[i].Mode);
          Udma_SetXferSize(Channel, DataSize-1);
          break;
        }
    }

  UDMA->ENASET = 1 << Channel; //start the transfer
}

/**
 * @brief Set how many items will be transfered
 * 
 * @param Channel 
 * @param DataSize 
 */
inline static void 
Udma_SetXferSize(UdmaChannel_t Channel, uint32_t DataSize)
{
  gChCtrlTable[Channel].CHCTL &= ~UDMA_CHCTL_XFERSIZE_Msk;
  gChCtrlTable[Channel].CHCTL |= DataSize << UDMA_CHCTL_XFERSIZE_Pos;
}
