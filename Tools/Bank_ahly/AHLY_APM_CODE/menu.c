//-----------------------------------------------------------------------------
//  File          : menu.c
//  Module        :
//  Description   : Generic menu handling routines.
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
#include "apm.h"
#include "util.h"
#include "sysutil.h"
#include "menu.h"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define MAX_PAGE_ITEM_COLOR            7
#define MAX_PAGE_ITEM_MONO             6

//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------

//*****************************************************************************
//  Function        : MenuSelect
//  Description     : Handle Menu Selection Operation.
//  Input           : aMenu;            // pointer to menu struct
//  Return          : selection index;  // -1 => No Selection
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int MenuSelect(const struct MENU_DAT *aMenu, int aCurrIndex)
{
  int i, item_idx, start_idx;
  int no_of_item, no_of_page, curr_page, items_per_page;
  DWORD keyin, line_no;
  BYTE disp_buf[MW_MAX_LINESIZE+1], tmpbuf[MW_MAX_LINESIZE+1];
  BOOLEAN manual_entry;
  DWORD max_line_size = (GetLcdTypeMW()==MW_DISP_COLOR)?MW_MAXCOLOR_LINESIZE:MW_MAXMONO_LINESIZE;
  DWORD max_line      = (GetLcdTypeMW()==MW_DISP_COLOR)?MW_LINE9:MW_LINE8;
  int  entry_sel=0;

  // Find Items Per Page
  line_no = MW_LINE2;
  items_per_page = (GetLcdTypeMW()==MW_DISP_COLOR)?MAX_PAGE_ITEM_COLOR:MAX_PAGE_ITEM_MONO;
  //items_per_page = 0;
  //while(line_no < max_line) {
  //  items_per_page++;
  //  line_no += MW_LINE3-MW_LINE2;
  //}

  // Find total item and total page
  for (no_of_item = 0; no_of_item <= MAX_MENU_ITEMS; no_of_item++) {
    if (aMenu->psMenuItem[no_of_item].iID == -1)
      break;
  }
  no_of_page = (no_of_item+items_per_page-1)/items_per_page;

  // Draw Frame & Header
  DispLineMW(aMenu->scHeader, MW_LINE1, MW_CLRDISP|MW_CENTER|MW_SPFONT);
  DispLineMW("+[", MW_LINE1, MW_LEFT|MW_SPFONT);
  DispLineMW("]+", MW_LINE1, MW_RIGHT|MW_SPFONT);
  memset(tmpbuf, ' ', sizeof(tmpbuf));
  tmpbuf[max_line_size-2] = 0;
  sprintf(disp_buf, "|%s|", tmpbuf);
  for (line_no=MW_LINE2; line_no < max_line; line_no+=(MW_LINE2-MW_LINE1)) {
    DispLineMW(disp_buf, line_no, MW_CENTER|MW_SPFONT);
  }
  memset(tmpbuf, '-', sizeof(tmpbuf));
  tmpbuf[max_line_size-2] = 0;
  sprintf(disp_buf, "+%s+", tmpbuf);
  DispLineMW(disp_buf, max_line, MW_CENTER|MW_SPFONT);

  // Assign current selection
  curr_page = aCurrIndex / items_per_page;
  start_idx = curr_page * items_per_page;
  entry_sel = aCurrIndex;

  manual_entry = FALSE;
  while (1) {
    // Show Menu Item
    item_idx = start_idx;
    for (line_no=MW_LINE2; line_no < max_line; line_no+=(MW_LINE2-MW_LINE1)) {
      if (aMenu->psMenuItem[item_idx].iID == -1)  break;
      memset(tmpbuf, ' ', sizeof(tmpbuf));
      if (aMenu->psMenuItem[item_idx].pcDesc == NULL)
        memcpy(tmpbuf, aMenu->psMenuItem[item_idx].scDescDat, sizeof(aMenu->psMenuItem[item_idx].scDescDat));
      else
        memcpy(tmpbuf, aMenu->psMenuItem[item_idx].pcDesc, strlen(aMenu->psMenuItem[item_idx].pcDesc));
      tmpbuf[max_line_size-2-3] = 0;  // max desc = max size - border - (index + .)
      sprintf(disp_buf, "%02d.%s", aMenu->psMenuItem[item_idx].iID, tmpbuf);
      if (item_idx == aCurrIndex) {
        DispLineMW(disp_buf, line_no+1, MW_REVERSE|MW_SPFONT);
      }
      else {
        DispLineMW(disp_buf, line_no+1, MW_SPFONT);
      }
      item_idx++;
    }
    // Clear the rest line
    memset(disp_buf, ' ', sizeof(disp_buf));
    disp_buf[max_line_size-2] = 0;
    for (; line_no < max_line; line_no+=(MW_LINE2-MW_LINE1)) {
      DispLineMW(disp_buf, line_no+1, MW_SPFONT);
    }

    // Show Trailer line
    if (curr_page == 0) {
      DispLineMW("+---", max_line, MW_SPFONT);
    }
    else {
      DispLineMW("+<<<", max_line, MW_SPFONT);
    }

    sprintf(disp_buf, "[%02d] %02x/%02x", manual_entry?entry_sel:aMenu->psMenuItem[aCurrIndex].iID, curr_page+1, no_of_page);
    DispLineMW(disp_buf, max_line+(max_line_size-strlen(disp_buf))/2, MW_SPFONT);

    if (curr_page == no_of_page-1) {
      DispLineMW("---+", max_line, MW_RIGHT|MW_SPFONT);
    }
    else {
      DispLineMW(">>>+", max_line, MW_RIGHT|MW_SPFONT);
    }

    keyin = APM_WaitKey(KBD_TIMEOUT, 0);
    if (keyin == MWKEY_CANCL) {
      return -1;
    }

    // Selected
    if ((keyin == MWKEY_ENTER) || (keyin == MWKEY_SELECT)) {
      break;
    }

    // id entered.
    if ((keyin >= MWKEY_0) && (keyin <= MWKEY_9)) {
      if (manual_entry == FALSE) {
        entry_sel = 0;
        manual_entry = TRUE;
      }
    }
    else
      manual_entry = FALSE;

    switch (keyin) {
      case MWKEY_RIGHT:
      case MWKEY_FUNC1:
        if (curr_page < no_of_page-1) {
          curr_page++;
          start_idx += items_per_page;
          aCurrIndex = start_idx;
          break;
        }
        LongBeep();
        break;
      case MWKEY_LEFT:
      case MWKEY_FUNC2:
        if (curr_page > 0) {
          curr_page--;
          start_idx -= items_per_page;
          aCurrIndex = start_idx;
          break;
        }
        LongBeep();
        break;
      case MWKEY_UP:
      case MWKEY_FUNC3:
        if ((aCurrIndex > 0)) {
          aCurrIndex--;
          if (aCurrIndex < start_idx) {
            curr_page--;
            start_idx -= items_per_page;
          }
          break;
        }
        LongBeep();
        break;
      case MWKEY_DN:
      case MWKEY_FUNC4:
        if ((aCurrIndex < no_of_item-1)) {
          aCurrIndex++;
          if (aCurrIndex >= start_idx+items_per_page) {
            start_idx += items_per_page;
            curr_page++;
          }
          break;
        }
        LongBeep();
        break;
      case MWKEY_0:
      case MWKEY_1:
      case MWKEY_2:
      case MWKEY_3:
      case MWKEY_4:
      case MWKEY_5:
      case MWKEY_6:
      case MWKEY_7:
      case MWKEY_8:
      case MWKEY_9:
        entry_sel *= 10;
        entry_sel += keyin - MWKEY_0;
        entry_sel %= 100;
        for (i = 0; i < no_of_item; i++) {
          if (entry_sel == aMenu->psMenuItem[i].iID) {
            aCurrIndex = i;
            curr_page = (i / items_per_page);
            start_idx = curr_page * items_per_page;
            break;
          }
          if (entry_sel < aMenu->psMenuItem[i].iID)
            break;
        }
        break;
      case MWKEY_CLR:
        aCurrIndex = 0;
        curr_page = 0;
        start_idx = 0;
        LongBeep();
        break;
      default:
        LongBeep();
        break;
    }
    SleepMW();
  }

  return aMenu->psMenuItem[aCurrIndex].iID;
}
