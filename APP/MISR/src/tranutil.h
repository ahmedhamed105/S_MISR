//-----------------------------------------------------------------------------
//  File          : tranutil.h
//  Module        :
//  Description   : Declrartion & Defination for tranutil.c
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
#ifndef _TRANTUIL_H_
#define _TRANTUIL_H_
#include "common.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
#define MAX_CHR_PER_LINE  (16)

#define TRANS_ACP         0
#define TRANS_REJ         1
#define TRANS_FAIL        2
#define LINE_FAIL         3
#define MORE_RSP          4

struct INPUT_DATA {
  COMMON_DATA
  BYTE                  sb_pin[8];
  struct MW_TRK1BUF     s_trk1buf;
  struct MW_TRK2BUF     s_trk2buf;
  WORD                  w_card_idx;
  BYTE                  b_card_expired;
  BYTE                  sb_amex_4DBC[4];
};

struct TX_DATA {
  COMMON_DATA
  BYTE                  sb_pin[8];
  BYTE                  sb_proc_code[3];
  BYTE                  b_org_trans;
  BYTE                  sb_org_trace_no[3];
};

struct RSP_DATA {
  COMMON_DATA
  BYTE                  b_response;
  BYTE text[70];
  BYTE Actioncode[3];
};

//-----------------------------------------------------------------------------
//      Global Data Structure
//-----------------------------------------------------------------------------
struct TRANS_DATA {
  struct INPUT_DATA     s_input;
  struct TX_DATA        s_tx_data;
  struct RSP_DATA       s_rsp_data;
};
extern struct TRANS_DATA gGTS;

#define INPUT             gGTS.s_input
#define TX_DATA           gGTS.s_tx_data
#define RSP_DATA          gGTS.s_rsp_data

//-----------------------------------------------------------------------------
//    Functions
//-----------------------------------------------------------------------------
extern BOOLEAN OverMargin(void);
extern void PackDTGAA(void);
extern void PackRecordP(BOOLEAN aReprint);
extern void PackInputP(void);
extern void DispHeader(BYTE *aCardName);
extern void SetRspCode(WORD aRspCode);
extern void ClearResponse(void);
extern BOOLEAN ConfirmCard(void);
extern BOOLEAN TransAllowed(DWORD aInputMode);
extern void PackRspText(void);
extern void DispRspText(BOOLEAN aDispHdr);
extern void DispErrorMsg(const BYTE *aMsg);
extern void TransEnd(BOOLEAN aShowCard);
extern void DispAmount(DDWORD aAmount, DWORD aLineNo, DWORD aFont);
extern void AddToTotals(void);
extern void CalTotals(BOOLEAN aAll);
extern void PackIssuerSum(BOOLEAN aAll);
extern void PackIssuerSumT(WORD acq_idx);


extern void FormatHolderName(BYTE *aDest, BYTE *aName, BYTE aLen);
extern void IncTraceNo(void);
extern void IncTraceNoEND(void);
extern void ClearRspData(void);
extern void ResetTerm(void);
extern void PackComm(DWORD aHostIdx, BOOLEAN aSettle);
extern void PackHeader(void);
extern void Packprintconfig(void);
extern void Packprintissuer(void);

#endif // _TRANUTIL_H_

