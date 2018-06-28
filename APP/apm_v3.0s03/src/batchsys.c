//-----------------------------------------------------------------------------
//  File          : batchsys.c
//  Module        :
//  Description   : Include Batch system functions.
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
#include "midware.h"
#include "sysutil.h"
#include "apm.h"
#include "batchsys.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define MAX_BATCH_REC     1024          // KC++ increase number of record
#define MAX_REC_SIZE      512

struct BATCH_REC {
  struct BATSYS_HDR header;
  BYTE   content[MAX_REC_SIZE];
};

struct BATREC_LIST {
  struct BATSYS_HDR header;
  int    rec_idx;
};
//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
static int gMaxAcq=-1;
static WORD gMaxRec, gRecSize, gCurrRecCount;
static int *gBFileHdl=NULL;
static struct BATREC_LIST *gRecList=NULL;

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
static const char KBatchFile[]  = {"BatSys"};

//*****************************************************************************
//  Function        : CreateDummyRev
//  Description     : Create dummy reversal record if necessary
//  Input           : aFileHandle;         // file handle
//  Return          : N/A
//  Note            : //29-11-13 JC ++
//  Globals Changed : N/A
//*****************************************************************************
static void CreateDummyRev(int aFileHandle)
{
  struct BATCH_REC rev_rec;

  if (aFileHandle < 0)
    return;
  
  if (fLengthMW(aFileHandle) == 0) {
    // reserve first reversal record if new batch file
    memset(&rev_rec, 0, sizeof(struct BATCH_REC));
    rev_rec.header.b_status = STS_REC_EMPTY;
    fWriteMW(aFileHandle, &rev_rec, sizeof(struct BATCH_REC));
    fCommitAllMW();
  }
}
//*****************************************************************************
//  Function        : OpenBatchFile
//  Description     : Open all batch file for acquirer, create one if not exist
//  Input           : aAcqId;         // acquirer id,
//  Return          : file handle;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int OpenBatchFile(int aAcqId)
{
  #if (DEBUG_POINT)
  char tmpbuf[MW_MAX_LINESIZE+1];
  #endif
  char filename[32];
  int  fHandle;

  sprintf(filename, "B%02x", aAcqId);
  fHandle = fCreateMW(filename, 0);   // Private file & no encryption
  if (fHandle >= 0) {
    #if (DEBUG_POINT)
    sprintf(tmpbuf, "Create: %s", filename);
    DispLineMW(tmpbuf, MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
    #endif
    Delay1Sec(1, 0);
  }
  if (fHandle < 0)
    fHandle = fOpenMW(filename);
    
  CreateDummyRev(fHandle);            //29-11-13 JC ++
  return fHandle;
}
//*****************************************************************************
//  Function        : PurgeBatchFile
//  Description     : Delete & create new batch file for acquirer.
//  Input           : aAcqId;         // acquirer id,
//                    aFileHandle;    // current file handle
//  Return          : file handle;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int PurgeBatchFile(int aAcqId, int aFileHandle)
{
  char filename[32];

  if (aFileHandle >= 0) {
    fCloseMW(aFileHandle);
    aFileHandle = -1;
  }

  sprintf(filename, "B%02x", aAcqId);
  fDeleteMW(filename);
  aFileHandle = fCreateMW(filename, 0);   // Private file & no encryption
  CreateDummyRev(aFileHandle);            //29-11-13 JC ++
  return aFileHandle;
}
//*****************************************************************************
//  Function        : BatSysClose
//  Description     : Close Batch System.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void BatSysClose(void)
{
  int i;

  if (gMaxAcq == -1)  // BatchSys Not Init
    return;

  fCommitAllMW();
  for (i = 0; i < gMaxAcq; i++) {
    if (gBFileHdl[i] >= 0)
      fCloseMW(gBFileHdl[i]);
  }
  gMaxAcq = -1;
  if (gBFileHdl != NULL) {
    FreeMW(gBFileHdl);
    gBFileHdl = NULL;
  }
  if (gRecList != NULL) {
    FreeMW(gRecList);
    gRecList = NULL;
  }
}
////*****************************************************************************
////  Function        : GetLastRecStatus
////  Description     : Get the last record status for the batch.
////  Input           : aAcqId;
////  Return          : N/A
////  Note            : N/A
////  Globals Changed : N/A
////*****************************************************************************
//static BYTE GetLastRecStatus(int aAcqId)
//{
//  struct BATSYS_HDR header;
//  int file_size;
//
//  file_size = fLengthMW(gBFileHdl[aAcqId]);
//  if (file_size >= sizeof(struct BATCH_REC))
//    file_size -= sizeof(struct BATCH_REC);
//  else
//    file_size = 0;
//
//  if (fSeekMW(gBFileHdl[aAcqId], file_size) != file_size)
//    return STS_REC_UNKNOWN;
//
//  if (fReadMW(gBFileHdl[aAcqId], &header, sizeof(struct BATSYS_HDR)) != sizeof(struct BATSYS_HDR))
//    return STS_REC_UNKNOWN;
//
//  return header.b_status;
//}
//*****************************************************************************
//  Function        : BatSysInit
//  Description     : Initialize Batch System.
//  Input           : aMaxAcq;        // number of acquirer support
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN BatSysInit(int aMaxAcq)
{
  int i;

  BatSysClose();

  gMaxRec  = MAX_BATCH_REC;
  gRecSize = MAX_REC_SIZE;
  gMaxAcq = aMaxAcq;

  if (aMaxAcq == 0)
    return TRUE;

  gBFileHdl = (int *) MallocMW(sizeof(int) * aMaxAcq);
  gRecList =  (struct BATREC_LIST *) MallocMW(sizeof(struct BATREC_LIST) * MAX_BATCH_REC);
  if ((gBFileHdl == NULL) || (gRecList == NULL)) {
    BatSysClose();
    return FALSE;
  }

  // Open All Files
  gCurrRecCount = 0;
  for (i = 0; i < aMaxAcq; i++) {
    gBFileHdl[i] = OpenBatchFile(i);
  }

  BuildTraceList(TRUE);
  return TRUE;
}
//*****************************************************************************
//  Function        : BatSysClear
//  Description     : Clear Batch Record
//  Input           : aAcqId;         // acquirer id,
//                                    // -1 => All Acquirers
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void BatSysClear(int aAcqId)
{
  int i;

  if (gMaxAcq == -1)  // BatchSys Not Init
    return;

  fCommitAllMW();
  if (aAcqId != -1)
    gBFileHdl[aAcqId] = PurgeBatchFile(aAcqId, gBFileHdl[aAcqId]);
  else {
    for (i = 0; i < gMaxAcq; i++) {
      gBFileHdl[i] = PurgeBatchFile(i, gBFileHdl[i]);
    }
  }
  BuildTraceList(TRUE);
}
//*****************************************************************************
//  Function        : BatchFull
//  Description     : Create an empty record for acquirer.
//  Input           : aAcqId;         // acquirer id,
//  Return          : TRUE/FALSE;     // FALSE => fail to create empty record.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN BatchFull(int aAcqId)
{
  #define MIN_FREE_SIZE     (20*1024)
  //struct BATCH_REC batch_rec;
  int file_size;
  //BYTE tmp[MW_MAX_LINESIZE+1];

  if (aAcqId >= gMaxAcq)
    return TRUE;

  if (gCurrRecCount >= gMaxRec)
    return TRUE;

  file_size = fLengthMW(gBFileHdl[aAcqId]);
  if (file_size >= sizeof(struct BATCH_REC))
    file_size -= sizeof(struct BATCH_REC);
  else
    file_size = 0;

  if (fSeekMW(gBFileHdl[aAcqId], file_size) != file_size)
    return TRUE;

  //sprintf(tmp, "Free: %d", os_file_get_free_space());
  //DispLineMW(tmp, MW_MAX_LINE, MW_REVERSE|MW_CENTER|MW_SPFONT);
  //while(os_kbd_getkey()==0) os_sleep();

  // Check Disk Space
  if (os_file_get_free_space() > MIN_FREE_SIZE) {
    return FALSE;
  }

  //if (fReadMW(gBFileHdl[aAcqId], &batch_rec, sizeof(struct BATCH_REC)) != sizeof(struct BATCH_REC)) {
  //  if (batch_rec.header.b_status == STS_REC_EMPTY)
  //    return FALSE;
  //}

  //// if last is not batch rec, reuse it, else append empty record.
  //if (batch_rec.header.b_status != STS_REC_BATCH)
  //  fSeekMW(gBFileHdl[aAcqId], file_size);
  //batch_rec.header.b_status = STS_REC_EMPTY;
  //if (fWriteMW(gBFileHdl[aAcqId], &batch_rec, sizeof(struct BATCH_REC)) == sizeof(struct BATCH_REC))
  //  return FALSE;

  return TRUE;
}
//*****************************************************************************
//  Function        : SaveBatRec
//  Description     : Save Batch Record in the last empty or reversal slot.
//  Input           : aRec;           // pointer to batch record content
//                    aHdr;           // pointer to barch record header.
//  Return          : TRUE/FALSE;     // FALSE => operation fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SaveBatRec(void *aRec, struct BATSYS_HDR *aHdr)
{
  struct BATCH_REC batch_rec;
  int file_size;
  //BYTE status;

  if (aHdr->w_acq_id >= gMaxAcq)
    return FALSE;

  CheckPointerAddr(aRec);
  CheckPointerAddr(aHdr);

  // No Need
  //status = GetLastRecStatus(aHdr->w_acq_id);
  //if ((status != STS_REC_EMPTY) && (status != STS_REC_REVERSAL))
  //  return FALSE;
  
  //14-12-12 JC ++
  if ((aHdr->b_status == STS_REC_EMPTY)||(aHdr->b_status == STS_REC_REVERSAL))
    return SaveRevRec(aRec, aHdr);
  //14-12-12 JC --
  
  memcpy(&batch_rec.header, aHdr, sizeof(struct BATSYS_HDR));
  memcpy(&batch_rec.content, aRec, gRecSize);
  file_size = fLengthMW(gBFileHdl[aHdr->w_acq_id]);
  //if (file_size >= sizeof(struct BATCH_REC))
  //  file_size -= sizeof(struct BATCH_REC);
  //else
  //  file_size = 0;
  if (fSeekMW(gBFileHdl[aHdr->w_acq_id], file_size) != file_size)
    return FALSE;

  if (fWriteMW(gBFileHdl[aHdr->w_acq_id], &batch_rec, sizeof(struct BATCH_REC)) == sizeof(struct BATCH_REC)) {
    memcpy(&gRecList[gCurrRecCount], &batch_rec.header, sizeof(struct BATSYS_HDR));
    gRecList[gCurrRecCount].rec_idx = file_size/sizeof(struct BATCH_REC);
    if (batch_rec.header.b_status == STS_REC_BATCH)
      gCurrRecCount++;
    return TRUE;
  }

  return FALSE;
}
//*****************************************************************************
//  Function        : UpdateBatRec
//  Description     : Update Batch Record.
//  Input           : aIdx;
//                    aRec;           // pointer to batch record content
//                    aHdr;           // pointer to barch record header.
//  Return          : TRUE/FALSE;     // FALSE => operation fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdateBatRec(int aIdx, void *aRec, struct BATSYS_HDR *aHdr)
{
  struct BATCH_REC batch_rec;
  int offset;
  BOOLEAN rev2rec=FALSE;

  if (aHdr->w_acq_id >= gMaxAcq)
    return FALSE;

  CheckPointerAddr(aRec);
  CheckPointerAddr(aHdr);

  if (fReadMW(gBFileHdl[aHdr->w_acq_id], &batch_rec, sizeof(struct BATCH_REC)) == sizeof(struct BATCH_REC)) {
    if ((batch_rec.header.b_status == STS_REC_REVERSAL) && (aHdr->b_status == STS_REC_BATCH))
      rev2rec = TRUE;
  }

  memcpy(&batch_rec.header, aHdr, sizeof(struct BATSYS_HDR));
  memcpy(&batch_rec.content, aRec, gRecSize);
  offset = sizeof(struct BATCH_REC) * gRecList[aIdx].rec_idx;
  if (fSeekMW(gBFileHdl[aHdr->w_acq_id], offset) != offset)
    return FALSE;


  if (fWriteMW(gBFileHdl[aHdr->w_acq_id], &batch_rec, sizeof(struct BATCH_REC)) == sizeof(struct BATCH_REC)) {
    if (rev2rec == TRUE)
      gCurrRecCount++;
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : BuildTraceList
//  Description     : Build Record list by trace no.
//  Input           : aSort;      // TRUE => sort the list by trace_no
//  Return          : number of batch record on the list.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD BuildTraceList(BOOLEAN aSort)
{
  int i, j, rec_size, rec_idx;
  struct BATSYS_HDR header;
  struct BATREC_LIST *ptr, rec_list;
  int offset;

  // Read All Record
  ptr = gRecList;
  gCurrRecCount = 0;
  for (i = 0; i < gMaxAcq; i++) {
    if (gBFileHdl[i]<0)
      continue;
    rec_idx = 0;
    offset = 0;
    while (1) {
      if (fSeekMW(gBFileHdl[i], offset) != offset) {
        break;
      }
      rec_size = fReadMW(gBFileHdl[i], &header, sizeof(struct BATSYS_HDR));
      //printf("\x12\fRec: %d %d %X %X %c", i, gCurrRecCount, rec_size, sizeof(struct BATSYS_HDR), header.b_status);
      //while (os_kbd_getkey()==0) os_sleep(); // !TT
      offset += sizeof(struct BATCH_REC);
      if (rec_size != sizeof(struct BATSYS_HDR)) {
        break;
      }
      if (header.b_status != STS_REC_BATCH) {
        //22-11-12 JC ++
        //break;
        rec_idx++;
        continue;
        //22-11-12 JC -
      }
      memcpy(&ptr->header, &header, sizeof(struct BATSYS_HDR));
      ptr->rec_idx = rec_idx;
      rec_idx++;
      ptr++;
      gCurrRecCount++;
    }
  }

  if (!aSort)
    return gCurrRecCount;

  // Sort Record List
  for (i = 0; i < gCurrRecCount-1; i++) {
    for (j = i+1; j < gCurrRecCount; j++) {
      if (memcmp(gRecList[i].header.sb_trace_no, gRecList[j].header.sb_trace_no, sizeof(gRecList[i].header.sb_trace_no)) > 0) {
        memcpy(&rec_list, &gRecList[i], sizeof(struct BATREC_LIST));
        memcpy(&gRecList[i], &gRecList[j], sizeof(struct BATREC_LIST));
        memcpy(&gRecList[j], &rec_list, sizeof(struct BATREC_LIST));
      }
    }
  }
  return gCurrRecCount;
}
//*****************************************************************************
//  Function        : GetRecCount
//  Description     : Get Current Total batch record count.
//  Input           : N/A
//  Return          : record count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD GetRecCount(void)
{
  return gCurrRecCount;
}
//*****************************************************************************
//  Function        : GetBatchRec
//  Description     : Get Batch Record by index.
//  Input           : aIdx;         // record Idx
//                    aRec;         // pointer to record buffer.
//                    aRecSize;     // max record size
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetBatchRec(int aIdx, void *aRec, int aRecSize)
{
  struct BATCH_REC rec;
  int offset;

  if (aIdx >= gCurrRecCount)
    return FALSE;

  CheckPointerAddr(aRec);
  offset = sizeof(struct BATCH_REC) * gRecList[aIdx].rec_idx;
  if (fSeekMW(gBFileHdl[gRecList[aIdx].header.w_acq_id], offset) != offset)
    return FALSE;

  if (fReadMW(gBFileHdl[gRecList[aIdx].header.w_acq_id], &rec, sizeof(struct BATCH_REC)) != sizeof(struct BATCH_REC))
    return FALSE;

  memcpy(aRec, rec.content, aRecSize);
  return TRUE;
}
//*****************************************************************************
//  Function        : TraceInBatch
//  Description     : Locate whether the trace no is used.
//  Input           : aTraceNo;     // pointer to trace no.
//  Return          : -1;           // not in use
//                    others;       // record index with the number.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int TraceInBatch(BYTE *aTraceNo)
{
  int i;

  CheckPointerAddr(aTraceNo);
  for (i = 0; i < gCurrRecCount; i++) {
    if (memcmp(gRecList[i].header.sb_trace_no, aTraceNo, sizeof(gRecList[i].header.sb_trace_no)) == 0)
      return i;
  }
  return -1;
}
//*****************************************************************************
//  Function        : TraceInReversal
//  Description     : Locate whether the trace no is used for reversal rec.
//  Input           : aTraceNo;     // pointer to trace no.
//  Return          : TRUE/FALSE;
//  Note            : //14-12-12 JC ++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN TraceInReversal(BYTE *aTraceNo)
{
  int i;
  struct BATSYS_HDR header;

  CheckPointerAddr(aTraceNo);

  for (i = 0; i < gMaxAcq; i++) {
    if (fSeekMW(gBFileHdl[i], MW_FSEEK_SET) != 0)
      continue;

    if (fReadMW(gBFileHdl[i], &header, sizeof(struct BATSYS_HDR)) != sizeof(struct BATSYS_HDR))
      continue;
    if ((header.b_status == STS_REC_REVERSAL) && memcmp(header.sb_trace_no, aTraceNo, sizeof(header.sb_trace_no) == 0))
      return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : RocInBatch
//  Description     : Locate whether the ROC no is used.
//  Input           : aRocNo;       // pointer to ROC no.
//  Return          : -1;           // not in use
//                    others;       // record index with the number.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int RocInBatch(BYTE *aRocNo)
{
  int i;

  CheckPointerAddr(aRocNo);
  for (i = 0; i < gCurrRecCount; i++) {
    if (memcmp(gRecList[i].header.sb_roc_no, aRocNo, sizeof(gRecList[i].header.sb_roc_no)) == 0)
      return i;
  }
  return -1;
}
//*****************************************************************************
//  Function        : SaveRevRec
//  Description     : Save Reversal Record for the acquirer.
//  Input           : aRec;         // pointer to record buffer.
//                    aHdr;
//  Return          : -1;           // Fail
//                    others;       // success
//  Note            : //14-12-12 JC ++
//  Globals Changed : N/A
//*****************************************************************************
int SaveRevRec(void *aRec, struct BATSYS_HDR *aHdr)
{
  struct BATCH_REC batch_rec;

  if (aHdr->w_acq_id >= gMaxAcq)
    return FALSE;

  CheckPointerAddr(aRec);
  CheckPointerAddr(aHdr);

  memcpy(&batch_rec.header, aHdr, sizeof(struct BATSYS_HDR));
  memcpy(&batch_rec.content, aRec, gRecSize);
  
  if (fSeekMW(gBFileHdl[aHdr->w_acq_id], MW_FSEEK_SET) != 0)
    return FALSE;

  if (fWriteMW(gBFileHdl[aHdr->w_acq_id], &batch_rec, sizeof(struct BATCH_REC)) == sizeof(struct BATCH_REC))
    return TRUE;

  return FALSE;
}
//*****************************************************************************
//  Function        : GetRevRec
//  Description     : Get Reversal Record for the acquirer.
//  Input           : aAcqId;       // Acquirer id
//                    aRec;         // pointer to record buffer.
//                    aRecSize;
//  Return          : -1;           // Fail
//                    others;       // success
//  Note            : //14-12-12 JC ++
//  Globals Changed : N/A
//*****************************************************************************
int GetRevRec(int aAcqId, void *aRec, int aRecSize)
{
  struct BATCH_REC rec;

  CheckPointerAddr(aRec);
  if (fSeekMW(gBFileHdl[aAcqId], MW_FSEEK_SET) != 0)
    return -1;

  if (fReadMW(gBFileHdl[aAcqId], &rec, sizeof(struct BATCH_REC)) != sizeof(struct BATCH_REC))
    return -1;

  if (rec.header.b_status != STS_REC_REVERSAL)
    return -1;

  memcpy(aRec, rec.content, aRecSize);
  return 0;
}
//*****************************************************************************
//  Function        : ClearRevRec
//  Description     : Clear Reversal record.
//  Input           : aAcqId;       // Acquirer id
//  Return          : N/A
//  Note            : //14-12-12 JC ++
//  Globals Changed : N/A
//*****************************************************************************
void ClearRevRec(int aAcqId)
{
  struct BATSYS_HDR header;

  if (fSeekMW(gBFileHdl[aAcqId], MW_FSEEK_SET) != 0)
    return;

  if (fReadMW(gBFileHdl[aAcqId], &header, sizeof(struct BATSYS_HDR)) != sizeof(struct BATSYS_HDR))
    return;

  if (header.b_status != STS_REC_REVERSAL)
    return;

  if (fSeekMW(gBFileHdl[aAcqId], MW_FSEEK_SET) != 0)
    return;
    
  header.b_status = STS_REC_EMPTY;

  fWriteMW(gBFileHdl[aAcqId], &header, sizeof(struct BATSYS_HDR));
}
