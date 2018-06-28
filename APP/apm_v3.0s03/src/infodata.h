//-----------------------------------------------------------------------------
//  File          : infodata.h
//  Module        :
//  Description   : Declaration & defines for infodata.c
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
#ifndef _INC_INFODATA_H_
#define _INC_INFODATA_H_
#include "common.h"

//----------------------------------------------------------------------------
//  Defines
//----------------------------------------------------------------------------
struct INFO_DATA
{
  BYTE  b_line_width;
  WORD  w_info_len;
  BYTE  *pb_header;
  BYTE  *pb_info;
};

//----------------------------------------------------------------------------
//  Generic API
//----------------------------------------------------------------------------
extern int PackInfoData(char *aInOut, char *aRData, int aWidth);
extern DWORD DispInfoData(struct INFO_DATA aInfoData);


#endif // _INC_INFODATA_H_
