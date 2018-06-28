//-----------------------------------------------------------------------------
//  File          : kbdutil.c
//  Module        :
//  Description   : Provide keyboard utilities.
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "midware.h"
#include "sysutil.h"
#include "hardware.h"
#include "kbdutil.h"

//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define ALPHA_INPUT_TIMEOUT  200

//-----------------------------------------------------------------------------
//  Global Varialbles
//-----------------------------------------------------------------------------
struct KBDGDS {
  DDWORD ddAmount;
  WORD wFont;
  BYTE bDecimalPos;
  BYTE sbAmtBuf[12];
  BYTE sbPrefix[4];
  WORD wLineNo;
  BYTE bInputLen;
  BYTE sbInputBuf[MAX_INPUT_LEN+1];
  BYTE bDispLen;
  BYTE sbDispBuf[MAX_INPUT_LEN+1];
  DWORD dEntryMode;
  DWORD dOption;
  DWORD dTimeout;        // GetKbd Timeout Value
  int   iTimerHdl;
  BYTE  bRightJust;      // Right Justify
};
static struct KBDGDS gKbdGDS = {
  0, 0, (BYTE)(DECIMAL_POS2>>24),
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {' ', ' ', ' ', ' '},
  0,
  0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  0,
  0,
  KBD_TIMEOUT,
  -1,
};

//*****************************************************************************
//  Function        : InitTimer
//  Description     : Init KBDUTIL internal timer if necessary.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void InitTimer(void)
{
  if (gKbdGDS.iTimerHdl == -1) {
    gKbdGDS.iTimerHdl = TimerOpenMW();
  }
}
//*****************************************************************************
//  Function        : ClearKeyIn
//  Description     : Clear all key buffer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClearKeyin(void)
{
  InitTimer();
  while (GetCharMW()!=0) SleepMW();
}
//*****************************************************************************
//  Function        : GetKeyin
//  Description     : Return a key when available.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetKeyin(void)
{
  DWORD keyin;

  InitTimer();
  keyin = GetCharMW();
  if ((keyin==0)&&(TimerGetMW(gKbdGDS.iTimerHdl)==0))
    return MWKEY_CANCL;

  return keyin;
}
//*****************************************************************************
//  Function        : SetKbdTimeout
//  Description     : Set kbd timeout value.
//  Input           : value in ms;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetKbdTimeout(DWORD aTimeout)
{
  InitTimer();
  gKbdGDS.dTimeout = aTimeout;
  TimerSetMW(gKbdGDS.iTimerHdl, aTimeout);
}
//*****************************************************************************
//  Function        : GetKbdTimeout
//  Description     : Get keyboard timeout value.
//  Input           : N/A
//  Return          : getkbd timeout in sec.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetKbdTimeout(void)
{
  InitTimer();
  return TimerGetMW(gKbdGDS.iTimerHdl);
}
//*****************************************************************************
//  Function        : WaitKey
//  Description     : Wait a key for specify time.
//  Input           : timeout;   // timeout value in second
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD WaitKey(DWORD aTimeout)
{
  DWORD keyin, i;

  aTimeout *=2;     // convert to 5ms unit.
  keyin = i = 0;
  do {
    SleepMW();
    keyin = GetCharMW();
  } while ((i++ < aTimeout) && (keyin == 0));
  return(keyin);
}
//*****************************************************************************
//  Function        : WaitKeyCheck
//  Description     : Wait a key for specify time.
//  Input           : timeout;   // timeout value in second
//                    aCheckList
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD WaitKeyCheck(DWORD aTimeout, DWORD aCheckList)
{
  DWORD keyin;
  //DWORD icc_id;

  //icc_id = 0;
  SetKbdTimeout(aTimeout);
  while ((keyin=GetKeyin())==0) {

    if (aCheckList & WAIT_ICC_INSERT) {
      if (ICCInsertedMW()&0x01) {
        keyin = WAIT_ICC_INSERT;
        break;
      }
    }

    if (aCheckList & WAIT_ICC_REMOVE) {
      if ((ICCInsertedMW()&0x01)==0x00) {
        keyin = WAIT_ICC_REMOVE;
        break;
      }
    }

    if ((aCheckList & WAIT_MSR_READY) && (gDevHandle[APM_SDEV_MSR] != -1)) {
      if (IOCtlMW(gDevHandle[APM_SDEV_MSR], IO_MSR_RXLEN, NULL) > 0) {
        keyin = WAIT_MSR_READY;
        break;
      }
    }

    SleepMW();
  }
  return(keyin);
}
//*****************************************************************************
//  Function        : YesNo
//  Description     : Wait user to select YES or NO.
//  Input           : N/A
//  Return          : 0 - Cancel key pressed,
//                    1 - Clear key pressed(NO)
//                    2 - Enter key pressed(YES)
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD YesNo(void)
{
  while (1) {
    switch (WaitKey(gKbdGDS.dTimeout)) {
      case MWKEY_ENTER:
        return 2;
      case MWKEY_CLR:
        return 1;
      case MWKEY_CANCL:
        return 0;
    }
  }
}
//*****************************************************************************
//  Function        : UpdateDisplay
//  Description     : Show the Buffer data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void UpdateDisplay(void)
{
  BYTE decimal_pos;
  BYTE *pBuf = gKbdGDS.sbDispBuf;
  BYTE tmpbuf[MW_MAX_LINESIZE+1];
  BYTE disp_len;

  memset(gKbdGDS.sbDispBuf, ' ', sizeof(gKbdGDS.sbDispBuf));
  switch (gKbdGDS.dEntryMode) {
    case HIDE_NUMERIC:
      memset(gKbdGDS.sbDispBuf, '*', gKbdGDS.bInputLen);
      gKbdGDS.bDispLen = gKbdGDS.bInputLen;
      break;
    case DECIMAL_INPUT:
    case AMOUNT_INPUT:
      gKbdGDS.bDispLen = gKbdGDS.bInputLen;
      memcpy(gKbdGDS.sbDispBuf, gKbdGDS.sbInputBuf,gKbdGDS.bInputLen);
      if ((gKbdGDS.bDispLen==0)|| (gKbdGDS.bDispLen<=gKbdGDS.bDecimalPos)) {
        gKbdGDS.bDispLen = gKbdGDS.bDecimalPos + 1;
        memset(gKbdGDS.sbDispBuf, '0', gKbdGDS.bDispLen);
        memcpy(&gKbdGDS.sbDispBuf[gKbdGDS.bDispLen-gKbdGDS.bInputLen],
               gKbdGDS.sbInputBuf, gKbdGDS.bInputLen);
      }
      if (gKbdGDS.bDecimalPos) {
        decimal_pos = gKbdGDS.bDispLen-gKbdGDS.bDecimalPos;
        memmove(&gKbdGDS.sbDispBuf[decimal_pos+1] , &gKbdGDS.sbDispBuf[decimal_pos], gKbdGDS.bDecimalPos);
        gKbdGDS.sbDispBuf[decimal_pos] = '.';
        gKbdGDS.bDispLen++;
      }
      if (gKbdGDS.dOption & PREFIX_ENB) {
        memmove(&gKbdGDS.sbDispBuf[PREFIX_SIZE], gKbdGDS.sbDispBuf, gKbdGDS.bDispLen);
        memcpy(gKbdGDS.sbDispBuf, gKbdGDS.sbPrefix, PREFIX_SIZE);
        gKbdGDS.bDispLen += PREFIX_SIZE;
      }
      break;
    default:
      memcpy(gKbdGDS.sbDispBuf, gKbdGDS.sbInputBuf, gKbdGDS.bInputLen);
      gKbdGDS.bDispLen = gKbdGDS.bInputLen;
      break;
  }
  pBuf[gKbdGDS.bDispLen] = 0;
  if (gKbdGDS.wFont==MW_BIGFONT) {
    DispClrLineMW(gKbdGDS.wLineNo);
    if (gKbdGDS.bDispLen<=MW_NOR_LINESIZE) {
      DispLineMW(pBuf, gKbdGDS.wLineNo, (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
      //DispClrLineMW(gKbdGDS.wLineNo+(MW_LINE4-MW_LINE2));   //03-10-26 JC ++
    }
    else {
      disp_len = gKbdGDS.bDispLen-MW_NOR_LINESIZE;
      memcpy(tmpbuf, pBuf, disp_len);
      tmpbuf[disp_len] = 0;
      DispLineMW(tmpbuf, gKbdGDS.wLineNo, (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
      pBuf += disp_len;
      DispLineMW(pBuf, gKbdGDS.wLineNo+(MW_LINE4-MW_LINE2), (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
    }
  }
  else if (gKbdGDS.wFont==MW_SMFONT) {
    DispClrLineMW(gKbdGDS.wLineNo);
    if (gKbdGDS.bDispLen<=MW_NOR_LINESIZE) {
      DispLineMW(pBuf, gKbdGDS.wLineNo, (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
      //DispClrLineMW(gKbdGDS.wLineNo+(MW_LINE3-MW_LINE2));   //03-10-26 JC ++
    }
    else {
      disp_len = gKbdGDS.bDispLen-MW_NOR_LINESIZE;
      memcpy(tmpbuf, pBuf, disp_len);
      tmpbuf[disp_len] = 0;
      DispLineMW(tmpbuf, gKbdGDS.wLineNo, (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
      pBuf += disp_len;
      DispLineMW(pBuf, gKbdGDS.wLineNo+(MW_LINE3-MW_LINE2), (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
    }
  }
  else {
    DispClrLineMW(gKbdGDS.wLineNo);
    if (gKbdGDS.bDispLen<=MW_MAX_LINESIZE) {
      DispLineMW(pBuf, gKbdGDS.wLineNo, (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
      //DispClrLineMW(gKbdGDS.wLineNo+(MW_LINE3-MW_LINE2));   //03-10-26 JC ++
    }
    else {
      disp_len = gKbdGDS.bDispLen-MW_MAX_LINESIZE;
      memcpy(tmpbuf, pBuf, disp_len);
      tmpbuf[disp_len] = 0;
      DispLineMW(tmpbuf, gKbdGDS.wLineNo, (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
      pBuf += disp_len;
      DispLineMW(pBuf, gKbdGDS.wLineNo+(MW_LINE3-MW_LINE2), (gKbdGDS.bRightJust?MW_RIGHT:0)|gKbdGDS.wFont|MW_CLREOL);
    }
  }
}
//*****************************************************************************
//  Function        : ConvertAlphaNew
//  Description     : Convert the input char to alpha.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : gKbdGDS
//*****************************************************************************
static void ConvertAlphaNew(void)
{
  BYTE i, j;

  static const BYTE alpha_tbl[10][9]
  = {{'0',' ','+','-','"','*','#',')','~'},
    {'1','Q','Z','q','z',':','.','!','`'},
    {'2','A','B','C','a','b','c','@','\x27'},
    {'3','D','E','F','d','e','f','|','='},
    {'4','G','H','I','g','h','i','$',';'},
    {'5','J','K','L','j','k','l','%','?'},
    {'6','M','N','O','m','n','o','^',','},
    {'7','P','R','S','p','r','s','&','_'},
    {'8','T','U','V','t','u','v','/','['},
    {'9','W','X','Y','w','x','y','(',']'}
  };


  if (gKbdGDS.bInputLen == 0) {
    LongBeep();
    return;
  }

  for (i = 0; i != 10; i++)
  {
    for (j = 0; j != 9 ; j++)
    {
      if (gKbdGDS.sbInputBuf[gKbdGDS.bInputLen-1] == alpha_tbl[i][j]) {
        j = alpha_tbl[i][(j + 1) % 9];
        goto FOUND;
      }
    }
  }
FOUND:
  if (gKbdGDS.dEntryMode  == HEX_INPUT)
    if ((j > 'F') || (j < '0') || ((j > '9') && (j < 'A')))
      j = alpha_tbl[i][0];
  gKbdGDS.sbInputBuf[gKbdGDS.bInputLen-1] = j;
}
//*****************************************************************************
//  Function        : ConvertAlpha
//  Description     : Convert the input char to alpha.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : gKbdGDS;
//*****************************************************************************
static void ConvertAlpha(void)
{
  BYTE i, j;
  static const BYTE alpha_tbl[10][6]
  = {{'0',' ','+','-',')','~'},
    {'1','Q','Z','_','!','`'},
    {'2','A','B','C','@','\x27'},
    {'3','D','E','F','#',':'},
    {'4','G','H','I','$',';'},
    {'5','J','K','L','%','?'},
    {'6','M','N','O','^',','},
    {'7','P','R','S','&','.'},
    {'8','T','U','V','*','['},
    {'9','W','X','Y','(',']'}
  };

  if (gKbdGDS.bInputLen == 0) {
    LongBeep();
    return;
  }

  for (i = 0; i != 10; i++)
  {
    for (j = 0; j != 6 ; j++)
    {
      if (toupper(gKbdGDS.sbInputBuf[gKbdGDS.bInputLen-1]) == alpha_tbl[i][j]) {
        j = alpha_tbl[i][(j + 1) % 6];
        goto FOUND;
      }
    }
  }
FOUND:
  if (gKbdGDS.dEntryMode == HEX_INPUT)
    if ((j > 'F') || (j < '0') || ((j > '9') && (j < 'A')))
      j = alpha_tbl[i][0];
  gKbdGDS.sbInputBuf[gKbdGDS.bInputLen-1] = j;
}
//*****************************************************************************
//  Function        : GetKbd
//  Description     : Get Keyboard input base on parameters.
//  Input           : aEntryMode;       // Entry Mode + Line Number
//                    aLen;             // Max Len
//                    aOutBuf;          // Output Buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetKbd(DWORD aEntryMode, DWORD aLen, BYTE *aOutBuf)
{
  BOOLEAN keyed;
  DWORD keyin, last_key = 0;
  DWORD last_tick, min_len, max_len;
  BYTE  lead0len;

  CheckPointerAddr(aOutBuf);
  keyed = FALSE;
  min_len             = (aLen >> 8);
  max_len             = aLen & 0xFF;
  gKbdGDS.bRightJust  = ((aEntryMode&RIGHT_JUST)==RIGHT_JUST)?TRUE:FALSE;
  gKbdGDS.wLineNo     = (WORD)(gKbdGDS.bRightJust?(aEntryMode&0xF00):(aEntryMode&0xFFF));
  gKbdGDS.wFont       = (WORD)aEntryMode & 0xF000;
  gKbdGDS.dEntryMode  = aEntryMode & 0x00F0000;
  gKbdGDS.dOption     = aEntryMode & 0x0F00000;
  gKbdGDS.bDecimalPos = (BYTE)(aEntryMode >> 24);
  gKbdGDS.bInputLen   = 0;
  gKbdGDS.bDispLen    = 0;
  memset(gKbdGDS.sbInputBuf, ' ', sizeof(gKbdGDS.sbInputBuf));
  memset(gKbdGDS.sbPrefix, ' ', sizeof(gKbdGDS.sbPrefix));

  // Adjust the Amount input len.
  if (gKbdGDS.dEntryMode == AMOUNT_INPUT) {
    gKbdGDS.bRightJust  = TRUE;
    if ((max_len - gKbdGDS.bDecimalPos) > 12)
      /* host is always expecting 2 decimal digits */
      max_len = 12 - gKbdGDS.bDecimalPos;
  }

  //11-06-14 JC ++
  // Get PreFix first
  if (gKbdGDS.dOption & PREFIX_ENB) {
    memcpy(gKbdGDS.sbPrefix, &aOutBuf[1], PREFIX_SIZE);
    aOutBuf[0] -= PREFIX_SIZE;
    memcpy(&aOutBuf[1], &aOutBuf[5], aOutBuf[0]);
    if (max_len > MAX_INPUT_PREFIX)
      max_len = MAX_INPUT_PREFIX;
  }
  
  // Remove leading zero
  if ((gKbdGDS.dEntryMode == AMOUNT_INPUT) || (gKbdGDS.dEntryMode==DECIMAL_INPUT)) {
    lead0len = (BYTE)skpb(&aOutBuf[1], '0', aOutBuf[0]);
    aOutBuf[0] -= lead0len;
    if (aOutBuf[0] > 0)
      memmove(&aOutBuf[1], &aOutBuf[lead0len+1], aOutBuf[0]);
  }
  //11-06-14 JC --

  // Check ECHO
  if (!(gKbdGDS.dOption&ECHO)) {
    aOutBuf[0] = 0; /* clear aOutBuf if no echo */
  }
  if (aOutBuf[0]!=0) {
    gKbdGDS.bInputLen = (aOutBuf[0] > MAX_INPUT_LEN) ? MAX_INPUT_LEN: aOutBuf[0];
    memcpy(gKbdGDS.sbInputBuf, &aOutBuf[1], gKbdGDS.bInputLen);
    if (gKbdGDS.dEntryMode == AMOUNT_INPUT)
      keyed = TRUE;
  }
  UpdateDisplay();


  // Init Alpha input timeout
  last_tick = FreeRunMW();
  while (1) {
    gKbdGDS.dTimeout = (gKbdGDS.dTimeout == 0)?KBD_TIMEOUT:gKbdGDS.dTimeout;
    keyin=WaitKey(gKbdGDS.dTimeout);
    switch (keyin) {
      case MWKEY_CANCL:
        return 0;
      case MWKEY_CLR:
        if (gKbdGDS.bInputLen && (gKbdGDS.dEntryMode==AMOUNT_INPUT)) {
          DispClrLineMW(gKbdGDS.wLineNo);
          gKbdGDS.bInputLen = 0;
          aOutBuf[0] = 0;
        }
        else {
          keyed = TRUE;
          if (gKbdGDS.bInputLen)
            gKbdGDS.bInputLen -= 1;
        }
        UpdateDisplay();
        continue;
      case KEY_ALPHA: /* alpha key */
        if ((gKbdGDS.dEntryMode == ALPHA_INPUT) || (gKbdGDS.dEntryMode == HEX_INPUT)) {
          ConvertAlpha();
          UpdateDisplay();
          continue;
        }
        else
          if ((gKbdGDS.dEntryMode == ALPHA_INPUT_NEW) || (gKbdGDS.dEntryMode == HEX_INPUT)) {
            if (isupper(gKbdGDS.sbInputBuf[gKbdGDS.bInputLen]))
              gKbdGDS.sbInputBuf[gKbdGDS.bInputLen]=tolower(gKbdGDS.sbInputBuf[gKbdGDS.bInputLen]);
            else
              gKbdGDS.sbInputBuf[gKbdGDS.bInputLen]=toupper(gKbdGDS.sbInputBuf[gKbdGDS.bInputLen]);
            UpdateDisplay();
            continue;
          }
        break;
      case MWKEY_00:
        if (gKbdGDS.bInputLen || (gKbdGDS.dEntryMode != AMOUNT_INPUT)) {
          if (!keyed) {
            gKbdGDS.bInputLen = 0; /* first keyin, clear echoed entry */
            keyed = TRUE;
          }
          if (gKbdGDS.bInputLen < max_len-1) {
            memcpy(&gKbdGDS.sbInputBuf[gKbdGDS.bInputLen], "00", 2);
            gKbdGDS.bInputLen += 2;
            UpdateDisplay();
          }
          continue;
        }
        break;
      case MWKEY_ASTERISK:
      case MWKEY_SHARP:
        if ((gKbdGDS.dEntryMode != ALPHA_INPUT) && (gKbdGDS.dEntryMode != ALPHA_INPUT_NEW))
          break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '*':
        if ((gKbdGDS.dEntryMode == BINARY_INPUT) && (keyin > '1'))
          break;
        else
          if (gKbdGDS.bInputLen || (gKbdGDS.dEntryMode != AMOUNT_INPUT) || (keyin != '0')) {
            if (!keyed) {
              gKbdGDS.bInputLen = 0; /* first keyin, clear echoed entry */
              keyed = TRUE;
            }
            //03-10-26 JC ++
            if (keyin == MWKEY_ASTERISK)
              keyin = '*';
            else if (keyin == MWKEY_SHARP)
              keyin = '#';
            //03-10-26 JC --
            if (gKbdGDS.dEntryMode == ALPHA_INPUT_NEW) {
              // Update Display 1st
              if (gKbdGDS.bInputLen < max_len)
                gKbdGDS.sbInputBuf[gKbdGDS.bInputLen] = (BYTE) keyin;
              UpdateDisplay();
              if (((FreeRunMW()-last_tick) > ALPHA_INPUT_TIMEOUT)||(last_key != keyin)) {
                if (gKbdGDS.bInputLen < max_len) {
                  gKbdGDS.sbInputBuf[++gKbdGDS.bInputLen] = (BYTE) keyin;
                  last_key = keyin;
                }
              }
              else
                ConvertAlphaNew();
              last_tick = FreeRunMW();
            }
            else
              if ((gKbdGDS.dEntryMode == HEX_INPUT_NEW)&&(keyin=='2'||keyin=='3')) {
                // Update Display 1st
                if (gKbdGDS.bInputLen < max_len)
                  gKbdGDS.sbInputBuf[gKbdGDS.bInputLen] = (BYTE) keyin;
                UpdateDisplay();
                if (((FreeRunMW()-last_tick) > ALPHA_INPUT_TIMEOUT)||(last_key != keyin)) {
                  if (gKbdGDS.bInputLen < max_len) {
                    gKbdGDS.sbInputBuf[++gKbdGDS.bInputLen] = (BYTE) keyin;
                    last_key = keyin;
                  }
                }
                else
                  ConvertAlphaNew();
                last_tick = FreeRunMW();
              }
              else
                if (gKbdGDS.bInputLen < max_len)
                  gKbdGDS.sbInputBuf[gKbdGDS.bInputLen++] = (BYTE) keyin;
            UpdateDisplay();
          }
        continue;
      case MWKEY_ENTER:
      case MWKEY_SELECT:
        if ((gKbdGDS.bInputLen==0) && (gKbdGDS.dOption&NULL_ENB) && (min_len==max_len))
          return TRUE;
        //10-08-12 JC ++ fix bug in updating aOutBuf when user clear chars
        //if ((gKbdGDS.bInputLen == 0) && (min_len == 0) && (gKbdGDS.dEntryMode != AMOUNT_INPUT))
        //  return TRUE;
        //10-08-12 JC --
        if ((gKbdGDS.bInputLen >= min_len) && (gKbdGDS.bInputLen <= max_len)) {
          aOutBuf[0] = gKbdGDS.bInputLen;
          memcpy(&aOutBuf[1], gKbdGDS.sbInputBuf, gKbdGDS.bInputLen);
          aOutBuf[aOutBuf[0] + 1] = 0;
          return TRUE;
        }
        else if (gKbdGDS.bInputLen && (gKbdGDS.dOption & JUSTIFIED)) {
          memset(&aOutBuf[1],'0',max_len);
          memcpy(&aOutBuf[max_len + 1 - gKbdGDS.bInputLen], gKbdGDS.sbInputBuf, gKbdGDS.bInputLen);
          aOutBuf[0] = (BYTE) max_len;
          aOutBuf[aOutBuf[0] + 1] = 0;
          return TRUE;
        }
        else if (gKbdGDS.bInputLen && (min_len == 0)) {
          if (gKbdGDS.dEntryMode == AMOUNT_INPUT) {
            memset(&aOutBuf[1], '0', 12);
            memcpy(&aOutBuf[13 - gKbdGDS.bInputLen], &gKbdGDS.sbInputBuf, gKbdGDS.bInputLen);
            aOutBuf[0] = 12;
          }
          else {
            memset(&aOutBuf[1], ' ', max_len);
            memcpy(&aOutBuf[1], gKbdGDS.sbInputBuf, gKbdGDS.bInputLen);
            aOutBuf[0] = gKbdGDS.bInputLen;
          }
          aOutBuf[aOutBuf[0] + 1] = 0;
          return TRUE;
        }
        break;
      case 0x00:    //timeout of the waitkey          //31-10-26 JC ++
        return 0;
    }
    LongBeep();
  }
}
