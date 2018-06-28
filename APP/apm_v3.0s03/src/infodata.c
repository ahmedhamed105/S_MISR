//-----------------------------------------------------------------------------
//  File          : infodata.c
//  Module        :
//  Description   : Generic infodata handling routines.
//  Author        : Pody
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
//  Jan 2008      Pody        Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include "hwdef.h"
#include "midware.h"
#include "util.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "infodata.h"

//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------


//*****************************************************************************
//  Function        : PackInfoData
//  Description     : pack info data fix to line width
//  Input           : aInOut    - left data and output string
//                    aRData    - right adjust data string
//                    aWidth    - line width
//  Return          : output string length
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int PackInfoData(char *aInOut, char *aRData, int aWidth)
{
  WORD len, data_len, pack_len;

  len = strlen(aInOut);
  data_len = strlen(aRData);
  pack_len = (aWidth - ((len + data_len) % aWidth)) % aWidth;

  while (pack_len > 0) {
    aInOut[len++] = ' ';
    pack_len--;
  }

  strcpy(aInOut+len, aRData);
  return (len + data_len);
}
//*****************************************************************************
//  Function        : DispInfoData
//  Description     : display information data page by page
//  Input           : aInfoData   // infodata to display
//                    aBigFont    // 4 or 8 lines per page
//  Return          : last input key
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD DispInfoData(struct INFO_DATA aInfoData)
{
  WORD line_per_page, char_per_page, cur_page, page_len;
  WORD tot_line, tot_page;
  DWORD keyin;
  BYTE header[MW_MAX_LINESIZE +1];

  line_per_page = 7;
  cur_page = 0;
  char_per_page = line_per_page * (WORD)aInfoData.b_line_width;
  tot_line = (aInfoData.w_info_len + aInfoData.b_line_width - 1) / aInfoData.b_line_width;
  tot_page = (tot_line + line_per_page - 1) / line_per_page;

  // copy header
  memset(header, ' ', sizeof(header));
  memcpy(header, aInfoData.pb_header, strlen(aInfoData.pb_header));

  while (1) {
    // show header
    SprintfMW(header+aInfoData.b_line_width-5, "%02d/%02d", cur_page+1, tot_page);
    DispLineMW(header, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_CLREOL|MW_SPFONT);

    // show info data
    if (cur_page+1 == tot_page)
      page_len = aInfoData.w_info_len - (cur_page * char_per_page);
    else
      page_len = char_per_page;
    DispGotoMW(MW_LINE2, MW_SPFONT); 
    DispPutNCMW(aInfoData.pb_info+cur_page*char_per_page, page_len);
    

    // keybd input handle
    keyin = WaitKey(KBD_TIMEOUT);
    switch (keyin) {
      case MWKEY_CANCL:
        return keyin;
      case MWKEY_ENTER:
      case MWKEY_DN:
      case MWKEY_SELECT:
        if (cur_page < tot_page-1)
          cur_page++;
        else
          return keyin;
        break;
      case MWKEY_UP:
      case MWKEY_CLR:
        if (cur_page > 0)
          cur_page--;
        break;
      case MWKEY_LEFT:
        cur_page = 0;
        break;
      case MWKEY_RIGHT:
        cur_page = tot_page-1;
        break;
      default:
        break;
    }
  }
}
