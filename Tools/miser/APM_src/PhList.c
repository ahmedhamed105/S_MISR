//-----------------------------------------------------------------------------
//  File          : phlist.c
//  Module        :
//  Description   : Phone List Routine
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
//-----------------------------------------------------------------------------
#if (R700)
//-----------------------------------------------------------------------------
#include <string.h>
#include "midware.h"
#include "util.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "phlist.h"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define ITEM_PER_PAGE   6

//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------
static const char KFrame[] = { "\x12\f+[\x1B\x13]+|\x1B\x34||\x1B\x54||\x1B\x74|"\
                               "|\x1B\x94||\x1B\xB4||\x1B\xD4|+\x1B\xF4+"
                             };

static const BYTE KMenuHeight[] = {  3,  6 };
static const BYTE KMenuWidth[]  = { 14, 19 };
static const BYTE KTrailer[] = { "+---    XX/XX    ---+" };

//*****************************************************************************
//  Function        : ListSelect
//  Description     : Handle List Selection Operation.
//  Input           : aMenu;            // pointer to menu struct
//  Return          : selection index;  // -1 => No Selection
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int ListSelect(const struct LIST_DAT *aList)
{
  DWORD i, item_idx, start_idx;
  DWORD no_of_page, curr_item, curr_page;
  DWORD keyin;
  BYTE disp_buf[21];

  if (aList->dwListCount == 0)
    return -1;

  no_of_page = (aList->dwListCount+5)/6;   // 6 item per page

  // Draw Frame & Header
  DispPutNCMW(KFrame, sizeof(KFrame));
  DispPutNCMW("\x1B\x02", 2);
  DispPutNCMW(aList->sbHeader, sizeof(aList->sbHeader));

  curr_page = 1;
  curr_item = 0;
  start_idx = 0;

  while (1) {
    DispPutNCMW("\x12\x1B\x21", 3);
    // Show Menu Item
    for (i = 0; i < ITEM_PER_PAGE; i++) {
      item_idx = i + start_idx;
      memset(disp_buf, ' ', sizeof(disp_buf));
      if (item_idx < aList->dwListCount) {
        memcpy(disp_buf, aList->psListItem[item_idx].sbDesc, sizeof(disp_buf));
      }
      if (item_idx == curr_item)
        DispPutCMW(MW_REV_ON);
      DispPutNCMW(disp_buf, 19);
      if (item_idx == curr_item)
        DispPutCMW(MW_REV_OFF);
      DispPutCMW('\n');
      DispPutCMW(K_CurRight);
    }

    // Show Trailer line
    memcpy(disp_buf, KTrailer, sizeof(disp_buf));
    if (curr_page > 1)          memcpy(disp_buf, "<<<-", 4);
    if (curr_page < no_of_page) memcpy(&disp_buf[16], "->>>", 4);
    DispPutNCMW("\x12\x1B\xE0", 3);
    bin2dec(curr_item,  &disp_buf[8], 2);
    bin2dec(aList->dwListCount, &disp_buf[11], 2);
    DispPutNCMW(disp_buf, sizeof(disp_buf));

    keyin = WaitKey(KBD_TIMEOUT);
    if (keyin == MWKEY_CANCL) {
      return -1;
    }

    // Selected
    if (keyin == MWKEY_ENTER) {
      return curr_item;
    }

    switch (keyin) {
      case MWKEY_SHARP:
        if (curr_page < no_of_page) {
          curr_page++;
          start_idx += ITEM_PER_PAGE;
          curr_item = start_idx;
          break;
        }
        LongBeep();
        break;
      case MWKEY_ASTERISK:
        if (curr_page > 1) {
          curr_page--;
          start_idx -= ITEM_PER_PAGE;
          curr_item = start_idx;
          break;
        }
        LongBeep();
        break;
      case MWKEY_8:
        if ((curr_item > 0)) {
          curr_item--;
          if (curr_item < start_idx) {
            curr_page--;
            start_idx -= ITEM_PER_PAGE;
          }
          break;
        }
        LongBeep();
        break;
      case MWKEY_0:
        if ((curr_item < aList->dwListCount-1)) {
          curr_item++;
          if (curr_item >= start_idx+ITEM_PER_PAGE) {
            start_idx += ITEM_PER_PAGE;
            curr_page++;
          }
          break;
        }
        LongBeep();
        break;
      default:
        LongBeep();
        break;
    }
    SleepMW();
  }
}
//-----------------------------------------------------------------------------
#endif // (R700)
//-----------------------------------------------------------------------------
