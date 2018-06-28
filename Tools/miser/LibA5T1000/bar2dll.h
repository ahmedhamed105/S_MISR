//-----------------------------------------------------------------------------
//  File          : Bar2dll.h
//  Module        : 
//  Description   : Declrartion & Defination for BARDLL export functions.
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
//  24 Nov  2016  Lewis       Initial Version.
//  09 Jan  2017  Lewis       ID change to 0x0D;
//-----------------------------------------------------------------------------
#ifndef _INC_BARDLL_H_
#define _INC_BARDLL_H_
#include "common.h"
//#include "appid.h"

//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#ifndef BARDLL_ID
#define BARDLL_ID          0x0D
#endif

// Command ID
enum {
BAR_DECODE1D,
BAR_DECODE2D,
BAR_ENCODE,
BAR_ZXDECODE1D,
BAR_DECODE1D_ENH,
BAR_ZXDECODE1D_ENH,
BAR_1D_TRYHARD,
BARDLL_FUNC_COUNT
};


//-----------------------------------------------------------------------------
//   Defines
//-----------------------------------------------------------------------------
//symbology support
enum {
  BAR_NONE,
  BAR_CODE39,
  BAR_CODE93,
  BAR_CODE128,
  BAR_I25,
  BAR_CODABAR,
  BAR_DATABAR,
  BAR_UPCA,
  BAR_UPCE,
  BAR_EANX,
  BAR_ISBNX,
  BAR_QRCODE,
  BAR_DATAMATRIX,
  BAR_MAX_SUPPORT,
};

//input_mode support
enum {
  MODE_ASCII,
  MODE_UNICODE,
};

typedef struct BAR_DAT {
  DWORD type;
  WORD  width;
  WORD  height;
  BYTE  *dat;
} T_BAR_DAT;

typedef struct BAR_IN {
  int symbology;            // barcode type
  int vzoom;                // 1D barcode, default 1 pixel
  int scale;                // default 1;
  int quite_zone_x;         // quite zone in module, default 0
  int quite_zone_y;         // 
  int input_mode;           // input data mode, default ASCII_MODE
  DWORD option1;            // QRCode:ECC level [1-4]
  DWORD option2;            // QRCode:version [1-40]
  BYTE  *dat; 
  DWORD dat_len;
} T_BARIN_DAT;

typedef struct BAR_OUT {
  BYTE   *dat;
  DWORD  max_dat_len;
  BYTE   *type_str;
  DWORD  max_type_len;
} T_BAROUT_DAT;

//-----------------------------------------------------------------------------
//   API
//-----------------------------------------------------------------------------
//int BAR_1DTryHard(int aTrue);
#define BAR_1DTryHard(x)           (int)lib_app_call(BARDLL_ID, BAR_1D_TRYHARD, (DWORD)x, 0, 0)

//int BAR_Decode1D(T_BAROUT_DAT *aOut, T_BAR_DAT *aInDat);
#define BAR_Decode1D(x,y)          (int)lib_app_call(BARDLL_ID, BAR_DECODE1D, (DWORD)x, (DWORD)y, 0)

//int BAR_ZXDecode1D(T_BAROUT_DAT *aOut, T_BAR_DAT *aInDat);
#define BAR_ZXDecode1D(x,y)        (int)lib_app_call(BARDLL_ID, BAR_ZXDECODE1D, (DWORD)x, (DWORD)y, 0)

//int BAR_Decode1DEnh(T_BAROUT_DAT *aOut, T_BAR_DAT *aInDat);
#define BAR_Decode1DEnh(x,y)       (int)lib_app_call(BARDLL_ID, BAR_DECODE1D_ENH, (DWORD)x, (DWORD)y, 0)

//int BAR_ZXDecode1D(T_BAROUT_DAT *aOut, T_BAR_DAT *aInDat);
#define BAR_ZXDecode1DEnh(x,y)     (int)lib_app_call(BARDLL_ID, BAR_ZXDECODE1D_ENH, (DWORD)x, (DWORD)y, 0)

//int BAR_Decode2D(T_BAROUT_DAT *aOut, T_BAR_DAT *aInDat, DWORD aTimeoutSec);  // aTimeourSec == 0 => disable DM
#define BAR_Decode2D(x,y,z)     (int)lib_app_call(BARDLL_ID, BAR_DECODE2D, (DWORD)x, (DWORD)y, (DWORD)z)

//int BAR_Encode(T_BAR_DAT *aBarDat, T_BARIN_DAT *aBarIn)
#define BAR_Encode(x,y)       (int)lib_app_call(BARDLL_ID, BAR_ENCODE, (DWORD)x, (DWORD)y, 0)

#endif // _INC_BARDLL_H_
