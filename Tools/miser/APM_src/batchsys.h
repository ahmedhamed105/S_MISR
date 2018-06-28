//-----------------------------------------------------------------------------
//  File          : batchsys.h
//  Module        :
//  Description   : Declrartion & Defination for batchsys.c
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
#ifndef _INC_BATCHSYS_H_
#define _INC_BATCHSYS_H_
#include "common.h"

//-----------------------------------------------------------------------------
//     Defines
//-----------------------------------------------------------------------------
#if 0 // Move to manager.h
#define STS_REC_EMPTY       'E'
#define STS_REC_BATCH       'B'
#define STS_REC_REVERSAL    'R'
#define STS_REC_UNKNOWN     'U'

struct BATSYS_HDR {
  BYTE status;
  BYTE trace_no[3];
  BYTE roc_no[3];
  WORD acq_id;
  WORD app_id;
};
#endif
//-----------------------------------------------------------------------------
//     Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//     Function API
//-----------------------------------------------------------------------------
extern void BatSysClose(void);
extern BOOLEAN BatSysInit(int aMaxAcq);
extern void BatSysClear(int aAcqId);   // aAcqId == -1 => All
extern BOOLEAN BatchFull(int aAcqId);

extern WORD BuildTraceList(BOOLEAN aSort);
extern WORD GetRecCount(void);
extern BOOLEAN GetBatchRec(int aIdx, void *aRec, int aRecSize);

extern int TraceInBatch(BYTE *aTraceNo);
extern int RocInBatch(BYTE *aRocNo);

extern BOOLEAN TraceInReversal(BYTE *aTraceNo);

extern int SaveRevRec(void *aRec, struct BATSYS_HDR *aHdr);
extern int GetRevRec(int aAcqId, void *aRec, int aRecSize);
extern void ClearRevRec(int aAcqId);
extern BOOLEAN SaveBatRec(void *aRec, struct BATSYS_HDR *aHdr);
extern BOOLEAN UpdateBatRec(int aIdx, void *aRec, struct BATSYS_HDR *aHdr);

#endif // _INC_BATCHSYS_H_

