//-----------------------------------------------------------------------------
//  File          : brdrtest.h
//  Module        :
//  Description   : BarCode Reader Test.
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
//  05 Mar  2013  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_BRDRTEST_H_
#define _INC_BRDRTEST_H_
#include <string.h>
#include "common.h"
#include "system.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define MAX_PROD_NAME       7
#define MAX_PROD_STR_LEN    30

enum
{
  BRD_RES_OK      ,
  BRD_RES_NO_DEV  ,
  BRD_RES_EN_ERR  ,
  BRD_RES_FAIL    ,
  BRD_RES_TO      ,
  BRD_RES_CANCL   ,
};

extern BYTE gProdName[MAX_PROD_NAME][MAX_PROD_STR_LEN+1];    // product names
extern BYTE gProdCnt;                                        // product count

//-----------------------------------------------------------------------------
//      Function API
//-----------------------------------------------------------------------------
extern void BRdrTest(void);
extern BYTE BRdrGetString(BYTE *Str, DWORD *aLen);
extern BOOLEAN BRdrGetAmount(void);

#endif // _INC_BRDRTEST_H_
