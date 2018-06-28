//-----------------------------------------------------------------------------
//  File          : auxcom.c
//  Module        :
//  Description   : Aux Port driver routines.
//  Author        : John
//  Notes         : N/A
//
//  Naming conventions
//  ~~~~~~~~~~~~~~~~~~
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  21 Feb  2011  John        Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include "midware.h"
#include "sysutil.h"
#include "auxcom.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static int gAuxId=-1;

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------


//*****************************************************************************
//  Function        : AuxOpen
//  Description     : Open the Aux Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AuxOpen(void)
{
  BOOLEAN ret_val = TRUE;
  struct MW_AUX_CFG aux_cfg;
  char dev_name[24];

  aux_cfg.b_len     = 0x06;
  aux_cfg.b_mode    = 0x05;       // ASYNC no parity
  aux_cfg.b_speed   = 0x07;       // 115200bps
  aux_cfg.b_rx_gap  = 0x02;
  aux_cfg.b_rsp_gap = 0x10;
  aux_cfg.b_tx_gap  = 0x02;
  aux_cfg.b_retry   = 0x01;

  strcpy(dev_name, DEV_AUX1);
  if (((gAuxId = OpenMW(dev_name, MW_RDWR)) < 0)
      || (IOCtlMW(gAuxId, IO_AUX_CFG, &aux_cfg) < 0)) {
    ret_val = FALSE;
  }
  return ret_val;
}
//*****************************************************************************
//  Function        : AuxClose
//  Description     : Close the opened Aux Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AuxClose(void)
{
  BOOLEAN ret_val = TRUE;
  while (IOCtlMW(gAuxId, IO_AUX_TX_RDY, NULL) <= 0) {};
  if (CloseMW(gAuxId) < 0)
    ret_val = FALSE;
  gAuxId = -1;
  return ret_val;
}
//*****************************************************************************
//  Function        : AuxRead
//  Description     : Read data from the opened Aux Port.
//  Input           : aBuf;             // pointer to read buffer
//                    aLen;             // len of read buffer
//  Return          : len of data read.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD AuxRead(BYTE *aBuf, DWORD aLen)
{
  int len;

  if (gAuxId < 0)
    return 0;
    
  len = ReadMW(gAuxId, aBuf, aLen);
  if (len < 0)
    len = 0;

  return len;
}
//*****************************************************************************
//  Function        : AuxWrite
//  Description     : Write data from the opened Aux Port.
//  Input           : aBuf;             // pointer to read buffer
//                    aLen;             // len of read buffer
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AuxWrite(BYTE *aBuf, DWORD aLen)
{
  BOOLEAN ret_val = TRUE;

  if (gAuxId < 0)
    return 0;
    
  while (IOCtlMW(gAuxId, IO_AUX_TX_RDY, NULL) <= 0) {};
  if (WriteMW(gAuxId, aBuf, aLen) != aLen)
    ret_val = FALSE;
  return ret_val;
}
//*****************************************************************************
//  Function        : AuxRxRdy
//  Description     : Check for Aux port receive ready.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AuxRxRdy(void)
{
  if (gAuxId < 0)
    return FALSE;
    
  if (IOCtlMW(gAuxId, IO_AUX_RX_RDY, NULL) <= 0)
    return FALSE;
  else
    return TRUE;
}
//*****************************************************************************
//  Function        : AuxTxRdy
//  Description     : Check for Aux port transmit ready.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AuxTxRdy(void)
{
  if (gAuxId < 0)
    return FALSE;
    
  if (IOCtlMW(gAuxId, IO_AUX_TX_RDY, NULL) <= 0)
    return FALSE;
  else
    return TRUE;
}
//*****************************************************************************
//  Function        : dbgAux
//  Description     : Write Debug Info to AUX port.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void dbgAux(BYTE *aTitle, BYTE *aData, DWORD aLen, BOOLEAN aHex)
{
  BYTE buf[64];
  DWORD i;
  
  AuxOpen();
  sprintf(buf, "%s(%d): ", aTitle, aLen);
  AuxWrite(buf, strlen(buf));
  if (aData != NULL) {
    if (aHex == TRUE) {
      for (i=0; i<aLen; i++) {
        sprintf(buf, "%02X ", aData[i]);
        AuxWrite(buf, 3);
      }
    }
    else {
      i = (aLen>64)? 64: aLen;
      memcpy(buf, aData, i);
      AuxWrite(buf, i);
    }
  }
  buf[0] = '\n';
  AuxWrite(buf, 1);
  AuxClose();
}
