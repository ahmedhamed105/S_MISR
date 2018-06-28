//-----------------------------------------------------------------------------
//  File          : constant.c
//  Module        :
//  Description   : Include Global constants & related routines.
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
#include "message.h"
#include "constant.h"

//-----------------------------------------------------------------------------
//    Constants
//-----------------------------------------------------------------------------
const struct TRANS_BITMAP KTransBitmap[MAX_TXN_TYPE] = {
  {
    0x0200, {0x70, 0x38, 0x05, 0x80, 0x2e, 0xc0, 0x02, 0x04},  /* 0 swiped auth ahmed*/
    0x00, 0x00, AUTH_SWIPE, 0x1, 0x00
  },

  {0x0100, {0x70, 0x24, 0x05, 0x80, 0x00, 0xC0, 0x02, 0x04},  /* 1 manual auth */
   0x00, 0x00,  AUTH_MANUAL, 0x1, 0x00
  },

  {0x0200, {0x70 , 0x38 , 0x05 , 0x80 , 0x2e , 0xc0 , 0x02 , 0x04},  /* 2 swiped sale ahmed*/
   0x00, 0x00,  SALE_SWIPE, 0x1, 0x00
  },

  {0x0200, {0x70, 0x24, 0x05, 0x80, 0x00, 0xC0, 0x06, 0x0C},  /* 3 manual sale */
   0x00, 0x00,  SALE_MANUAL, 0x1, 0x00
  },

  {0x0200, {0x70 , 0x38 , 0x05 , 0x80 , 0x2e , 0xc0 , 0x02 , 0x04},  /* 4 online refund swipe ahmed*/
   0x20, 0x00, ONL_REFUND_SW, 0x0, 0x00
  },

  {0x0200, {0x70, 0x24, 0x05, 0x80, 0x00, 0xC0, 0x02, 0x0C},  /* 5 online refund manual */
   0x20, 0x00, ONL_REFUND_MNL, 0x0, 0x00
  },

  {0x0220, {0x70 , 0x20 , 0x05 , 0x80 , 0x20 , 0xc0 , 0x02 , 0x04},  /* 6 offline refund ahmed*/
   0x20, 0x00, REFUND, 0x0, 0x80
  },

  {0x0220, {0x70 , 0x38 , 0x05 , 0x80 , 0x2e , 0xc0 , 0x00 , 0x04},  /* 7 offline sale ahmed*/
   0x00, 0x00, SALE_OFFLINE, 0x1, 0x80
  },

  {0x0220, {0x70, 0x3C, 0x05, 0x80, 0x04, 0xD0, 0x04, 0x0C},  /* 8 sale completion */
   0x00, 0x06, SALE_COMPLETE, 0x1, 0x80
  },

  {0x0220, {0x70, 0x3C, 0x05, 0x80, 0x04, 0xD0, 0x04, 0x0C},  /* 9 Under Floor Limit Sale */
   0x00, 0x00, SALE_UNDER_LMT, 0x1, 0x80
  },

  {0x0200, {0x70 , 0x38 , 0x05 , 0x80 , 0x2e , 0xc0 , 0x02 , 0x14},  /* 10 online void ahmed*/
   0x02, 0x00, VOID, 0x0, 0x00
  },

  {0x0220, {0x70 , 0x38 , 0x05 , 0x80 , 0x2c , 0xc0 , 0x02 , 0x14},  /* 11 adjust */
   0x02, 0x00, ADJUST, 0x0, 0x00
  },

  {0x0320, {0x30 , 0x3c , 0x01 , 0x80 , 0x0e , 0xc0 , 0x00 , 0x12},  /* 12 transaction upload ahmed*/
   0x00, 0x00, TRANS_UPLOAD, 0x0,  0x00
  },

  {0x0500, {0x20 , 0x38 , 0x01 , 0x00 , 0x0a , 0xc0 , 0x00 , 0x12},  /* 13 settlement - close batch ahmed*/
   0x92, 0x00, SETTLEMENT, 0x0, 0x00
  },

  {0x0400, {0x70, 0x24, 0x05, 0x80, 0x00, 0xC0, 0x04, 0x04},  /* 14 reversal : auth-00A000  */
   0x00, 0x00, EDC_REV, 0x0, 0x00
  },

  {0x0200, {0x30, 0x20, 0x05, 0x80, 0x20, 0xC0, 0x06, 0x0C},  /* 15 AEIPS Sale */
   0x00, 0x00, SALE_ICC, 0x1, 0x00
  },

  {0x0100, {0x30, 0x20, 0x05, 0x80, 0x20, 0xC0, 0x06, 0x04},   /* 16 AEIPS Auth */
   0x00, 0x00, AUTH_ICC, 0x1, 0x00
  },

  {0x0220, {0x70, 0x3C, 0x05, 0x80, 0x04, 0xD0, 0x06, 0x0C},   /* 17 AEIPS Under Floor Limit Sale */
   0x00, 0x00, ESALE_UNDER_LMT, 0x1, 0x80
  },

  {0x0220, {0x70, 0x3C, 0x05, 0x80, 0x04, 0xD0, 0x06, 0x0C},   /* 18 AEIPS Sale completion */
   0x00, 0x06, ESALE_COMPLETE, 0x1, 0x80
  },

  {0x0200, {0x30, 0x20, 0x05, 0x80, 0x20, 0xC0, 0x06, 0x0C},  /* 15 AEIPS Sale */
   0x00, 0x00, SALE_CTL, 0x1, 0x00
  },

  {0x0800, {0x20, 0x20, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00},   /* 19 test trans */
   0x99, 0x00, TEST_MSG, 0x00, 0x00
  },
 
  {0x0500, {0x20, 0x38, 0x01, 0x00, 0x0a, 0xc0, 0x00, 0x12},   /* 19 INST 6 20 38 01 00 0a c0 00 12 */
   0x99, 0x00, INST_six, 0x00, 0x00
  },
  

  {0x0500, {0x20, 0x20, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00},   /* 19 INST 9 */
   0x99, 0x00, INST_nin, 0x00, 0x00
  },
  

  {0x0500, {0x20, 0x20, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00},   /* 19 INST 10 */
   0x99, 0x00, INST_ten, 0x00, 0x00
  },
  

  {0x0500, {0x20, 0x20, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00},   /* 19 INST 12 */
   0x99, 0x00, INST_twe, 0x00, 0x00
  },

  {0x0400, {0x70, 0x24, 0x05, 0x80, 0x00, 0xC0, 0x04, 0x04},   /* 19 Ref */
   0x99, 0x00, rev_ICC, 0x00, 0x00
  },
};


const BYTE KSetlPCode2[3] = {0x96,0,0};

const BYTE KTransHeader[] = {
  EDC_CN_AUTH              ,   //0
  EDC_CN_AUTH              ,   //1
  EDC_CN_SALE              ,   //2
  EDC_CN_SALE              ,   //3
  EDC_CN_REFUND            ,   //4
  EDC_CN_REFUND            ,   //5
  EDC_CN_REFUND            ,   //6
  EDC_CN_OFFLINE           ,   //7
  EDC_CN_SALE_COMP         ,   //8
  EDC_CN_SALE              ,   //9
  EDC_CN_VOID              ,   //10
  EDC_CN_ADJUST            ,   //11
  EDC_CN_UPLOAD            ,   //12
  EDC_CN_SETTLE            ,   //13
  EDC_CN_REVERSAL          ,   //14
  EDC_CN_SALE              ,   //15
  EDC_CN_AUTH              ,   //16
  EDC_CN_REFUND            ,   //17
  EDC_CN_SALE              ,   //18
  EDC_CN_SALE_COMP         ,   //19
  EDC_CN_SALE              ,   //20
  EDC_CN_AUTH              ,   //21
  EDC_CN_REFUND            ,   //22
  EDC_CN_TEST_MSG          ,   //23
};

//*****************************************************************************
//  Function        : SaleType
//  Description     : Return TRUE is transaction is define as sale type.
//  Input           : trans;  // transaction id.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SaleType(BYTE aTrans)
{
  return (KTransBitmap[aTrans].b_sale_flag != 0);
}
