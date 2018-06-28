//-----------------------------------------------------------------------------
//  File          : toggle.c
//  Module        :
//  Description   : Include Toggle Selection functions.
//  Author        : Lewis
//  Notes         :
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
//  01 Dec  2007 Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include "midware.h"
#include "hwdef.h"
#include "apm.h"
#include "sysutil.h"
#include "toggle.h"

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
#ifndef DEBUG_MODE
const char *KInitMode[] =  { "SYNC  ", "TCP/IP", "ASYNC ", NULL};
#else
const char *KInitMode[] =  { "SYNC  ", "TCP/IP", "ASYNC ", "AUX   ", NULL};
#endif
const char *KTrueFalse[] =  { "FALSE", "TRUE ", NULL};
const char *KDialMode[]  =  { "TONE ", "PULSE", NULL};
const char *KOnOff[]  =  {"OFF","ON ", NULL};
const char *KEnable[] =  {"Disable", " Enable", NULL};
const char *KAuxSpeed[] = { "  9600", "  2400", "  4800", " 19200", " 28800", " 38400", " 57600", "115200", "230400", NULL};
const char *KAuxPort[]  = { "AUXD", "AUX1", "AUX2", NULL};
const char *KAuxCommMode[]  = { "    STX", " EN_STX", "SOH_LRC", "SOH_CRC", "ASYN_N1", "ASYN_E1", "ASYN_O1", NULL};
const char *KAuxDataLen[] =  { "1 Byte", "2 Byte", NULL};

const char *KPPPport[]   = { "LINE", "AUX1", "AUX2", "AUXD", "GPRS", NULL};
const char *KProtoMode[] = { "SDLC ", "ASYNC", NULL};
const char *KCcittMode[] = { "CCITT", "BELL ", NULL};
const char *KMdmSpeed[]  = { "  300", " 1200", " 2400", " 9600", " Auto", " V.42", "Union", "11400", "33600", "56000", NULL};

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
  DWORD mode, line_no, font;

  // Find Max Option & Len;
  max_len = 0;
  for (max_opt = 0; max_opt < MAX_OPTION_LIST; max_opt++) {
    if (aOptTbl[max_opt] == NULL)
      break;
    len = strlen(aOptTbl[max_opt]);
    max_len = max_len < len ? len : max_len;
  }

  mode    = aCurVal & 0xFF0000;
  if (aMsg != NULL) {
    if (mode == TOGGLE_MODE_4LINE) {
      line_no = MW_LINE5;
      font    = MW_BIGFONT;
    }
    else {
      line_no = MW_LINE7;
      font    = MW_SMFONT;
    }
    DispLineMW(aMsg, line_no, font);
  }

  cur_val = (BYTE)(aCurVal & 0xFF);
  if (cur_val > max_opt)
    cur_val = 0;

  while (1) {
    DispCtrlMW(MW_REV_ON);
    if (mode == TOGGLE_MODE_4LINE) {
      line_no = MW_LINE7;
      font    = MW_BIGFONT;
    }
    else {
      line_no = MW_LINE8;
      font    = MW_SMFONT;
    }
    DispLineMW(aOptTbl[cur_val], line_no, MW_RIGHT|font);
    DispCtrlMW(MW_REV_OFF);
    switch (APM_WaitKey(KBD_TIMEOUT,0)) {
      case MWKEY_ENTER:
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

