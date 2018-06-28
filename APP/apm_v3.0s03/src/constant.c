//-----------------------------------------------------------------------------
//  File          : constant.c
//  Module        :
//  Description   : Constant data.
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
#include "sysutil.h"
#include "message.h"
#include "constant.h"

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------

const struct RSP_TEXT KRspText[] = {
  {'0'*256+'0',KComplted            },
  {'1'*256+'3',KBadAmount           },
  {'7'*256+'8',KTxnNotFound         },
  {'A'*256+'P',KTxnAccepted         },
  {'B'*256+'L',KLowBattery          },
  {'C'*256+'N',KTxnCanceled         },
  {'D'*256+'E',KDataError           },
  {'D'*256+'N',KNoDialTone          },
  {'E'*256+'C',KECRCommErr          },
  {'I'*256+'P',KBadProcCode         },
  {'I'*256+'R',KInvalidSeq          },
  {'I'*256+'S',KInvalidTrace        },
  {'I'*256+'T',KInvalidTerm         },
  {'H'*256+'O',KHostOffline         },
  {'H'*256+'N',KHostNotFound        },
  {'L'*256+'B',KLineBusy            },
  {'L'*256+'C',KTryAgainLC          },
  {'L'*256+'N',KNoLine              },
  {'L'*256+'O',KLineOccup           },
  {'M'*256+'A',KSwipeCard           },
  {'M'*256+'E',KWrongMAC            },
  {'N'*256+'C',KTryAgainNC          },
  {'N'*256+'E',KNotAllowed          },
  {'N'*256+'T',KNoTransRec          },
  {'P'*256+'F',KInvalidPWD          },
  {'R'*256+'E',KCardReadErr         },
  {'S'*256+'B',KCardBlocked         },
  {'S'*256+'C',KSwipeCard           },
  {'S'*256+'E',KSysError            },
  {'T'*256+'O',KTryAgainTO          },
  {'U'*256+'C',KUnSupportCard       },
  {'Z'*256+'1',KUpdateRequest       },
  {'*'*256+'*',KInvalidResp         },
};


const char *KTransHeader[] = {
  KEDCInit,
  KDnloadBIN,
  KDnloadSchedule,
};

const struct TRANS_BITMAP KTransBitmap[DLOAD_VBIN] = {
  {
    0x0800,{0x20,0x20,0x01,0x00,0x00,0x80,0x00,0x10},     /* 0 initialize */
    0x92
  },
};
