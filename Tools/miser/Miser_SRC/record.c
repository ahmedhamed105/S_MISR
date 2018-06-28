//-----------------------------------------------------------------------------
//  File          : record.c
//  Module        :
//  Description   : Include routines for handling record operation.
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
#include <stdio.h>
#include <string.h>
#include "corevar.h"
#include "constant.h"
#include "message.h"
#include "tranutil.h"
#include "util.h"
#include "record.h"

//*****************************************************************************
//  Function        : MoveInput2Tx
//  Description     : Save data from input to TX_DATA.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : TX_DATA
//*****************************************************************************
void MoveInput2Tx(void)
{
  ByteCopy((BYTE *)&TX_DATA, (BYTE *)&INPUT, TX_DATA.sb_proc_code-&TX_DATA.b_trans);
  ByteCopy((BYTE *)&TX_DATA.s_icc_data, (BYTE *)&INPUT.s_icc_data, sizeof(struct ICC_DATA));
}
//*****************************************************************************
//  Function        : Input2RecordBuf
//  Description     : Save data from input to RECORD_BUF.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : RECORD_BUF;
//*****************************************************************************
void Input2RecordBuf(void)
{
  memset(&RECORD_BUF, 0x00, sizeof(struct TXN_RECORD));

  ByteCopy((BYTE *)&RECORD_BUF, (BYTE *)&INPUT, (DWORD)((DWORD)&RECORD_BUF.w_crc - (DWORD)&RECORD_BUF));
  memcpy(RECORD_BUF.sb_rrn, &RSP_DATA.sb_rrn, sizeof(RECORD_BUF.sb_rrn));
  memcpy(RECORD_BUF.sb_auth_code, &RSP_DATA.sb_auth_code, sizeof(RECORD_BUF.sb_auth_code));
  if (memcmp(TX_DATA.sb_trace_no, INPUT.sb_trace_no, 3) > 0)
    memcpy(RECORD_BUF.sb_trace_no, &TX_DATA.sb_trace_no, 3);
  RECORD_BUF.w_rspcode = RSP_DATA.w_rspcode;
  RECORD_BUF.b_PinVerified = RSP_DATA.b_PinVerified;
  RECORD_BUF.dd_org_amount = INPUT.dd_amount;
  memcpy(&RECORD_BUF.s_dtg, &RSP_DATA.s_dtg, sizeof(RECORD_BUF.s_dtg));
}
//*****************************************************************************
//  Function        : UpdateHostStatus
//  Description     : Update acquirer status;
//  Input           : aAction;       // host status flag
//  Return          : N/A
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
void UpdateHostStatus(BYTE aAction)
{
  struct BATSYS_HDR header;

  if (aAction == REV_PENDING) {
    ReadRTC(&RSP_DATA.s_dtg);
    Input2RecordBuf();
    header.b_status = STS_REC_REVERSAL;
    header.w_acq_id = INPUT.w_host_idx;
    memcpy(header.sb_trace_no, RECORD_BUF.sb_trace_no, sizeof(header.sb_trace_no));
    APM_SaveBatRec(&RECORD_BUF, &header);
  }
  /* KC++ 21-Sep-2012 Fixed batch record bug */
  else if (aAction == NO_PENDING) {
    header.b_status = STS_REC_EMPTY;
    header.w_acq_id = INPUT.w_host_idx;
    memcpy(header.sb_trace_no, RECORD_BUF.sb_trace_no, sizeof(header.sb_trace_no));
    APM_SaveBatRec(&RECORD_BUF, &header);
  }
  /* KC-- 21-Sep-2012 */
  APM_SetPending(INPUT.w_host_idx, aAction);
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : SaveRecord
//  Description     : Save record to Batch file
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SaveRecord(void)
{
  struct BATSYS_HDR header;

  Input2RecordBuf();
  header.b_status = STS_REC_BATCH;
  header.w_acq_id = RECORD_BUF.w_host_idx;
  memcpy(header.sb_trace_no, RECORD_BUF.sb_trace_no, sizeof(header.sb_trace_no));
  memcpy(header.sb_roc_no, RECORD_BUF.sb_roc_no, sizeof(header.sb_roc_no));
  RECORD_BUF.w_crc = cal_crc((BYTE *)&RECORD_BUF,
                             (BYTE *)&RECORD_BUF.w_crc-(BYTE *)&RECORD_BUF.b_trans);

  // Save Record & Clear Reversal
  APM_SaveBatRec(&RECORD_BUF, &header);
  UpdateHostStatus(NO_PENDING);
  fCommitAllMW();    // make sure all update in the same time
}
//*****************************************************************************
//  Function        : SaveOfflRecord
//  Description     : Save offline record to Batch file
//  Input           : N/A
//  Return          : N/A
//  Note            : //17-12-12 JC ++
//  Globals Changed : N/A
//*****************************************************************************
void SaveOfflRecord(void)
{
  struct BATSYS_HDR header;

  Input2RecordBuf();
  header.b_status = STS_REC_BATCH;
  header.w_acq_id = RECORD_BUF.w_host_idx;
  memcpy(header.sb_trace_no, RECORD_BUF.sb_trace_no, sizeof(header.sb_trace_no));
  memcpy(header.sb_roc_no, RECORD_BUF.sb_roc_no, sizeof(header.sb_roc_no));
  RECORD_BUF.w_crc = cal_crc((BYTE *)&RECORD_BUF,
                             (BYTE *)&RECORD_BUF.w_crc-(BYTE *)&RECORD_BUF.b_trans);

  // Save Record
  APM_SaveBatRec(&RECORD_BUF, &header);
  fCommitAllMW();    // make sure all update in the same time
}
//*****************************************************************************
//  Function        : UpdateRecord
//  Description     : Update transaction record to recovery buffer.
//  Input           : aRecIdx;      // Record Index
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void UpdateRecord(int aRecIdx)
{
  struct BATSYS_HDR header;

  header.b_status = STS_REC_BATCH;
  header.w_acq_id = RECORD_BUF.w_host_idx;
  memcpy(header.sb_trace_no, RECORD_BUF.sb_trace_no, sizeof(header.sb_trace_no));
  memcpy(header.sb_roc_no, RECORD_BUF.sb_roc_no, sizeof(header.sb_roc_no));
  RECORD_BUF.w_crc = cal_crc((BYTE *)&RECORD_BUF,
                             (BYTE *)&RECORD_BUF.w_crc-(BYTE *)&RECORD_BUF.b_trans);

  APM_UpdateBatRec(aRecIdx, &RECORD_BUF, &header);
  fCommitAllMW();    // make sure all update in the same time
}
