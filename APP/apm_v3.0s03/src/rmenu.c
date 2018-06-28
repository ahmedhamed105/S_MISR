//-----------------------------------------------------------------------------
//  File          : rmenu.c
//  Module        :
//  Description   : Right ATM Key Menu Selection function.
//  Author        : Lewis
//  Notes         : N/A
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
#include <string.h>
#include "hwdef.h"
#include "midware.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "rmenu.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
//
//*****************************************************************************
//  Function        : RMenuSel
//  Description     : Prompt for menu selection.
//  Input           : aTbl;     // pointer to RMENUDAT
//                    aIcon;    // Icon Data (106x106) pixel.
//                    aIconDesc;// message under icon(user control)
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int RMenuSel(struct RMENUDAT *aTbl, BYTE *aIcon, BYTE *aIconDesc)
{
  #if (CLCD_SUPPORT)
  struct  GLOC loc;
  #endif
  DWORD   start_idx, old_idx, keyin, i, len, max_len, max_item, curr_idx;
  BYTE    tmp[MW_MAX_LINESIZE+1];
  DWORD   curr_sel, line_no;
  int     select = -1;

  DispCtrlMW(MW_CLR_DISP);
  #if (CLCD_SUPPORT)
  max_len = 20;
  if (aIcon != NULL) {
    loc.wPosX = 18;
    loc.wPosY = 10;
    loc.wWidth = aIcon[5]*256+aIcon[4];
    loc.wHeight= aIcon[7]*256+aIcon[6];
    ShowGraphicT800(&loc, &aIcon[8]);
    if (aIconDesc!=NULL)
      DispPutStr(aIconDesc);
    max_len = 10;
  }
  #elif (!CLCD_SUPPORT)
  if (aIcon != NULL) {
    ShowBWLogo(aIcon, 536);
  }
  max_len = 10;
  #endif

  // find max menu item
  max_item = 0;
  while (aTbl[max_item].iID != -1)  max_item++;

  curr_sel = start_idx = curr_idx = 0;
  old_idx   = -1;
  while (1) {
    // Update Idle Display
    if (start_idx != old_idx) {
      // Show Application List
      for (i = 0; i < 4; i++) {
        memset(tmp, ' ', max_len);
        if ((start_idx + i) < max_item) {
          curr_idx = start_idx + i;
          len = strlen(aTbl[curr_idx].sbName);
          if (len > max_len)
            len = max_len;
          memcpy(&tmp[max_len-len], aTbl[curr_idx].sbName, len);
        }
        tmp[max_len] = 0;
        line_no = MW_LINE1+i*(MW_LINE3-MW_LINE1);
        DispLineMW(tmp, line_no, MW_RIGHT|MW_BIGFONT);
        if (((start_idx + i) < max_item)&&(curr_sel == i))  {
          if (line_no != MW_LINE7)
            line_no += MW_LINE2-MW_LINE1;
          DispLineMW("*", line_no, MW_RIGHT|MW_SMFONT);
        }
        #if (T800)        
        sprintf(tmp, "[%d/%d] ", (start_idx/4+1), (max_item-1)/4+1);
        DispLineMW(tmp, MW_MAX_LINE, MW_RIGHT|MW_SMFONT);
        #endif        
      }
      if (start_idx != 0) {  // Has previous page
        DispLineMW("+", MW_LINE1, MW_RIGHT|MW_SMFONT);
      }
      if ((curr_idx+1) < max_item) {  // Has Next Page
        DispLineMW("+", MW_MAX_LINE, MW_RIGHT|MW_SMFONT);
      }
      old_idx = start_idx;
    }

    keyin = GetCharMW();
    switch (keyin) {
      case MWKEY_CLR:
        start_idx = 0;
        break;
      case MWKEY_ENTER:
        select = aTbl[start_idx+curr_sel].iID;
        break;
      case MWKEY_RIGHT1:
        select = aTbl[start_idx].iID;
        break;
      case MWKEY_RIGHT2:
        if ((start_idx + 1) < max_item)
          select = aTbl[start_idx+1].iID;
        break;
      case MWKEY_RIGHT3:
        if ((start_idx + 2) < max_item)
          select = aTbl[start_idx+2].iID;
        break;
      case MWKEY_RIGHT4:
        if ((start_idx + 3) < max_item)
          select = aTbl[start_idx+3].iID;
        break;
      case MWKEY_POWER:
        select = RMNU_POWEROFF;
        break;
      case MWKEY_FUNC1:  // Next Page
      case MWKEY_RIGHT:  // Next Page
        if (aTbl[curr_idx+1].iID != -1) {
          start_idx += 4;
          curr_sel = 0;
          old_idx = -1; //refresh display
        }
        break;
      case MWKEY_FUNC2:  // Prev Page
      case MWKEY_LEFT:  // Next Page
        if (start_idx >= 4) {
          start_idx -= 4;
          curr_sel = 3;
          old_idx = -1; //refresh display
        }
        break;
      case MWKEY_FUNC4:  // Next Sel
      case MWKEY_DN:
        if ((curr_sel < 3) && ((start_idx + curr_sel) < max_item-1)) {
          curr_sel += 1;
          old_idx = -1; //refresh display
        }
        break;
      case MWKEY_FUNC3:  // Prev Sel
      case MWKEY_UP:
        if (curr_sel > 0) {
          curr_sel -= 1;
          old_idx = -1; //refresh display
        }
        break;
    }
    if ((select >= 0) || (keyin == MWKEY_CANCL))
      break;
  }
  return select;
}
