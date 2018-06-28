//-----------------------------------------------------------------------------
//  File          : termcfg.c
//  Module        :
//  Description   : Terminal Data management.
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |        Struct define : Leading T                                         |
// |         Struct/Array : Leading s                                         |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g  (variable use outside one C file)      |
// |                                                                          |
// |   BYTE( 8 Bits) size : Leading b                                         |
// |   CHAR( 8 Bits) size : Leading c                                         |
// |   WORD(16 Bits) size : Leading w                                         |
// |  DWORD(32 Bits) size : Leading d                                         |
// |    int(32 Bits) size : Leading i                                         |
// | DDWORD(64 Bits) size : Leading dd                                        |
// |              Pointer : Leading p                                         |
// |                                                                          |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |                                                                          |
// | Examples:                                                                |
// |                Array : Leading s, (sb = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  29 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include "midware.h"
#include "util.h"
#include "sysutil.h"
#include "hardware.h"
#include "kbdutil.h"
#include "menu.h"
#include "message.h"
#include "toggle.h"
#include "stis.h"
#include "termdata.h"
#include "comm.h"
#include "emvtrans.h"
#include "lptutil.h"
#include "ecrdll.h"
#include "key2dll.h"
#include "emv2dll.h"
#include "emvcl2dll.h"
#include "ctlcfg.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Globals
//-----------------------------------------------------------------------------
static BYTE bAccessDLL;
static struct TERM_DATA *psTermDataOffset = 0;
static int    iTermDataFd=-1;

//-----------------------------------------------------------------------------
//    Constants
//-----------------------------------------------------------------------------
static char KTermDataFile[] = { "TermData" };
// Default Terminal Data
static const struct TERM_DATA KDefaultTermData = {
  SETUP_MODE,                                                               // STIS mode
  12,                                                                       // LCD contrast
  {'4', '0', '0', '0', '0', '0', '0', '1', '4'},                            // Term ID
  APM_COMM_TCPIP,                                                            // Init Mode(COMM_SYNC = 0, COMM_TCPIP, COMM_ASYNC)
  {0x22, 0x64, 0x39, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Primary Phone
  0x30, 0x02,                                                               // Pri Conn time, redial
  {0x22, 0x64, 0x39, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Secondary Phone
  0x30, 0x02,                                                               // Sec Conn time, redial
  0x01,                                                                     // Async Conn Delay
  DIAL_TONE,                                                                // Dial Mode (TONE=0, PULSE)
  {0xFF, 0xFF, 0xFF, 0xFF},                                                 // PABX
  {0x00, 0x00, 0x01}, {0x00, 0x00, 0x01},                                   // Trace & ROC Num.
  {0xD2, 0x03, 0x1D, 0x57}, {0x03, 0xDE},                                   // InitIP & Port (210.3.29.87:990)
  {0x00, 0x00},                                                             // Init NII
  0x00,                                                                     // EMV Enable
  0x10,                                                                     // Default Manager
  0x05,                                                                     // Return to Base Timeout
  0x0000,                                                                   // Training Status
  MAGIC ,                                                                   // Debug Status
  0x00,                                                                     // Default Amount
  0x00,                                                                     // Default Ext Pinpad Port
  0x00,                                                                     // Default ECR Port
  //29-09-16 JC ++
  0x00,                                                                     // ECR Port SSL flag (0-NoSSL, 1-ServerAuth, 2-ClientAuth)
  {0x00, 0x00},                                                             // EDC Param File Checksum
  {0x00, 0x00, 0x00, 0x00},                                                 // EDC Param File Date (CCYYMMDD)
  0,                                                                        // EDC Param File Size
  {0x00, 0x00},                                                             // EMVApp Param File Checksum
  {0x00, 0x00, 0x00, 0x00},                                                 // EMVApp Param File Date (CCYYMMDD)
  0,                                                                        // EMVApp Param File Size
  {0x00, 0x00},                                                             // EMVKey Param File Checksum
  {0x00, 0x00, 0x00, 0x00},                                                 // EMVKey Param File Date (CCYYMMDD)
  0,                                                                        // EMVKey Param File Size
  {0x00, 0x00},                                                             // ExApp Param File Checksum
  {0x00, 0x00, 0x00, 0x00},                                                 // ExApp Param File Date (CCYYMMDD)
  0,                                                                        // ExApp Param File Size
  {0x00, 0x00},                                                             // LnExApp Param File Checksum
  {0x00, 0x00, 0x00, 0x00},                                                 // LnExApp Param File Date (CCYYMMDD)
  0,                                                                        // LnExApp Param File Size
  {0x00},                                                                   // FTP Username (32B)
  {0x00},                                                                   // FTP Password (32B)
  {0x00},                                                                   // PPP Username (32B)
  {0x00},                                                                   // PPP Password (32B)
  {0xD2, 0x03, 0x1D, 0x57}, {0x03, 0xDE},                                   // PPP IP & Port (210.3.29.87:990)
  {0x00},                                                                   // Server Key (25B)
  {0x22, 0x64, 0x39, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Checkin Primary Phone
  0x30, 0x02,                                                               // Checkin Pri Conn time, redial
  {0x22, 0x64, 0x39, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Checkin Secondary Phone
  0x30, 0x02,                                                               // Checkin Sec Conn time, redial
  0x00,                                                                     // Init Fallback
  0x0000,                                                                   // App List Checksum
  0x00,                                                                     // App List Upload flag
  //29-09-16 JC --
};
//*****************************************************************************
//  Function        : UpdTermData
//  Description     : Update Terminal Data File
//  Input           : aDat;         // pointer data buffer
//                    aCalCRC;      // 1 => ReCal CRC before update.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdTermData(struct TERM_DATA *aDat)
{
  if (iTermDataFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(iTermDataFd, (DWORD) psTermDataOffset, aDat, sizeof(struct TERM_DATA));
  return TRUE;
}
//*****************************************************************************
//  Function        : GetTermData
//  Description     : Get Terminal Data.
//  Input           : aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetTermData(struct TERM_DATA *aDat)
{
  CheckPointerAddr(aDat);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, aDat, sizeof(struct TERM_DATA));
  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memcpy(aDat, &KDefaultTermData, sizeof(struct TERM_DATA));
  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);

  return FALSE;
}
//*****************************************************************************
//  Function        : CloseTermData
//  Description     : Close Term Data file
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CloseTermData(void)
{
  if (iTermDataFd >= 0) {
    fCloseMW(iTermDataFd);
    iTermDataFd = -1;
  }
}
//*****************************************************************************
//  Function        : CreateTermData
//  Description     : Create Term Data file
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CreateTermData(void)
{
  BYTE filename[32];
  struct TERM_DATA term_data;
#if (MCP_SUPPORT)
  T_APP_INFO app_info;
#endif
  DWORD len;

  strcpy(filename, KTermDataFile);
  fDeleteMW(filename);
  iTermDataFd = fCreateMW(filename, 0);
  memcpy(&term_data, &KDefaultTermData, sizeof(struct TERM_DATA));
#if (MCP_SUPPORT)
  // check if both Bluetooth & ECRDLL exist
  if ((os_hd_config() & K_HdBt) && os_app_info(ECRDLL_ID, &app_info)) {
    term_data.b_ecr_port = (TOG_BT|ECR_RAW_MODE);     // turn on Bluetooth by default
  }
#endif
  term_data.w_crc = (WORD) cal_crc((BYTE *)&term_data, (BYTE *) &term_data.w_crc - (BYTE *) &term_data);
  len = fWriteMW(iTermDataFd, &term_data, sizeof(struct TERM_DATA));
  if (len != sizeof(struct TERM_DATA)) {
    DispLineMW("File WR Err!", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    ErrorDelay();
  }
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : TermDataInit
//  Description     : Reset Terminal Data to default value.
//  Input           : void
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void TermDataInit(void)
{
  char filename[32];

  strcpy(filename, KTermDataFile);
  // Init Term Data File
  iTermDataFd = fOpenMW(filename);
  if (iTermDataFd < 0) {
    DispLineMW("Create: TermData", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
    CreateTermData();
  }
}
//*****************************************************************************
//  Function        : GetSTISMode
//  Description     : Return current STIS data mode.
//  Input           : N/A
//  Return          : STIS mode;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE GetSTISMode(void)
{
  BYTE mode;
  ReadSFile(iTermDataFd, (DWORD) &psTermDataOffset->b_stis_mode, &mode, sizeof(mode));
  return mode;
}
//*****************************************************************************
//  Function        : SetSTISMode
//  Description     : Change STIS data mode.
//  Input           : STIS mode;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetSTISMode(BYTE aMode)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  term_data.b_stis_mode = aMode;
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : SetDebugMode
//  Description     : Change STIS debug mode.
//  Input           : STIS mode;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetDebugMode(BOOLEAN aOn)
{
  struct TERM_DATA term_data;
  WORD mode=0;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  if (aOn)
    mode = MAGIC;
  term_data.w_debug_status = mode;
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : DebugModeON
//  Description     : Return TRUE if training mode is set on.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DebugModeON(void)
{
  WORD status;

  ReadSFile(iTermDataFd, (DWORD) &psTermDataOffset->w_debug_status, &status, sizeof(status));

  return(status == MAGIC);
}
//*****************************************************************************
//  Function        : TrainingModeON
//  Description     : Return TRUE if training mode is set on.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN TrainingModeON(void)
{
  WORD status;

  ReadSFile(iTermDataFd, (DWORD) &psTermDataOffset->w_training_status, &status, sizeof(status));

  return(status == MAGIC);
}
//*****************************************************************************
//  Function        : PrintRcptHeader
//  Description     : Print Receipt Header
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PrintRcptHeader(void)
{
  struct TERM_CFG term_cfg;
  char prndata[256], *ptr;
  char line_feed[] = {"\n\x00"};
  int  space_len;

  if (!GetTermCfg(&term_cfg))
    return;

  space_len = (PRINT_WIDTH - 23) / 2;
  ptr = prndata;
  memcpy(ptr, "\x1B\x40", 2);   // Reset PRINTER
  ptr += 2;

  memset(ptr, ' ', space_len);
  ptr += space_len;
  memcpy(ptr, term_cfg.sb_dflt_name, 23);
  ptr += 23;
  memcpy(ptr, line_feed, 1);
  ptr += 1;

  memset(ptr, ' ', space_len);
  ptr += space_len;
  memcpy(ptr, term_cfg.sb_name_loc, 23);
  ptr += 23;
  memcpy(ptr, line_feed, 1);
  ptr += 1;

  memset(ptr, ' ', space_len);
  ptr += space_len;
  memcpy(ptr, &term_cfg.sb_name_loc[23], 23);
  ptr += 23;

  memcpy(ptr, line_feed, 1);
  ptr += 1;
  memcpy(ptr, line_feed, 1);
  ptr += 1;
  PrintBuf(prndata, ptr-prndata, TRUE);
}
//******************************************************************************
//  Function        : PrintSTIS
//  Description     : Print or display EDC and EMV parameter.
//  Input           : aDetail
//                    aToDisplay
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
void PrintSTIS(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  T_APP_INFO appl_info;

  if (!aToDisplay) {
    DispLineMW(KPrintStis, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
    Disp2x16Msg(KPrintingMsg, MW_LINE5, MW_BIGFONT);
    LptOpen(NULL);
    PrintRcptHeader();
  }

  PrintSTISParam(TRUE, aToDisplay);
  if (AppInfoGetMW(EMVDLL_ID, &appl_info)) {    // Emv kernel exist
    PrintEMVParam(TRUE, aToDisplay);
  }
  if (AppInfoGetMW(EMVCLDLL_ID, &appl_info)) {  // Emv kernel exist
    PrintCTLParam(TRUE, aToDisplay);
  }

  if (!aToDisplay) {
    PrintRcptFF();
    LptClose();
  }
}
//*****************************************************************************
//  Function        : UpdateTermID
//  Description     : Update terminal id value.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN UpdateTermID(void)
{
  struct TERM_DATA term_data;

  BYTE kbdbuf[MAX_INPUT_LEN+1];

  DispClrBelowMW(MW_LINE3);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset->sb_term_id, &kbdbuf[1], 9);
  kbdbuf[0] = 9;
  while (1) {
    DispLineMW(KTermId, MW_LINE5, MW_BIGFONT);

    if (!GetKbd(NUMERIC_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(9)+IMAX(9), kbdbuf))
      return FALSE;

    if (chk_digit_ok(&kbdbuf[1], 9)) {
      ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
      memcpy(term_data.sb_term_id, &kbdbuf[1], 9);
      UpdTermData(&term_data);
      return TRUE;
    }

    DispLineMW(KChkSumErr, MW_LINE7, MW_CENTER|MW_BIGFONT);
    ErrorDelay();
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : CancelExit
//  Description     : Prompt user press CANCEL key to exit.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN CancelExit(void)
{
  DispClrBelowMW(MW_LINE3);
  DispLineMW(KCancelExit, MW_LINE5, MW_CENTER|MW_BIGFONT);
  ClearKeyin();
  while (1) {
    switch (WaitKeyCheck(KBD_TIMEOUT, 0)) {
      case MWKEY_CANCL:
        return(TRUE);
      case MWKEY_ENTER:
      case MWKEY_SELECT:
        return(FALSE);
    }
    SleepMW();
  }
}
//*****************************************************************************
//  Function        : SetInitIP
//  Description     : Save Init ip & port.
//  Input           : N/A
//  Return          : TRUE;     // VALID INPUT
//                    FALSE;    // Cancel
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SetInitIP(void)
{
  struct TERM_DATA term_data;
  DWORD port;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  DispClrBelowMW(MW_LINE3);
  port = term_data.sb_port[0]*256 + term_data.sb_port[1];

  DispLineMW(KInitIP, MW_LINE3, MW_SMFONT);
  if (!EditIp(term_data.sb_ip, MW_LINE4))
    return FALSE;

  DispLineMW(KInitPort, MW_LINE5, MW_SMFONT);
  if (!EditPort(&port, MW_LINE6))
    return FALSE;

  DispLineMW(KInitEnterSave, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_CENTER|MW_SPFONT);
  if (WaitKey(KBD_TIMEOUT) != MWKEY_ENTER)
    return FALSE;

  term_data.sb_port[0]=(BYTE)(port/256);
  term_data.sb_port[1]=(BYTE)(port&0xFF);
  UpdTermData(&term_data);
  return TRUE;
}
//*****************************************************************************
//  Function        : SetupSTIS
//  Description     : Setup STIS init parameters.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetupSTIS(void)
{
  struct TERM_DATA term_data;
  char option;
  BYTE tmp[12];
  BYTE init_mode;
  BYTE kbdbuf[MAX_INPUT_LEN+1];

  DispLineMW("STIS Setup", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  while (1) {
    if (!UpdateTermID())
      return;

    // Init Mode
    ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
    option = term_data.b_init_mode;
    DispClrBelowMW(MW_LINE3);
    DispLineMW(KInitModeStr, MW_LINE5, MW_BIGFONT);
#if (MCP_SUPPORT)
    if (os_hd_config() & K_HdBt)
      option = ToggleOption(NULL, KInitModeBt, option);
#else
    option = ToggleOption(NULL, KInitMode, option);
#endif
    if (option == -1)
      return;
    if (option == APM_COMM_WIFI) {
      if ((os_hd_config_extend() & K_XHdWifi) == 0) {
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KNonWifiModel, MW_LINE5, MW_CENTER|MW_BIGFONT);
        LongBeep();
        WaitKey(KBD_TIMEOUT);
        return;
      }
    }
#if (MCP_SUPPORT)
    else if (option == APM_COMM_BT) {
      if ((os_hd_config() & K_HdBt) == 0) {
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KNonBtModel, MW_LINE5, MW_CENTER|MW_BIGFONT);
        LongBeep();
        WaitKey(KBD_TIMEOUT);
        return;
      }
    }
#endif
    term_data.b_init_mode = option;
    UpdTermData(&term_data);

    init_mode = option;
    // setup for dial mode
    switch (init_mode) {
      case APM_COMM_SYNC:
      case APM_COMM_ASYNC:
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KInitPriNum, MW_LINE5, MW_BIGFONT);
        memcpy(tmp, term_data.sb_pri_no, 12);
        if (!UpdateFData(tmp, 12))
          return;
        memcpy(term_data.sb_pri_no, tmp, 12);
        UpdTermData(&term_data);

        DispClrBelowMW(MW_LINE3);
        DispLineMW(KInitSecNum, MW_LINE5, MW_BIGFONT);
        memcpy(tmp, term_data.sb_sec_no, 12);
        if (!UpdateFData(tmp, 12))
          return;
        memcpy(term_data.sb_sec_no, tmp, 12);
        UpdTermData(&term_data);

        option = term_data.b_dial_mode;
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KDialModeStr, MW_LINE5, MW_BIGFONT);
        option = ToggleOption(NULL, KDialMode, option);
        if (option==-1)
          return;
        term_data.b_dial_mode = option;
        UpdTermData(&term_data);

        if (init_mode == APM_COMM_ASYNC) {
          tmp[0] = term_data.b_async_delay;
          DispClrBelowMW(MW_LINE3);
          DispLineMW(KTimeDelay, MW_LINE5, MW_BIGFONT);
          split(&kbdbuf[1], tmp, 1);
          kbdbuf[0] = 2;
          if (!GetKbd(NUMERIC_INPUT+ECHO+JUSTIFIED+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(2)+IMAX(2), kbdbuf))
            return;
          compress(tmp, &kbdbuf[1], 1);
          term_data.b_async_delay = tmp[0];
          UpdTermData(&term_data);
        }

        DispClrBelowMW(MW_LINE3);
        if (!UpdatePABX())
          return;

        // pri_no inited
        if (skpb(term_data.sb_pri_no, 0xFF, 12) != 12) {
          if (GetSTISMode() == SETUP_MODE)
            SetSTISMode(INIT_MODE);
        }
        break;
      case APM_COMM_BT:
      case APM_COMM_WIFI:
      case APM_COMM_GPRS:
      case APM_COMM_TCPIP:
        if (!SetInitIP())
          return;
        if (GetSTISMode() == SETUP_MODE)
          SetSTISMode(INIT_MODE);
        break;
      case APM_COMM_AUX:
        if (GetSTISMode() == SETUP_MODE)
          SetSTISMode(INIT_MODE);
        break;
    }

    AcceptBeep();
    if (CancelExit()) {
      fCommitAllMW();
      break;
    }
  }
}
//*****************************************************************************
//  Function        : CompressInputData
//  Description     : Fill the data with leading '0' & compress it.
//  Input           : aDest;    // pointer to destinate buffer;
//                    aSrc;     // pointer to src buffer.
//                              // 1st byte is len of input
//                    aLen;     // number of byte to compress.
//  Return          : TRUE/FALSE;
//  Note            : Max len for input is 12.
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CompressInputData(void *aDest, BYTE *aSrc, BYTE aLen)
{
  BYTE tmp[12];

  if ((aLen > 6) || (aSrc[0] > 12))
    return FALSE;

  memset(tmp, '0', 12); /* 12 is the max len */
  memcpy(&tmp[aLen*2-aSrc[0]], &aSrc[1], aSrc[0]);
  compress(aDest, tmp, aLen);
  return TRUE;
}
//*****************************************************************************
//  Function        : CompressInputFData
//  Description     : Fill the data with trailing 'F' & compress it.
//  Input           : aDest;    // pointer to destinate buffer;
//                    aSrc;     // pointer to src buffer.
//                              // 1st byte is len of input
//                    aLen;     // number of byte to compress.
//  Return          : TRUE/FALSE;
//  Note            : Max len for input is 12.
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CompressInputFData(void *aDest, BYTE *aSrc,  BYTE aLen)
{
  BYTE tmp[25];

  if ((aLen > 12) || (aSrc[0] > 24))
    return FALSE;

  memset(tmp, 'F', sizeof(tmp));
  memcpy(tmp, &aSrc[1], aSrc[0]);
  compress(aDest, tmp, aLen);

  return TRUE;
}
//*****************************************************************************
//  Function        : SetModeOnOff
//  Description     : Toggle term data operation mode.
//  Input           : aWhich;           // TOGGLE_DEBUG/TOGGLE_TRAINING
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : parea;
//*****************************************************************************
void SetModeOnOff(DWORD aWhich)
{
  struct TERM_DATA term_data;
  char option;
  WORD status;
  const char *pMsg;

  ReadSFile(iTermDataFd, (DWORD)psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  switch (aWhich) {
    case TOGGLE_DEBUG:
      status = term_data.w_debug_status;
      pMsg = KDebugMode;
      break;
    default:
    case TOGGLE_TRAINING:
      status = term_data.w_training_status;
      pMsg = KTrainingMode;
      break;
  }
  option = (status == MAGIC) ? 1: 0;
  DispLineMW(pMsg, MW_LINE5, MW_CLRDISP|MW_BIGFONT); 
  option = ToggleOption(pMsg, KEnable, option);
  if (option != -1) {
    status = (option == 1)? MAGIC : 0;
    switch (aWhich) {
      case TOGGLE_DEBUG:
        term_data.w_debug_status = status;
        UpdTermData(&term_data);
        break;
      default:
      case TOGGLE_TRAINING:
        term_data.w_training_status = status;
        UpdTermData(&term_data);
        break;
    }
  }
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : UpdateFData
//  Description     : Prompt user to input data & update with 'F' packed
//                    if input len is less than max len.
//  Input           : aDest;     // pointer to update buffer
//                    aLen;       // max len of data input.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdateFData(BYTE *aDest, BYTE aLen)
{
  BYTE kbdbuf[MAX_INPUT_LEN+1];

  memset(&kbdbuf[1], 'F', 25); /* tel no. max len = 24 */

  split(&kbdbuf[1], aDest, aLen);
  kbdbuf[0] = (BYTE)fndb(&kbdbuf[1],'F',24);

  if (!GetKbd(HEX_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(aLen*2), kbdbuf))
    return(FALSE);

  CompressInputFData(aDest, kbdbuf, aLen);
  return(TRUE);
}
//*****************************************************************************
//  Function        : UpdatePABX
//  Description     : Update PABX value.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdatePABX(void)
{
  struct TERM_DATA term_data;

  DispLineMW(KPABX, MW_LINE5, MW_BIGFONT);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  if (UpdateFData(term_data.sb_pabx, 4)) {
    UpdTermData(&term_data);
    fCommitAllMW();
    return(TRUE);
  }
  return(FALSE);
}
//*****************************************************************************
//  Function        : SetBatchNumber
//  Description     : Set batch number.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SetBatchNumber(void)
{
  int acq_id;
  struct ACQUIRER_TBL acq_tbl;
  BYTE tmp[6];
  BYTE kbdbuf[MAX_INPUT_LEN+1];

  acq_id = SelectAcquirer(0);
  if (acq_id == -1)
    return TRUE;

  GetAcqTbl(acq_id, &acq_tbl);
  DispPutStr(KCurrBatchNo);
  DispLineMW(KCurrBatchNo, MW_LINE5, MW_CLRDISP|MW_BIGFONT);
  memcpy(tmp, acq_tbl.sb_curr_batch_no, 3);
  split(&kbdbuf[1], tmp, 3);
  kbdbuf[0] = 6;
  if (!GetKbd(NUMERIC_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(6)+IMAX(6), kbdbuf))
    return FALSE;
  CompressInputData(tmp, kbdbuf, 3);
  memcpy(&acq_tbl.sb_curr_batch_no, tmp, 3);
  bcdinc(tmp, 3);
  memcpy(&acq_tbl.sb_next_batch_no, tmp, 3);
  UpdAcqTbl(acq_id, &acq_tbl);
  return TRUE;
}
//*****************************************************************************
//  Function        : SetHostInfo
//  Description     : Set host info.
//  Input           : N/A
//  Return          : N/A
//  Note            : //29-09-15 JC ++
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SetHostInfo(void)
{
  int acq_id;
  struct ACQUIRER_TBL acq_tbl;
  BYTE tmp[32];
  BYTE kbdbuf[MAX_INPUT_LEN+1];
  BYTE ip_addr[4];
  DWORD port;
  BYTE ssl_key_idx, extra_len;

  acq_id = SelectAcquirer(0);
  if (acq_id == -1)
    return TRUE;

  DispLineMW(KHostParamHdr, MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SMFONT);
  GetAcqTbl(acq_id, &acq_tbl);
  memcpy(ip_addr, acq_tbl.sb_ip, 4);
  port = acq_tbl.sb_port[0]*256 + acq_tbl.sb_port[1];
  ssl_key_idx = acq_tbl.b_ssl_key_idx;
  extra_len = acq_tbl.b_reserved1;

  // Host IP
  DispLineMW(KHostIP, MW_LINE2, MW_SPFONT);
  memcpy(tmp, ip_addr, 4);
  if (!EditIp(tmp, MW_LINE2+6))
    return FALSE;
  memcpy(ip_addr, tmp, 4);

  // Host Port
  DispLineMW(KHostPort, MW_LINE3, MW_SPFONT);
  if (!EditPort(&port, MW_LINE3+6))
    return FALSE;
  
  // SSL Key Index
  DispLineMW(KHostSslKeyIdx, MW_LINE6, MW_SPFONT);
  split(&kbdbuf[1], &ssl_key_idx, 1);
  kbdbuf[0] = 2;
  if (!GetKbd(HEX_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(2)+IMAX(2), kbdbuf))
    return FALSE;
  compress(&ssl_key_idx, &kbdbuf[1], 1);
  
  // 2-Byte Length
  DispLineMW(KExtraMsgLen, MW_LINE6, MW_SPFONT);
  split(&kbdbuf[1], &extra_len, 1);
  kbdbuf[0] = 2;
  if (!GetKbd(HEX_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(2)+IMAX(2), kbdbuf))
    return FALSE;
  compress(&extra_len, &kbdbuf[1], 1);
  
  memcpy(acq_tbl.sb_ip, ip_addr, 4);
  acq_tbl.sb_port[0] = (BYTE)(port/256);
  acq_tbl.sb_port[1] = (BYTE)(port&0xFF);
  acq_tbl.b_ssl_key_idx = ssl_key_idx;
  acq_tbl.b_reserved1 = extra_len;
  UpdAcqTbl(acq_id, &acq_tbl);
  return TRUE;
}
//*****************************************************************************
//  Function        : GetPortDevName
//  Description     : Get Port Device Name from port id
//  Input           : aPort;        // port id
//                    aFileName;    // pointer to filename buffer.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GetPortDevName(DWORD aPort, char *aFileName)
{
  switch (aPort)  {
     case TOG_AUXD:
       strcpy(aFileName, DEV_AUX0);
       break;
     case TOG_AUX1:
       #if (PR608D)
       strcpy(aFileName, DEV_ECR1);
       #elif (T300|T810|A5T1000|T1000|R700|TIRO|PR608)
       strcpy(aFileName, DEV_AUX1);
       #endif
       break;
     case TOG_AUX2:
       #if (PR608D)
       strcpy(aFileName, DEV_ECR2);
       #elif (R700|TIRO|PR608)
       strcpy(aFileName, DEV_AUX2);
       #elif (T300|T810|A5T1000|T1000)
       aFileName[0] = 0;
       #endif
       break;
     default :
       aFileName[0] = 0;
  }
}
//*****************************************************************************
//  Function        : SetupTerm
//  Description     : Perform misc setup.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetupTerm(void)
{
  static const DWORD KDecimalPos[4] = { DECIMAL_NONE, DECIMAL_POS1, DECIMAL_POS2, DECIMAL_POS3};
  BYTE kbdbuf[MAX_INPUT_LEN+1];
  BYTE tmp[20], filename[32];
  BYTE dec_pos;
  BYTE txn_amt_len;
  struct TERM_DATA term_data;
  struct TERM_CFG  term_cfg;
  int  option, extra;
  DWORD ret;

  GetTermData(&term_data);
  GetTermCfg(&term_cfg);

  while (1) {
    // setup terminal init connection time
    DispLineMW(KInitConnTime, MW_LINE5, MW_CLRDISP|MW_BIGFONT);
    split(&kbdbuf[1], &term_data.b_pri_conn_time, 1);
    kbdbuf[0] = 2;
    if (!GetKbd(NUMERIC_INPUT+ECHO+JUSTIFIED+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(2), kbdbuf))
      return;
    CompressInputData(&term_data.b_pri_conn_time, kbdbuf, 1);
    term_data.b_sec_conn_time = term_data.b_pri_conn_time;
    UpdTermData(&term_data);

    // init NII
    DispLineMW(KInitNII, MW_LINE5, MW_CLRDISP|MW_BIGFONT);
    split(&kbdbuf[1], term_data.sb_init_nii, 2);
    memmove(&kbdbuf[1], &kbdbuf[2], 3);
    kbdbuf[0] = 3;
    if (!GetKbd(NUMERIC_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(3)+IMAX(3), kbdbuf))
      return;
    kbdbuf[0] = '0';
    compress(term_data.sb_init_nii, kbdbuf, 2);
    UpdTermData(&term_data);

    // setup return to base timeout
    Disp2x16Msg(KReturnBaseTO, MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    bin2dec(term_data.b_return_timeout, &kbdbuf[1], 3);
    kbdbuf[0] = 3;
    if (!GetKbd(NUMERIC_INPUT+ECHO+JUSTIFIED+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(3)+IMAX(3), kbdbuf))
      return;
    term_data.b_return_timeout = (BYTE)dec2bin(&kbdbuf[1], 3);
    if (term_data.b_return_timeout < 5)
      term_data.b_return_timeout = 5;
    else
      if (term_data.b_return_timeout > 120)
        term_data.b_return_timeout = 120;
    UpdTermData(&term_data);

    // setup default amount
    DispLineMW(KDefaultAmt, MW_LINE5, MW_CLRDISP|MW_BIGFONT);

    memset(kbdbuf, 0, sizeof(kbdbuf));
    kbdbuf[1] = ' ';
    memcpy(&kbdbuf[2], term_cfg.sb_currency_name, 3);
    dbin2asc(tmp, term_data.dd_default_amount);

    tmp[0] = (BYTE) skpb(&tmp[1], '0', 20-3);
    memmove(&kbdbuf[5], &tmp[tmp[0]+1], 19-tmp[0]);
    kbdbuf[0] = (19-tmp[0])+PREFIX_SIZE;

    dec_pos = term_cfg.b_decimal_pos;
    txn_amt_len = term_cfg.b_trans_amount_len;
    if (!GetKbd(AMOUNT_INPUT+PREFIX_ENB+ECHO+MW_BIGFONT+MW_LINE7+KDecimalPos[dec_pos], txn_amt_len, kbdbuf))  // 2 decimal digit
      return;
    term_data.dd_default_amount = decbin8b(&kbdbuf[1], kbdbuf[0]);
    UpdTermData(&term_data);

    // batch number
    if (!SetBatchNumber())
      return;

    //16-09-15 JC ++
    // host ip & port
    if (!SetHostInfo())
      return;
    //16-09-15 JC --

    // setup External Pinpad Port
    DispLineMW(KPPadPort, MW_LINE5, MW_CLRDISP|MW_BIGFONT);
    option = term_data.b_ppad_port;
    option = ToggleOption(NULL, KAuxPort, option);
    if (option == -1)
      return;
    //26-03-15 JC ++
    if (option != 0) {  // Key Inject for external pinpad channel
      GetPortDevName(option, filename);
      ret = SChnlSetupMW(filename, TRUE);
      WaitKey(KBD_TIMEOUT);
      if (ret == 0)
        option = 0;    // reset pinpad flag if setup failed
    }
    if (option)
      KDLL_ExtPPadSetup(MIDWARE_ID, SCHNLGETPINMW);  // Use External pinpad to get PIN
    else
      KDLL_ExtPPadSetup(0, 0);
    term_data.b_ppad_port = option;
    UpdTermData(&term_data);
    //26-03-15 JC --

    // setup Ecr Port
    //08-09-15 JC ++
    DispLineMW(KEcrPort, MW_LINE5, MW_CLRDISP|MW_BIGFONT);
    option = term_data.b_ecr_port & 0x3F;
    #if (MCP_SUPPORT)
    if (os_hd_config() & K_HdBt)
      option = ToggleOption(NULL, KAuxUsbWifiBtPort, option);
    else 
    #endif
    if ((os_hd_config_extend() & K_XHdWifi))
      option = ToggleOption(NULL, KAuxUsbWifiPort, option);
    else
      option = ToggleOption(NULL, KAuxUsbPort, option);
    if (option == -1)
      return;
    if ((term_data.b_ecr_port&0x3F) != option) {
      if (option != 0) {
        DispClrBelowMW(MW_LINE5);
        extra = (term_data.b_ecr_port & ECR_RAW_MODE) >> 6;
        extra = ToggleOption("RAW Mode:", KEnable, extra);
        if (extra == -1)
          return;
        //08-11-16 JC ++
        if (extra) {
          term_data.b_ecr_port = (option|ECR_RAW_MODE);   // Msg format defined by app
          #if (MCP_SUPPORT)
          // ECR SSL mode
          if (os_hd_config() & K_HdBt) {
            DispLineMW(KEcrSslMode, MW_LINE5, MW_CLRDISP|MW_BIGFONT);
            kbdbuf[1] = '0' + term_data.b_ecr_ssl;
            kbdbuf[0] = 1;
            if (!GetKbd(NUMERIC_INPUT+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMIN(1)+IMAX(1), kbdbuf))
              return;
            term_data.b_ecr_ssl = kbdbuf[1] - '0';
          }
          #endif
        }
        else
          term_data.b_ecr_port = (option|ECR_USE_SEQ);    // Standard ECR interface
        //08-11-16 JC --
      }
      else
        term_data.b_ecr_port = 0;
      UpdTermData(&term_data);
      Disp2x16Msg(KReboot, MW_LINE3, MW_CLRDISP|MW_BIGFONT);
      if (WaitKey(KBD_TIMEOUT) == MWKEY_ENTER) {
        fCommitAllMW();
        ResetMW();
      }
    }
    //08-09-15 JC --

    // setup Default App
    DispLineMW(KDefaultApp, MW_LINE5, MW_CLRDISP|MW_BIGFONT);
    bin2dec(term_data.b_default_app, &kbdbuf[1], 2);
    kbdbuf[0] = 2;
    if (!GetKbd(NUMERIC_INPUT+ECHO+JUSTIFIED+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(2), kbdbuf))
      return;
    term_data.b_default_app = (BYTE)dec2bin(&kbdbuf[1], 2);
    UpdTermData(&term_data);

    if (CancelExit()) {
      fCommitAllMW();
      return;
    }
  }
}
//*****************************************************************************
//  Function        : GenDLL
//  Description     : Generate a new DLL value.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : stis;
//*****************************************************************************
void GenDLL(void)
{
  BYTE time[8];

  RandMW(time);
  #if (DEBUG_MODE|WIN32)
  time[0] = 0x00;
  #endif
  if (DebugModeON()) {
    time[0] = 0x00;
  }
  if ((time[0] & 0xf0) > 0x90) time[0] -= 0x90;
  if ((time[0] & 0x0f) > 0x09) time[0] -= 0x09;
  bAccessDLL = time[0];
}
//*****************************************************************************
//  Function        : GetAccessCode
//  Description     : Prompt user to enter access code.
//  Input           : aFixCode;   // TRUE : fix code expected.
//                                // FALSE: code gen from DLL expected.
//  Return          : TRUE;    // valid access code entered
//                    FALSE;   // user cancel or invalid access code.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetAccessCode(void)
{
  DWORD retry;
  BYTE buffer[8];
  BYTE kbdbuf[MW_MAX_LINESIZE+1];

  retry = (BYTE)bcd2bin(bAccessDLL);
  bin2dec((DWORD)(retry * (retry - 1) * (retry - 2)), buffer, 6);

  sprintf(kbdbuf, "(DLL:%02X)", bAccessDLL);
  DispLineMW("ACCESS CODE", MW_LINE3, MW_CENTER|MW_BIGFONT);
  DispLineMW(kbdbuf, MW_LINE5, MW_CENTER|MW_BIGFONT);

  retry = 3;
  while (retry--) {
    if (!GetKbd(HIDE_NUMERIC+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(6), kbdbuf))
      return FALSE;

    if (memcmp(buffer, &kbdbuf[1], 6) == 0)
      return TRUE;

    LongBeep();
  }
  GenDLL();
  ErrorDelay();
  return FALSE;
}
//*****************************************************************************
//  Function        : SetROC
//  Description     : Set the roc no.
//  Input           : aNewROC;     // pointer to 3 BYTES ROC number.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetROC(void *aNewROC)
{
  struct TERM_DATA term_data;

  CheckPointerAddr(aNewROC);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memcpy(term_data.sb_roc_no, aNewROC, 3);
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : SetTrace
//  Description     : Set the trace no.
//  Input           : aNewTrace;     // pointer to 3 BYTES Trace number.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetTrace(void *aNewTrace)
{
  struct TERM_DATA term_data;

  CheckPointerAddr(aNewTrace);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memcpy(term_data.sb_trace_no, aNewTrace, 3);
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : GetTrace
//  Description     : Get the trace no.
//  Input           : aTrace;     // pointer to 3 BYTES Trace number.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GetTrace(void *aTrace)
{
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset->sb_trace_no, aTrace, 3);
}
//*****************************************************************************
//  Function        : IncTrace
//  Description     : Update the trace no.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IncTrace(void)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  bcdinc(term_data.sb_trace_no, 3);
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : CleanTermData
//  Description     : Reset TermData Table to default value.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CleanTermData(void)
{
  fCommitAllMW();
  CloseTermData();
  CreateTermData();
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : TermDataUpd
//  Description     : Update Termdata table.
//  Input           : aDat;         // pointer to table data.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD TermDataUpd(void *aDat)
{
  DWORD ret;
  CheckPointerAddr(aDat);
  ret = UpdTermData(aDat);
  fCommitAllMW();
  return ret;
}
//29-09-16 JC ++
#if 0
//*****************************************************************************
//  Function        : UpdateFTPUser
//  Description     : Update FTP download user.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN UpdateFTPUser(void)
{
  struct TERM_DATA term_data;

  BYTE kbdbuf[MAX_INPUT_LEN+1];

  DispClrBelowMW(MW_LINE3);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset->sb_ftp_user, &kbdbuf[1], sizeof(psTermDataOffset->sb_ftp_user));
  kbdbuf[0] = fndb(&kbdbuf[1], 0x00, sizeof(psTermDataOffset->sb_ftp_user));

  DispLineMW(KFTPUser, MW_LINE5, MW_BIGFONT);

  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(sizeof(psTermDataOffset->sb_ftp_user))-1, kbdbuf)) //-1 for save the 0x00 as terminator
    return FALSE;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memset(term_data.sb_ftp_user, 0x00, sizeof(psTermDataOffset->sb_ftp_user));
  memcpy(term_data.sb_ftp_user, &kbdbuf[1], kbdbuf[0]);
  UpdTermData(&term_data);
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdateFTPPwd
//  Description     : Update FTP download pwd.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN UpdateFTPPwd(void)
{
  struct TERM_DATA term_data;

  BYTE kbdbuf[MAX_INPUT_LEN+1];

  DispClrBelowMW(MW_LINE3);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset->sb_ftp_pwd, &kbdbuf[1], sizeof(psTermDataOffset->sb_ftp_pwd));
  kbdbuf[0] = fndb(&kbdbuf[1], 0x00, sizeof(psTermDataOffset->sb_ftp_pwd));

  DispLineMW(KFTPPwd, MW_LINE5, MW_BIGFONT);

  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(sizeof(psTermDataOffset->sb_ftp_pwd))-1, kbdbuf)) //-1 for save the 0x00 as terminator
    return FALSE;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memset(term_data.sb_ftp_pwd, 0x00, sizeof(psTermDataOffset->sb_ftp_pwd));
  memcpy(term_data.sb_ftp_pwd, &kbdbuf[1], kbdbuf[0]);
  UpdTermData(&term_data);
  return TRUE;
}
#endif
//*****************************************************************************
//  Function        : UpdatePPPUser
//  Description     : Update FTP download user.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN UpdatePPPUser(void)
{
  struct TERM_DATA term_data;

  BYTE kbdbuf[MAX_INPUT_LEN+1];

  DispClrBelowMW(MW_LINE3);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset->sb_ppp_user, &kbdbuf[1], sizeof(psTermDataOffset->sb_ppp_user));
  kbdbuf[0] = fndb(&kbdbuf[1], 0x00, sizeof(psTermDataOffset->sb_ppp_user));

  DispLineMW(KPPPUser, MW_LINE5, MW_BIGFONT);

  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(sizeof(psTermDataOffset->sb_ppp_user))-1, kbdbuf)) //-1 for save the 0x00 as terminator
    return FALSE;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memset(term_data.sb_ppp_user, 0x00, sizeof(psTermDataOffset->sb_ppp_user));
  memcpy(term_data.sb_ppp_user, &kbdbuf[1], kbdbuf[0]);
  UpdTermData(&term_data);
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdatePPPPwd
//  Description     : Update FTP download pwd.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN UpdatePPPPwd(void)
{
  struct TERM_DATA term_data;

  BYTE kbdbuf[MAX_INPUT_LEN+1];

  DispClrBelowMW(MW_LINE3);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset->sb_ppp_pwd, &kbdbuf[1], sizeof(psTermDataOffset->sb_ppp_pwd));
  kbdbuf[0] = fndb(&kbdbuf[1], 0x00, sizeof(psTermDataOffset->sb_ppp_pwd));

  DispLineMW(KPPPPwd, MW_LINE5, MW_BIGFONT);

  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(sizeof(psTermDataOffset->sb_ppp_pwd))-1, kbdbuf)) //-1 for save the 0x00 as terminator
    return FALSE;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memset(term_data.sb_ppp_pwd, 0x00, sizeof(psTermDataOffset->sb_ppp_pwd));
  memcpy(term_data.sb_ppp_pwd, &kbdbuf[1], kbdbuf[0]);
  UpdTermData(&term_data);
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdateFTPServerKey
//  Description     : Update FTP download pwd.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdateFTPServerKey(void)
{
  struct TERM_DATA term_data;

  BYTE kbdbuf[MAX_INPUT_LEN+1];

  DispClrBelowMW(MW_LINE3);
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset->sb_server_key, &kbdbuf[1], sizeof(psTermDataOffset->sb_server_key));
  kbdbuf[0] = fndb(&kbdbuf[1], 0x00, sizeof(psTermDataOffset->sb_server_key));

  DispLineMW(KFTPServerKey, MW_LINE5, MW_BIGFONT);

  if (!GetKbd(ALPHA_INPUT_NEW+ECHO+MW_BIGFONT+MW_LINE7+RIGHT_JUST, IMAX(sizeof(psTermDataOffset->sb_server_key))-1, kbdbuf)) //-1 for save the 0x00 as terminator
    return FALSE;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memset(term_data.sb_server_key, 0x00, sizeof(psTermDataOffset->sb_server_key));
  memcpy(term_data.sb_server_key, &kbdbuf[1], kbdbuf[0]);
  UpdTermData(&term_data);
  return TRUE;
}
//*****************************************************************************
//  Function        : SetInitPPPIP
//  Description     : Save Init ip & port for PPP connection.
//  Input           : N/A
//  Return          : TRUE;     // VALID INPUT
//                    FALSE;    // Cancel
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SetInitPPPIP(void)
{
  struct TERM_DATA term_data;
  DWORD port;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  DispClrBelowMW(MW_LINE3);
  //DispLineMW(KInitPPPIPHdr, MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
  //30-06-14++ SC (1)
  //port = term_data.sb_port[0]*256 + term_data.sb_port[1];
  port = term_data.sb_ppp_port[0]*256 + term_data.sb_ppp_port[1];
  //30-06-14-- SC (1)

  DispLineMW(KInitPPPIP, MW_LINE3, MW_SMFONT);
  if (!EditIp(term_data.sb_ppp_ip, MW_LINE4))
    return FALSE;

  DispLineMW(KInitPPPPort, MW_LINE5, MW_SMFONT);
  if (!EditPort(&port, MW_LINE6))
    return FALSE;

  DispLineMW(KInitEnterSave, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_CENTER|MW_SPFONT);
  if (WaitKey(KBD_TIMEOUT) != MWKEY_ENTER)
    return FALSE;

  term_data.sb_ppp_port[0]=(BYTE)(port/256);
  term_data.sb_ppp_port[1]=(BYTE)(port&0xFF);
  UpdTermData(&term_data);
  return TRUE;
}
//*****************************************************************************
//  Function        : STISFTPSetup
//  Description     : Setup STIS ftp init parameters.
//  Input           : N/A
//  Return          : N/A
//  Note            : 22-04-13++ SC (1)
//  Globals Changed : N/A
//*****************************************************************************
void STISFTPSetup(void)
{
  struct TERM_DATA term_data;
  char option;
  BYTE tmp[12];
  BYTE init_mode;

  DispLineMW("STIS Setup", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  while (1) {
    if (!UpdateTermID())
      return;

    // Init Mode
    ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
    switch (term_data.b_init_mode) {
      case APM_COMM_TCPIP:
        option = 1;
        break;
      case APM_COMM_GPRS:
        option = 2;
        break;
      case APM_COMM_WIFI:
        option = 3;
        break;
      case APM_COMM_ASYNC:
      default:
        option = 0;
        break;
    }
    DispClrBelowMW(MW_LINE3);
    DispLineMW(KInitModeStr, MW_LINE5, MW_BIGFONT);
    option = ToggleOption(NULL, KFTPInitMode, option);
    if (option == -1)
      return;

    switch (option) {
      case 0:
        term_data.b_init_mode = APM_COMM_ASYNC;
        break;
      case 1:
        term_data.b_init_mode = APM_COMM_TCPIP;
        break;
      case 2:
        term_data.b_init_mode = APM_COMM_GPRS;
        break;
      case 3:
        if ((os_hd_config_extend() & K_XHdWifi) == 0) {
          DispClrBelowMW(MW_LINE3);
          DispLineMW(KNonWifiModel, MW_LINE5, MW_BIGFONT);
          LongBeep();
          WaitKey(KBD_TIMEOUT);
          return;
        }
        term_data.b_init_mode = APM_COMM_WIFI;
        break;
    }
    UpdTermData(&term_data);

    init_mode = option;
    // setup for dial mode
    switch (init_mode) {
      case 1: //TCPIP
      case 3: //Wifi
        //term_data.b_init_mode = APM_COMM_TCPIP;
        if (!SetInitIP())
          return;
        break;
      case 0: //Dial
        //The file can be change in TCP settings, reload the changess
        ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));

        //term_data.b_init_mode = APM_COMM_ASYNC;
//Download PPP Phone 1
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KInitPriNum, MW_LINE5, MW_BIGFONT);
        memcpy(tmp, term_data.sb_pri_no, 12);
        if (!UpdateFData(tmp, 12))
          return;
        memcpy(term_data.sb_pri_no, tmp, 12);
        UpdTermData(&term_data);

//Download PPP Phone 2
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KInitSecNum, MW_LINE5, MW_BIGFONT);
        memcpy(tmp, term_data.sb_sec_no, 12);
        if (!UpdateFData(tmp, 12))
          return;
        memcpy(term_data.sb_sec_no, tmp, 12);
        UpdTermData(&term_data);

//Check In PPP Phone 1
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KCheckInPriNum, MW_LINE5, MW_BIGFONT);
        memcpy(tmp, term_data.sb_checkin_pri_no, 12);
        if (!UpdateFData(tmp, 12))
          return;
        memcpy(term_data.sb_checkin_pri_no, tmp, 12);
        UpdTermData(&term_data);

//Check In PPP Phone 2
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KCheckInSecNum, MW_LINE5, MW_BIGFONT);
        memcpy(tmp, term_data.sb_checkin_sec_no, 12);
        if (!UpdateFData(tmp, 12))
          return;
        memcpy(term_data.sb_checkin_sec_no, tmp, 12);
        UpdTermData(&term_data);

        option = term_data.b_dial_mode;
        DispClrBelowMW(MW_LINE3);
        DispLineMW(KDialModeStr, MW_LINE5, MW_BIGFONT);
        option = ToggleOption(NULL, KDialMode, option);
        if (option==-1)
          return;
        term_data.b_dial_mode = option;
        UpdTermData(&term_data);

        DispClrBelowMW(MW_LINE3);
        if (!UpdatePABX())
          return;

        if (DebugModeON()) { //19-06-14++ SC (2)
          if (!UpdatePPPUser())
            return;

          if (!UpdatePPPPwd())
            return;
        }

        if (!SetInitPPPIP())
          return;

        break;
      case 2: //GPRS
        //term_data.b_init_mode = APM_COMM_GPRS;
        DispClrBelowMW(MW_LINE3);
        if (!SetGPRS())
          return;
        if (!SetInitIP())
          return;
        break;
    }

#if 0
    if (DebugModeON()) { //19-06-14++ SC (2)
      if (!UpdateFTPUser())
        return;

      if (!UpdateFTPPwd())
        return;

      //if (!UpdateFTPServerKey())
      //  return;
    }
#endif

    AcceptBeep();
    if (GetSTISMode() == SETUP_MODE)
      SetSTISMode(INIT_MODE);
    if (CancelExit()) {
      fCommitAllMW();
      break;
    }
  }
}
//*****************************************************************************
//  Function        : CompareEDCDateChksum
//  Description     : Compare EDC param date and checksum.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CompareEDCDateChksum(BYTE *aChksum, BYTE *aDate)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  if (memcmp(term_data.sb_edc_chksum, aChksum, 2) == 0)
    if (memcmp(term_data.sb_edc_date, aDate, 4) == 0)
      return TRUE;

  return FALSE;
}
//*****************************************************************************
//  Function        : SetEDCDateChksumSize
//  Description     : Change STIS EDC param date and checksum.
//  Input           : STIS mode;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetEDCDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memcpy(term_data.sb_edc_chksum, aChksum, 2);
  memcpy(term_data.sb_edc_date, aDate, 4);
  term_data.sb_edc_size = aSize;
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : CompareEMVAPPDateChksum
//  Description     : Compare EMVAPP param date and checksum.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CompareEMVAPPDateChksum(BYTE *aChksum, BYTE *aDate)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  if (memcmp(term_data.sb_emvapp_chksum, aChksum, 2) == 0)
    if (memcmp(term_data.sb_emvapp_date, aDate, 4) == 0)
      return TRUE;

  return FALSE;
}
//*****************************************************************************
//  Function        : SetEMVAPPDateChksumSize
//  Description     : Change STIS EDC param date and checksum.
//  Input           : STIS mode;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetEMVAPPDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memcpy(term_data.sb_emvapp_chksum, aChksum, 2);
  memcpy(term_data.sb_emvapp_date, aDate, 4);
  term_data.sb_emvapp_size = aSize;
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : CompareEMVKEYDateChksum
//  Description     : Compare EMVAPP param date and checksum.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CompareEMVKEYDateChksum(BYTE *aChksum, BYTE *aDate)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  if (memcmp(term_data.sb_emvkey_chksum, aChksum, 2) == 0)
    if (memcmp(term_data.sb_emvkey_date, aDate, 4) == 0)
      return TRUE;

  return FALSE;
}
//*****************************************************************************
//  Function        : SetEMVKEYDateChksumSize
//  Description     : Change STIS EDC param date and checksum.
//  Input           : STIS mode;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetEMVKEYDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memcpy(term_data.sb_emvkey_chksum, aChksum, 2);
  memcpy(term_data.sb_emvkey_date, aDate, 4);
  term_data.sb_emvkey_size = aSize;
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : CompareXAPPDateChksum
//  Description     : Compare EMVAPP param date and checksum.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CompareXAPPDateChksum(BYTE *aChksum, BYTE *aDate)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  if (memcmp(term_data.sb_xapp_chksum, aChksum, 2) == 0)
    if (memcmp(term_data.sb_xapp_date, aDate, 4) == 0)
      return TRUE;

  return FALSE;
}
//*****************************************************************************
//  Function        : SetXAPPDateChksumSize
//  Description     : Change STIS EDC param date and checksum.
//  Input           : STIS mode;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetXAPPDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memcpy(term_data.sb_xapp_chksum, aChksum, 2);
  memcpy(term_data.sb_xapp_date, aDate, 4);
  term_data.sb_xapp_size = aSize;
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : CompareLXAPPDateChksum
//  Description     : Compare EMVAPP param date and checksum.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CompareLXAPPDateChksum(BYTE *aChksum, BYTE *aDate)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  if (memcmp(term_data.sb_lxapp_chksum, aChksum, 2) == 0)
    if (memcmp(term_data.sb_lxapp_date, aDate, 4) == 0)
      return TRUE;

  return FALSE;
}
//*****************************************************************************
//  Function        : SetLXAPPDateChksumSize
//  Description     : Change STIS EDC param date and checksum.
//  Input           : STIS mode;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetLXAPPDateChksumSize(BYTE *aChksum, BYTE *aDate, DDWORD aSize)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  memcpy(term_data.sb_lxapp_chksum, aChksum, 2);
  memcpy(term_data.sb_lxapp_date, aDate, 4);
  term_data.sb_lxapp_size = aSize;
  UpdTermData(&term_data);
}
//*****************************************************************************
//  Function        : AppListUploadReq
//  Description     : Return if application list upload is required.
//  Input           : N/A
//  Return          : TRUE: required;
//                    FALSE: no required.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE AppListUploadReq(void)
{
  BYTE upload_req;

  ReadSFile(iTermDataFd, (DWORD) &psTermDataOffset->b_app_list_upload, &upload_req, sizeof(upload_req));
  return upload_req;
}

//*****************************************************************************
//  Function        : SetAppListUploadReq
//  Description     : Set the application list checksum.
//  Input           : new value of the upload required
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetAppListUploadReq(BYTE aUploadReq)
{
  struct TERM_DATA term_data;

  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  term_data.b_app_list_upload = aUploadReq;
  UpdTermData(&term_data);
}

//*****************************************************************************
//  Function        : GetMirrorAppListChksum
//  Description     : Return the application list checksum.
//  Input           : N/A
//  Return          : application list chksum
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD GetMirrorAppListChksum(void)
{
  WORD w_app_list_chksum;

  ReadSFile(iTermDataFd, (DWORD) &psTermDataOffset->w_app_list_chksum, &w_app_list_chksum, sizeof(w_app_list_chksum));
  return w_app_list_chksum;
}

//*****************************************************************************
//  Function        : GetActualAppListChksum
//  Description     : Return the application list checksum.
//  Input           : N/A
//  Return          : application list chksum
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD GetActualAppListChksum(void)
{
  struct MW_APPL_INFO app_info;
  WORD checksum;
  BYTE i;

  checksum = 0x0000;
  for (i=0; i<0xFF; i++) {
    if (AppInfoGetMW(i, &app_info)) {
      checksum += app_info.w_checksum;
    }
  }

  return checksum;
}

//*****************************************************************************
//  Function        : SetAppListChksum
//  Description     : Set the application list checksum.
//  Input           : New application list checksum
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void UpdateAppListChksum(void)
{
  struct TERM_DATA term_data;
  struct MW_APPL_INFO app_info;
  WORD checksum;
  BYTE i;

  checksum = 0x0000;
  for (i=0; i<0xFF; i++) {
    if (AppInfoGetMW(i, &app_info)) {
      checksum += app_info.w_checksum;
    }
  }
  ReadSFile(iTermDataFd, (DWORD) psTermDataOffset, &term_data, sizeof(struct TERM_DATA));
  term_data.w_app_list_chksum = checksum;
  UpdTermData(&term_data);
}
//29-09-16 JC --
