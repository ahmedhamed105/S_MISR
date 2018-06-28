//-----------------------------------------------------------------------------
//  File          : toggle.c
//  Module        :
//  Description   : Include Toggle Selection functions.
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
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
#include "sysutil.h"
#include "kbdutil.h"
#include "toggle.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
const char *KInitMode[] =    { "SYNC  ", "ASYNC ", "TCP/IP", "GPRS  ", "AUX   ", "WIFI  ", NULL};
#if (MCP_SUPPORT)
const char *KInitModeBt[] =  { "SYNC  ", "ASYNC ", "TCP/IP", "GPRS  ", "AUX   ", "WIFI  ", "BT    ", NULL};
#endif
const char *KDialMode[] =  { "TONE ", "PULSE", NULL};
const char *KEnable[] =  {"Disable", " Enable", NULL};
#if (PR608|R700)
const char *KAuxPort[]     = { "NONE", "AUX1", "AUX2", NULL};
const char *KAuxUsbPort[]  = { "NONE", "AUX1", "AUX2", "USB ", NULL};
#elif (TIRO)
const char *KAuxPort[]     = { "NONE", "AUXD", "AUX1", "AUX2", NULL};
const char *KAuxUsbPort[]  = { "NONE", "AUXD", "AUX1", "AUX2", "USB ", NULL};
#elif (T700|T800)
const char *KAuxPort[]     = { "NONE", "AUX1", "AUXD", NULL};
const char *KAuxUsbPort[]  = { "NONE", "AUX1", "AUXD", "USB ", NULL};
#elif (T300|T810|A5T1000|T1000)
const char *KAuxPort[]     = { "NONE", "AUX1", "AUXD", NULL};
const char *KAuxUsbPort[]  = { "NONE", "AUX1", "AUXD", "USB ", "LAN ", NULL};
const char *KAuxUsbWifiPort[]   = { "NONE", "AUX1", "AUXD", "USB ", "LAN ", "WIFI", NULL};
#if (MCP_SUPPORT)
const char *KAuxUsbWifiBtPort[] = { "NONE", "AUX1", "AUXD", "USB ", "LAN ", "WIFI", "BT  ", NULL};
#endif
#elif (PR608D)
const char *KAuxPort[]     = { "NONE", "ECR1", "ECR2", NULL};
const char *KAuxUsbPort[]  = { "NONE", "ECR1", "ECR2", NULL};
#elif (PR500)
const char *KAuxPort[]     = { "NONE", "AUXD", NULL};
const char *KAuxUsbPort[]  = { "NONE", "AUXD", NULL};
#else
NEW_TERMINAL_HERE
#endif
const char *KYesNo[]       = { "YES", "NO ", NULL};
const char *KSimSel[]      = { "01", "02", NULL};
const char *KFTPInitMode[] = { "DIAL  ", "TCP/IP", "GPRS  ", "WIFI  ", NULL};    //29-09-16 JC ++

//*****************************************************************************
//  Function        : ToggleOption
//  Description     : Prompt user to toggle option.
//  Input           : aMsg;         // message to display
//                    aOptTbl;      // Pointer to Option Table
//                    aCurVal;      // current option & Display Mode
//  Return          : user select option.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
char ToggleOption(const char *aMsg, const char **aOptTbl, DWORD aCurVal)
{
  BYTE max_opt, max_len, len;
  BYTE cur_val;
  DWORD mode;


  // Find Max Option & Len;
  max_len = 0;
  for (max_opt = 0; max_opt < MAX_OPTION_LIST; max_opt++) {
    if (aOptTbl[max_opt] == NULL)
      break;
    len = strlen(aOptTbl[max_opt]);
    max_len = max_len < len ? len : max_len;
  }


  cur_val = (BYTE)(aCurVal & 0xFF);
  mode    = aCurVal & 0xFF00;
  if (cur_val > max_opt)
    cur_val = 0;

  if (aMsg != NULL) {
    if (mode == TOGGLE_MODE_4LINE) {
      DispLineMW(aMsg, MW_LINE5, MW_BIGFONT);
    }
    else {
      DispLineMW(aMsg, MW_LINE7, MW_SMFONT);
    }
  }
  while (1) {
    if (mode == TOGGLE_MODE_4LINE) {
      DispLineMW(aOptTbl[cur_val], MW_LINE7, MW_REVERSE|MW_RIGHT|MW_BIGFONT);
    }
    else {
      DispLineMW(aOptTbl[cur_val], MW_LINE8, MW_REVERSE|MW_RIGHT|MW_SMFONT);
    }
    switch (WaitKey(KBD_TIMEOUT)) {
      case MWKEY_ENTER:
      case MWKEY_SELECT:
        return(cur_val);
      case MWKEY_CANCL:
        return -1;
      case MWKEY_CLR:
        cur_val = (cur_val+1)%max_opt;
        break;
      default:
        LongBeep();
    }
  }
}
//*****************************************************************************
//  Function        : ToggleYesNo
//  Description     : Wait user to select YES or NO.
//  Input           : N/A
//  Return          : 0 - Cancel key pressed,
//                    1 - No Select
//                    2 - Yes Select
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ToggleYesNo(void)
{
  DWORD ret;
  char select = ToggleOption(NULL, KYesNo, 0);

  switch (select) {
    case 0: // Yes
      ret = 2;
      break;
    case 1: // No
      ret = 1;
      break;
    default:
      ret = 0;
      break;
  }
  return ret;
}
