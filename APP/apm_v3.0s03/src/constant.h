//-----------------------------------------------------------------------------
//  File          : constant.h
//  Module        :
//  Description   : Declrartion & Defination for constant.c
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
#ifndef _INC_CONSTANT_H_
#define _INC_CONSTANT_H_
#include "common.h"


//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
struct RSP_TEXT {
  WORD  w_idx;
  const char *pc_text;
};
extern const struct RSP_TEXT KRspText[];

// Transaction Support
#define EDC_INIT        0
#define DLOAD_VBIN      1
#define LOAD_SCHEDULE   2

struct TRANS_BITMAP {
  WORD w_txmsg_id;
  BYTE sb_txbitmap[8];
  BYTE b_proc_code;
};
extern const struct TRANS_BITMAP KTransBitmap[];

extern const char *KTransHeader[];

#endif // _INC_CONSTANT_H_

