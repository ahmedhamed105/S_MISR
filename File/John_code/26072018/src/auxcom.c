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
//    Defines
//-----------------------------------------------------------------------------
#define USB_DBG     1

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static int gAuxId=-1;         //-1: USB not enabled; 0: USB enabled; 1: USB port opened

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
#if (!USB_DBG)
  BOOLEAN ret_val = TRUE;
  struct MW_AUX_CFG aux_cfg;
  char dev_name[24];

  aux_cfg.b_len     = 0x06;
  aux_cfg.b_mode    = 5;            // no parity
  aux_cfg.b_speed   = 7;            // 115200bps
  aux_cfg.b_rx_gap  = 0x32;
  aux_cfg.b_rsp_gap = 0x32;
  aux_cfg.b_tx_gap  = 0x32;
  aux_cfg.b_retry   = 0x03;

  strcpy(dev_name, DEV_AUX1);
  if (((gAuxId = OpenMW(dev_name, MW_RDWR)) < 0)
      || (IOCtlMW(gAuxId, IO_AUX_CFG, &aux_cfg) < 0)) {
    ret_val = FALSE;
  }
  return ret_val;
#else

  if ((gAuxId==0) && os_usbser_open())
    gAuxId = 1;
  
  if (gAuxId == 1)
    return TRUE;
  
  return FALSE;
#endif
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
#if (!USB_DBG)
  BOOLEAN ret_val = TRUE;
  if (CloseMW(gAuxId) < 0)
    ret_val = FALSE;
  return ret_val;
#else

  if (gAuxId > 0)
    os_usbser_close();
  gAuxId = 0;
  return TRUE;
#endif
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
#if (!USB_DBG)
  int len;

  len = ReadMW(gAuxId, aBuf, aLen);
  if (len < 0)
    len = 0;

  return len;
#else

  int len;

  len = os_usbser_read(aBuf, aLen);
  if (len < 0)
    len = 0;

  return len;
#endif
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
#if (!USB_DBG)
  BOOLEAN ret_val = TRUE;

  if (WriteMW(gAuxId, aBuf, aLen) != aLen)
    ret_val = FALSE;
  return ret_val;
#else
  DWORD i=500;
  BYTE buf[16];
  while (AuxTxRdy() == FALSE) {
    //sprintf(buf, "USB-Sts=%04X\n", os_usbser_status());
    //DispLineMW(buf, MW_LINE5, MW_SPFONT|MW_CENTER);
    if (--i == 0)
      return FALSE;
  }
  return (BOOLEAN)os_usbser_write(aBuf, aLen);
#endif
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
#if (!USB_DBG)
  if (IOCtlMW(gAuxId, IO_AUX_RX_RDY, NULL) <= 0)
    return FALSE;
  else
    return TRUE;
#else

  return (BOOLEAN)(os_usbser_status() & K_USB_RXRDY);
#endif
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
#if (!USB_DBG)
  if (IOCtlMW(gAuxId, IO_AUX_TX_RDY, NULL) <= 0)
    return FALSE;
  else
    return TRUE;
#else

  return (BOOLEAN)(os_usbser_status() & K_USB_TXRDY);
#endif
}
//*****************************************************************************
//  Function        : AuxEnable
//  Description     : Enable debug on Aux port.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void AuxEnable(void)
{
  os_usbser_enable(TRUE);
  gAuxId = 0;
}
//*****************************************************************************
//  Function        : AuxDisable
//  Description     : Disable debug on Aux port.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void AuxDisable(void)
{
  AuxClose();
  os_usbser_enable(FALSE);
  gAuxId = -1;
}
/******************************************************************************
 *  Function        : dbgHex
 *  Description     : Debug HEX data.
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void dbgHex(BYTE *aTitle, BYTE *aData, DWORD aLen)
{
  BYTE buf[30];
  WORD i;
  
  if ((gAuxId == -1) || (aLen == 0))
    return;
  
  if (AuxOpen() == FALSE)
    return;
  
  // Print out TLV
  SprintfMW(buf, "%s(%d):\r\n", aTitle, aLen);
  AuxWrite(buf, strlen(buf));
  for (i=0; i<aLen; i++) {
    SprintfMW(buf, "%02X ", aData[i]);
    AuxWrite(buf, 3);
  }
  AuxWrite("\r\n", 2);
  AuxWrite("-- -- -- --\r\n", 13);
}
