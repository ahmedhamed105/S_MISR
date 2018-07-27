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
#ifndef _CONSTANT_H_
#define _CONSTANT_H_
#include "common.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------

/*-----  BATCH RECORD TRANS STATUS BIT ----------------------*/
#define OFFLINE         0x80
#define ADJUSTED_OFF    0x40
#define ADJUSTED        0x20
#define VOIDED          0x10

/*-----  TRANSACTION TYPES  ---------------------------------*/
enum {
  AUTH_SWIPE              ,   //0
  AUTH_MANUAL             ,   //1
  SALE_SWIPE              ,   //2
  SALE_MANUAL             ,   //3
  ONL_REFUND_SW           ,   //4
  ONL_REFUND_MNL          ,   //5
  REFUND                  ,   //6
  SALE_OFFLINE            ,   //7
  SALE_COMPLETE           ,   //8
  SALE_UNDER_LMT          ,   //9
  VOID                    ,   //10
  ADJUST                  ,   //11
  TRANS_UPLOAD            ,   //12
  SETTLEMENT              ,   //13
  EDC_REV                 ,   //14
  SALE_ICC                ,   //15
  AUTH_ICC                ,   //16
  REFUND_ICC              ,   //17
  ESALE_UNDER_LMT         ,   //18
  ESALE_COMPLETE          ,   //19
  SALE_CTL                ,   //20
  AUTH_CTL                ,   //21
  REFUND_CTL              ,   //22
  TEST_MSG                ,   //23
  SETT_ADV                ,   //24
  MAX_TXN_TYPE            ,
  DEFAULT_TRANS = 255     ,
};

struct TRANS_BITMAP {
  WORD w_txmsg_id;
  BYTE sb_txbitmap[16];
  BYTE b_proc_code;
  BYTE b_condition_code;
  BYTE b_type;
  BYTE b_sale_flag;  /* 0 not sale, 1 credit card sale, 2 debit sale, 3 redeem */
  BYTE b_status;
};

extern const struct TRANS_BITMAP KTransBitmap[MAX_TXN_TYPE];
extern const BYTE KSetlPCode2[3];
extern const BYTE KTransHeader[];

//-----------------------------------------------------------------------------
//      Functions Handle the data
//-----------------------------------------------------------------------------
extern BOOLEAN SaleType(BYTE aTrans);

#endif // _CONSTANT_H_

