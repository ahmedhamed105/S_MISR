//-----------------------------------------------------------------------------
//  File          : inittran.h
//  Module        :
//  Description   : Declrartion & Defination for inittran.c
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
#ifndef _INC_FTPTRAN_H_
#define _INC_FTPTRAN_H_
#include "common.h"
#include "remotedl.h"

//-----------------------------------------------------------------------------
//   Defines
//-----------------------------------------------------------------------------
enum {
  PARAM_INFO_EDC=0      ,
  PARAM_INFO_EMVAPP     ,
  PARAM_INFO_EMVKEY     ,
  PARAM_INFO_XAPP       ,
  PARAM_INFO_LXAPP      ,
  NUM_PARAM_INFO        ,
};

struct PARAM_INFO {
  BYTE  chksum[4];
  BYTE  date[8];
};

//-----------------------------------------------------------------------------
//   Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Functions
//-----------------------------------------------------------------------------
extern DWORD STISFTPDownloadTrans(BOOLEAN aManual, BYTE aDownloadMode, BOOLEAN aBurnInRoomMode);
extern void STISFTPPolling(struct REMOTEDL_DATA *aRemoteDl);

#endif //_INC_INITTRAN_H_
