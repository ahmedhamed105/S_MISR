//-----------------------------------------------------------------------------
//  File          : kbdutil.h
//  Module        :
//  Description   : Declrartion & Defination for KBDUTIL.C
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_KDBUTIL_H_
#define _INC_KDBUTIL_H_
#include "common.h"
#include "apm.h"

//-----------------------------------------------------------------------------
//  Common Defines
//-----------------------------------------------------------------------------
//// Items Move to apm.h
//// Define time out value
//#define TIME_1SEC       100
//#define KBD_TIMEOUT     (90*TIME_1SEC)      // 90 sec

extern void SetKbdTimeout(DWORD aTimeout);
extern DWORD GetKbdTimeout(void);

//// WaitKey Check List
//#define WAIT_ICC_INSERT 0x0100
//#define WAIT_ICC_REMOVE 0x0200

extern void ClearKeyin(void);
extern DWORD GetKeyin(void);
extern DWORD WaitKey(DWORD aTimeout);
extern DWORD WaitKeyCheck(DWORD aTimeout, DWORD aCheckList);
extern DWORD YesNo(void);

//// Mode
//#define NUMERIC_INPUT   0x00000000
//#define AMOUNT_INPUT    0x00010000
//#define ALPHA_INPUT     0x00020000
//#define HIDE_NUMERIC    0x00030000
//#define HEX_INPUT       0x00040000
//#define BINARY_INPUT    0x00050000
//#define ALPHA_INPUT_NEW 0x00060000
//#define HEX_INPUT_NEW   0x00070000
//#define DECIMAL_INPUT   0x00080000
//// Option
//#define JUSTIFIED       0x00100000
//#define ECHO            0x00200000
//#define PREFIX_ENB      0x00400000
//
//// Decimal Pos
//#define DECIMAL_NONE    0x00000000
//#define DECIMAL_POS1    0x01000000
//#define DECIMAL_POS2    0x02000000
//#define DECIMAL_POS3    0x03000000
//
//#define MAX_INPUT_LEN   (MW_MAX_LINESIZE*2)
//#define PREFIX_SIZE      4
//#define MAX_INPUT_PREFIX   (MAX_INPUT_LEN - PREFIX_SIZE)
//
//#define IMIN(x)         (x<<8)
//#define IMAX(x)         (x&0xFF)
//
//#define RIGHT_JUST      (0xFF)

// aEntryMode = Decimal_pos | Option | Mode | MW_xxFONT | MW_LINEx | x position (0xFF=>Right Justify)
// aLen       = IMIN(a) + IMAX(b)
extern BOOLEAN GetKbd(DWORD aEntryMode, DWORD aLen, BYTE *aOutBuf);

#endif // _INC_KDBUTIL_H_
