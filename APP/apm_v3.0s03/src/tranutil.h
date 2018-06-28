//-----------------------------------------------------------------------------
//  File          : Tranutil.h
//  Module        :
//  Description   : Declrartion & Defination for tranutil.c
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
#ifndef _INC_TRANUTIL_H_
#define _INC_TRANUTIL_H_
#include "common.h"

//-----------------------------------------------------------------------------
//   Defines
//-----------------------------------------------------------------------------
// STIS init mode
#define MODE_MENU_SELECT       0x00
#define MODE_ALL_PARAM         0x01
#define MODE_EMV_PARAM_ONLY    0x02
#define MODE_EMV_KEY_ONLY      0x03
#define MODE_EMV_PARAM_KEY     0x04
#define MODE_EXTRA_PARAM       0x05
#define MODE_LONG_PARAM        0x06
#define MODE_APPLICATION       0x07
#define MODE_UPLOAD            0x08
#define MODE_VBIN_ONLY         0xFE
#define MODE_ALL_PARAM_APP     0xFF

struct INPUT_DATA {
  BYTE b_trans;
  BYTE b_stis_init_mode;
  BYTE b_proc_code2;
  BYTE sb_nii[2];
  BYTE sb_trace_no[3];
  BYTE sb_term_id[8];
};

struct RSP_DATA {
  BYTE b_trans;
  BYTE sb_trace_no[3];
  BYTE b_year;
  BYTE sb_date[2];
  BYTE sb_time[3];
  BYTE sb_auth_code[6];
  BYTE sb_rrn[12];
  BYTE sb_text[70];
  WORD w_rspcode;
};

#define TRANS_ACP       0
#define TRANS_REJ       1
#define TRANS_FAIL      2
#define LINE_FAIL       3
#define MORE_RSP        4

struct TRANS_DATA {
  BYTE              b_response;
  struct INPUT_DATA s_input;
  struct RSP_DATA   s_rsp_data;
};

//-----------------------------------------------------------------------------
//   Globals Variables
//-----------------------------------------------------------------------------
extern struct TRANS_DATA  gsTransData;

//-----------------------------------------------------------------------------
//   Functions
//-----------------------------------------------------------------------------
extern void ResetTrans(void);
extern void ClearResponse(void);
extern void SetRspCode(WORD aRspCode, BOOLEAN aPackText);
extern void DispHeader(void);
extern void DispRspText(BOOLEAN aDispHdr);
extern void TransEnd(void);
extern BOOLEAN Return2Base(void);

#endif // _INC_TRANUTIL_H_

