//-----------------------------------------------------------------------------
//  File          : corevar.c
//  Module        :
//  Description   : Include Globals variables for EDC.
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
#include <string.h>
#include "midware.h"
#include "hardware.h"
#include "constant.h"
#include "util.h"
#include "corevar.h"
#include "record.h"

//-----------------------------------------------------------------------------
//      Global Data Structure
//-----------------------------------------------------------------------------
struct GDS *gGDS;
struct APPDATA gAppDat;

//----------------------------------------------------------------------
//  Application data file
//----------------------------------------------------------------------
int gAppDatHdl = -1;
static const char KAppDatFile[] = {"EDCFILE"};

//*****************************************************************************
//  Function        : DataFileInit
//  Description     : Init. Data file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DataFileInit(void)
{
  BYTE filename[32];
  struct APPDATA app_data;

  strcpy(filename, KAppDatFile);
  gAppDatHdl = fOpenMW(filename);
  if (gAppDatHdl < 0) {
    DispLineMW("Create: EDCFile", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
    gAppDatHdl = fCreateMW(filename, FALSE);
    memset(&app_data, 0, sizeof(struct APPDATA));
    fWriteMW(gAppDatHdl, &app_data, sizeof(struct APPDATA));
    fCommitAllMW();
  }
  else
    fReadMW(gAppDatHdl, &app_data, sizeof(struct APPDATA));

  fCloseMW(gAppDatHdl);

  memcpy(&gAppDat, &app_data, sizeof(struct APPDATA));
}
//*****************************************************************************
//  Function        : DataFileUpdate
//  Description     : Update Data File from memory.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DataFileUpdate(void)
{
  struct APPDATA app_dat;
  BYTE filename[32];

  memcpy(&app_dat, &gAppDat, sizeof(struct APPDATA));

  strcpy(filename, KAppDatFile);
  gAppDatHdl = fOpenMW(filename);
  fWriteMW(gAppDatHdl, &app_dat, sizeof(struct APPDATA));
  fCommitAllMW();
  fCloseMW(gAppDatHdl);

}
//*****************************************************************************
//  Function        : DataFileClose
//  Description     : Close local data file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DataFileClose(void)
{
  struct APPDATA app_dat;
  BYTE filename[32];

  strcpy(filename, KAppDatFile);
  gAppDatHdl = fOpenMW(filename);

  memcpy(&app_dat, &gAppDat, sizeof(struct APPDATA));
  fWriteMW(gAppDatHdl, &app_dat, sizeof(struct APPDATA));
  fCloseMW(gAppDatHdl);
  fCommitAllMW();
  gAppDatHdl = -1;
}
//*****************************************************************************
//  Function        : CorrectHost
//  Description     : Check Whether Support This host.
//  Input           : aHostType;        // Host Type
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CorrectHost(BYTE aHostType)
{
  //return (aHostType == HANDLE_HOST_TYPE);
  return TRUE;
}
//*****************************************************************************
//  Function        : GenAuthCode
//  Description     : Auto generate Authorization code.
//  Input           : aAuthCode;        // pointer to 6 byte buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GenAuthCode(BYTE *aAuthCode)
{
  bcdinc(gAppDat.auto_auth_code, 3);
  split(aAuthCode, gAppDat.auto_auth_code, 3);
}
//*****************************************************************************
//  Function        : SetDefault
//  Description     : Restore to Default Setup.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetDefault(void)
{
  memset(&gAppDat, 0, sizeof(struct APPDATA));
}
//*****************************************************************************
//  Function        : MsgBufSetup
//  Description     : Initialize pack utils for msgbuf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void MsgBufSetup(void)
{
  set_pptr(MSG_BUF.sb_content,MSG_BUF_LEN);
}
//*****************************************************************************
//  Function        : PackMsgBufLen
//  Description     : Update the msgbuf's len;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackMsgBufLen(void)
{
  MSG_BUF.d_len = get_distance();
}
//*****************************************************************************
//  Function        : TxBufSetup
//  Description     : Initialize pack utils for txbuf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void TxBufSetup(BOOLEAN aAdd2ByteLen)
{
  set_pptr(TX_BUF.sbContent,MSG_BUF_LEN);
  // pack additonal 2 byte msg len for TCP/IP connection
  if (aAdd2ByteLen)
    inc_pptr(2);
}
//*****************************************************************************
//  Function        : PackTxBufLen
//  Description     : Update the TxBuf's len;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackTxBufLen(BOOLEAN aAdd2ByteLen)
{
  DWORD len;

  TX_BUF.wLen = get_distance();
  if (aAdd2ByteLen) {
    len = TX_BUF.wLen - 2;
    TX_BUF.sbContent[0] = (BYTE)((len >> 8) & 0xFF);
    TX_BUF.sbContent[1] = (BYTE)(len & 0xFF);
  }
}
//*****************************************************************************
//  Function        : RxBufSetup
//  Description     : Initialize pack utils for txbuf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void RxBufSetup(BOOLEAN aAdd2ByteLen)
{
  set_pptr(RX_BUF.sbContent,MSG_BUF_LEN);
  // pack additonal 2 byte msg len for TCP/IP connection
  if (aAdd2ByteLen)
    inc_pptr(2);
}
//*****************************************************************************
//  Function        : PackRxBufLen
//  Description     : Update the TxBuf's len;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackRxBufLen(BOOLEAN aAdd2ByteLen)
{
  DWORD len;

  RX_BUF.wLen = get_distance();
  if (aAdd2ByteLen) {
    len = RX_BUF.wLen - 2;
    RX_BUF.sbContent[0] = (BYTE)((len >> 8) & 0xFF);
    RX_BUF.sbContent[1] = (BYTE)(len & 0xFF);
  }
}
//*****************************************************************************
//  Function        : ValidRecordCRC
//  Description     : Valid the record's crc.
//  Input           : aIdx;      // record index
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ValidRecordCRC(WORD aIdx)
{
  return (RECORD_BUF.w_crc == cal_crc((BYTE *)&RECORD_BUF,
                                      (BYTE *)&RECORD_BUF.w_crc-(BYTE *)&RECORD_BUF.b_trans));
}
//*****************************************************************************
//  Function        : RefreshDispAfter
//  Description     : Set Display Timer & display changed flag.
//  Input           : aSec;     // timer in sec.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void RefreshDispAfter(DWORD aSec)
{
  gGDS->b_disp_chgd = TRUE;
  aSec *= 200;  // convert unit of 10ms
  TimerSetMW(gTimerHdl[TIMER_DISP], aSec);
}
//*****************************************************************************
//  Function        : FreeGDS
//  Description     : Free GDS buffer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void FreeGDS(void)
{
  if (gGDS != NULL)
    FreeMW(gGDS);
}
//*****************************************************************************
//  Function        : MallocGDS
//  Description     : Malloc all buffer required.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MallocGDS(void)
{
  gGDS = (struct GDS *) MallocMW(sizeof(struct GDS));
  MemFatalErr(gGDS);  // Check Memory
  memset(gGDS, 0x00, sizeof(struct GDS));
  gGDS->s_EMVIn.bMsg = gGDS->sb_IOBuf;
  gGDS->s_EMVOut.bMsg = gGDS->sb_IOBuf;
  gGDS->s_CTLIn.pbMsg = gGDS->sb_IOBuf;
  gGDS->s_CTLOut.pbMsg = gGDS->sb_IOBuf;
  return TRUE;
}
//*****************************************************************************
//  Function        : TrainingModeON
//  Description     : Check training mode status;
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE => On.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN TrainingModeON(void)
{
  return (STIS_TERM_DATA.w_training_status == MAGIC);
}
//*****************************************************************************
//  Function        : IncAPMTraceNoEnd
//  Description     : set fiunal Increment the system trace no & save to input & tx_data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IncAPMTraceNoEnd(void)
{
  bcdinc(STIS_TERM_DATA.sb_trace_no, 3);
  if (memcmp(STIS_TERM_DATA.sb_trace_no, "\x00\x00\x00", 3)==0)
    bcdinc(STIS_TERM_DATA.sb_trace_no, 3);
  APM_SetTrace(STIS_TERM_DATA.sb_trace_no);
}
//*****************************************************************************
//  Function        : IncTraceNo
//  Description     : Increment the system trace no & save to input & tx_data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IncAPMTraceNo(void)
{
  bcdinc(STIS_TERM_DATA.sb_trace_no, 3);
  if (memcmp(STIS_TERM_DATA.sb_trace_no, "\x00\x00\x00", 3)==0)
    bcdinc(STIS_TERM_DATA.sb_trace_no, 3);
  APM_SetTrace(STIS_TERM_DATA.sb_trace_no);
}
//*****************************************************************************
//  Function        : CTLEnable
//  Description     : Enable/Disable CTL trans.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
void CTLEnable(BOOLEAN aEnable)
{
  gAppDat.s_ctl_data.b_enable = aEnable;
  DataFileUpdate();
}
//*****************************************************************************
//  Function        : CTLEnabled
//  Description     : CTL trans enabled checking.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
BOOLEAN CTLEnabled(void)
{
  return (BOOLEAN)gAppDat.s_ctl_data.b_enable;
}
//*****************************************************************************
//  Function        : PPPCfgUpdate
//  Description     : Update PPP Config for GPRS connection.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
void PPPCfgUpdate(struct PPPSREG *aPppCfg)
{
  memcpy(&gAppDat.s_ppp_cfg, aPppCfg, sizeof(struct PPPSREG));
  DataFileUpdate();
}
