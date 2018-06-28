//-----------------------------------------------------------------------------
//  File          : TLVUtil.c
//  Module        :
//  Description   : Utilities for TLV data structure data.
//  Author        : Pody
//  Notes         :
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
//  XX Apr  2008  Pody        Initial Version.
//  25 Sept 2008  Lewis
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "tlvutil.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constant
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Generic API
//----------------------------------------------------------------------------

//*****************************************************************************
//  Function        : bcd2BYTE
//  Description     : Convert packed BCD to binary.
//  Input           : packed BCD byte
//  Return          : binary value.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BYTE bcd2BYTE(BYTE aBcd)
{
  return (BYTE)((aBcd>>4)*10 + (aBcd&0x0F));
}
//*****************************************************************************
//  Function        : DDWORD2bcd
//  Description     : Convert DDWORD to packed BCD of 20-digit
//  Input           : dest buffer, DDWORD value
//  Return          : N/A
//  Note            : expect size of dest buffer is 10 BYTE
//  Globals Changed : N/A
//*****************************************************************************
void DDWORD2bcd(BYTE *aDest, DDWORD aVal)
{
  int i;

  memset(aDest,0,10);
  for (i=9;(i>=0) && aVal;i--) {
    aDest[i] = (BYTE)(aVal % 10);
    aVal /= 10;
    aDest[i] += (BYTE)((aVal % 10) * 16);
    aVal /= 10;
  }
}
//*****************************************************************************
//  Function        : bcd2DDWORD
//  Description     : Convert packed BCD  to binary DDWORD
//  Input           : dest buffer, len of packed bcd
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DDWORD bcd2DDWORD(BYTE *aDest, DWORD aLen)
{
  int i;
  DWORD tmp = 0;

  for (i = 0; i < aLen; i++) {
    tmp = tmp * 100 + bcd2BYTE(*aDest++);
  }
  return tmp;
}
//*****************************************************************************
// Function        : WGet
// Description     : Get WORD value from buffer.
// Input           : buffer pointer
// Return          : WORD value
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
static WORD WGet(BYTE *aSrc)
{
  return (WORD)((WORD)*(aSrc+1) + (WORD)(*aSrc << 8));
}
//*****************************************************************************
// Function        : WPut
// Description     : Save WORD data to buffer.
// Input           : buffer pointer, WORD value
// Return          : N/A
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
void WPut(BYTE *aDest, WORD aVal)
{
  *aDest     = (BYTE)(aVal >> 8);
  *(aDest+1) = (BYTE)(aVal & 0xff);
}
//*****************************************************************************
// Function        : TlvSizeOf
// Description     : Get total size of TLV string
// Input           : TLV pointer
// Return          : Total TLV string size
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
WORD TlvSizeOf(BYTE *aTLV)
{
  if (aTLV == NULL)  return 0;
  return (WORD)(TlvTSize(aTLV) + TlvLSize(aTLV) + TlvLen(aTLV));
}
//*****************************************************************************
// Function        : TlvTSize
// Description     : Get tag size
// Input           : TLV pointer
// Return          : Tag size (1 or 2)
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
WORD TlvTSize(BYTE *aTLV)
{
  if (aTLV == NULL)  return 0;
  return (WORD) (((*aTLV & 0x1F) == 0x1F) ? 2 : 1);
}
//*****************************************************************************
// Function        : TlvTag
// Description     : Get tag value
// Input           : TLV pointer
// Return          : Tag value
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
WORD TlvTag(BYTE *aTLV)
{
  if (aTLV == NULL)  return 0;
  if ((*aTLV & 0x1F) != 0x1F)
    return *aTLV;
  return WGet(aTLV);
}
//*****************************************************************************
// Function        : TlvSize
// Description     : Get L size
// Input           : TLV pointer
// Return          : L size
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
WORD TlvLSize(BYTE *aTLV)
{
  BYTE *pb;

  if (aTLV == NULL)  return 0;
  pb = aTLV + TlvTSize(aTLV);
  if (*pb & 0x80) {
    return (WORD)((*pb & 0x7F) + 1);
  }
  return 1;
}
//*****************************************************************************
// Function        : TlvLen
// Description     : Get len value
// Input           : TLV pointer
// Return          : Len value
//                   0xFFFF => Undefine
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
WORD TlvLen(BYTE *aTLV)
{
  BYTE *pb;

  if (aTLV == NULL)  return 0;
  pb = aTLV + TlvTSize(aTLV);
  if ((*pb & 0x80) == 0) {
    return *pb;
  }
  else if (*pb == 0x81) {
    return *(pb + 1);
  }
  else if (*pb == 0x82) {
    return WGet(pb + 1);
  }
  return 0xFFFF;
}
//*****************************************************************************
// Function        : TlvPtr
// Description     : Get var data pointer
// Input           : TLV pointer
// Return          : VAR ptr
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
BYTE *TlvVPtr(BYTE *aTLV)
{
  if (aTLV == NULL)  return NULL;
  return aTLV + TlvTSize(aTLV) + TlvLSize(aTLV);
}
//*****************************************************************************
// Function        : TlvSeek
// Description     : Seek aTag from aTlvList
// Input           : TLV list pointer, list len, tag to be found
// Return          : NULL - err; pointer to found tag
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
BYTE *TlvSeek(BYTE *aTlvList, WORD aLen, WORD aTag)
{
  BYTE *pb;

  if (aTlvList == NULL)  return NULL;
  pb = aTlvList;
  while (pb < aTlvList + aLen) {
    if (TlvTag(pb) == aTag) {
      return pb;
    }
    pb += TlvTSize(pb) + TlvLSize(pb) + TlvLen(pb);
  }
  return NULL;
}
//*****************************************************************************
// Function        : TlvNext
// Description     : point to next TLV data
// Input           : TLV list pointer
// Return          : NULL - err; pointer to next TLV
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
BYTE *TlvNext(BYTE *aTlvList)
{
  if (aTlvList == NULL) return NULL;
  return (aTlvList + TlvSizeOf(aTlvList));
}
//*****************************************************************************
// Function        : TlvIsCTag
// Description     : Check if constructed tag or not
// Input           : Tag value
// Return          : T/F
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
BOOLEAN TlvIsCTag(WORD aTag)
{
  BYTE first_byte;

  first_byte = (BYTE)((aTag>0x00FF)?(aTag>>8):(aTag));
  if (first_byte&20) {
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
// Function        : TlbPack
// Description     : pack TLV buffer
// Input           : aTag, aLen, aVar, aOut
// Return          : output length
// Note            : N/A
// Globals Changed : N/A
//*****************************************************************************
WORD TlvPack(WORD aTag, WORD aLen, BYTE *aVar, BYTE *aOut)
{
  WORD wRet = 0;

  // tag
  if (aTag > 0x00FF) {
    aOut[wRet++] = (BYTE)(aTag>>8);
    aOut[wRet++] = (BYTE)(aTag&0x00FF);
  }
  else {
    aOut[wRet++] = (BYTE)aTag;
  }
  // len
  if (aLen < 0x0080) {
    aOut[wRet++] = (BYTE)aLen;
  }
  else if (aLen < 0x0100) {
    aOut[wRet++] = 0x81;
    aOut[wRet++] = (BYTE)aLen;
  }
  else {
    aOut[wRet++] = 0x82;
    aOut[wRet++] = (BYTE)(aLen>>8);
    aOut[wRet++] = (BYTE)(aLen&0x00FF);
  }
  // var
  memcpy(aOut+wRet, aVar, aLen);
  wRet += aLen;

  return wRet;
}

