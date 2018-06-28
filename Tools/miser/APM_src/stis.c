//-----------------------------------------------------------------------------
//  File          : stis.c
//  Module        :
//  Description   : Include STIS data handling routines.
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
#include "midware.h"
#include "util.h"
#include "sysutil.h"
#include "menu.h"
#include "message.h"
#include "kbdutil.h"
#include "infodata.h"
#include "lptutil.h"
#include "stis.h"

//-----------------------------------------------------------------------------
//      MACRO
//-----------------------------------------------------------------------------
#define MAX(a, b) (((a) > (b))?(a):(b))

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
// File defination
// Term Config Table
struct TERM_CFG *gTermCfgOffset=0;
int    gTermCfgFd=-1;
// Card Table
struct CARD_TBL *gCrdTblOffset=0;
int    gCrdTblFd=-1;
// Issuer Table
struct ISSUER_TBL *gIssTblOffset=0;
int    gIssTblFd=-1;
// Acquirer Table
struct ACQUIRER_TBL *gAcqTblOffset=0;
int    gAcqTblFd=-1;
// Descriptor Table
struct DESC_TBL *gDscTblOffset=0;
int    gDscTblFd=-1;
// Logo Table
struct LOGO_TBL *gLogoTblOffset=0;
int    gLogoTblFd=-1;
// Extra Param Table
struct EXTRA_PARAM *gExParamOffset=0;
int    gExParamFd=-1;

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
static char KTermCfgFile[]  = { "TermCfg" };
static char KCrdTblFile[]   = { "CardTbl" };
static char KIssTblFile[]   = { "IssTbl" };
static char KAcqTblFile[]   = { "AcqTbl" };
static char KDscTblFile[]   = { "DscTbl" };
static char KLogoTblFile[]  = { "LogoTbl" };
static char KExParamFile[]  = { "ExParam" };

// Default Terminal Config Table
static const struct TERM_CFG KDefaultTermCfg = {
  0x00,                                                                     // DLL
  0x00,                                                                     // Init Control 00:No Action 01:Clear Batch
  0x00,                                                                     // Rsvd 0
  0x00,                                                                     // Rsvd 1
  {0x08, 0x01, 0x01, 0x00, 0x00, 0x00},                                     // Date Time YYMMDDHHMMSS
  0x00,                                                                     // Dial Option
  {'0', '0'},                                                               // Password
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Help Desk Phone
  {0xA3,                                                                    // Option 1 (Confirm Total, BS MMDD, print time, N/A, N/A, N/A, TIPs, DispMsr)
   0x02,                                                                    // Option 2 (N/A,N/A,N/A,N/A,N/A,N/A, No Printer, ECR ref)
   0x00,                                                                    // Option 3
   0x00																		// Option 4
  },
  {"  DEFAULT TERM CONFIG  SPECTRA TECHNOLOGIES   "},                       // Name/ Location
  {" PLEASE INIT. TERMINAL "},                                              // Default Name
  '$',                                                                      // Currency Symbol
  8,                                                                        // Trans Amount len
  2,                                                                        // Decimal Pos
  0x00,                                                                     // Rsvd 2
  12,                                                                       // Settlement Amount len
  {"HKD"},                                                                  // Currency Name
  0x00,                                                                     // Local Option
  {"                    "},                                                 // Additional Data
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               // Reserved field [30]
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               //
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },               //
  {"                "},                                                     // Password - Refund
  {"                "},                                                     // Password - Void
  {"                                "},                                     // username APN1
  {"                                "},                                     // Password APN1
  {"                                "},                                     // username APN2
  {"                                "},                                     // Password APN2
  {"                                "},                                     // config APN1
  {"                                "},                                     // config APN2
  {0x00, 0x00, 0x00, 0x00},                                                 // SIM2 IP
  {(10002 >> 8), (10002 & 0xff)},                                           // SIM2 Port
  0x0000,                                                                   // CRC
};
//*****************************************************************************
//  Function        : UpdTermCfg
//  Description     : Update Terminal Config File
//  Input           : aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdTermCfg(struct TERM_CFG *aDat)
{
  if (gTermCfgFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(gTermCfgFd, (DWORD) gTermCfgOffset, aDat, sizeof(struct TERM_CFG));
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdCardTbl
//  Description     : Update Card Table File
//  Input           : aIdx;       // Table Index
//                    aDat;       // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdCardTbl(DWORD aIdx, struct CARD_TBL *aDat)
{
  if (gCrdTblFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(gCrdTblFd, (DWORD) &gCrdTblOffset[aIdx], aDat, sizeof(struct CARD_TBL));
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdIssuerTbl
//  Description     : Update Issuer Table File
//  Input           : aIdx;       // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdIssuerTbl(DWORD aIdx, struct ISSUER_TBL *aDat)
{
  if (gIssTblFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(gIssTblFd, (DWORD) &gIssTblOffset[aIdx], aDat, sizeof(struct ISSUER_TBL));
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdAcqTbl
//  Description     : Update Acquirer Table File
//  Input           : aDat;         // pointer data buffer
//                    aCalCRC;      // 1 => ReCal CRC before update.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdAcqTbl(DWORD aIdx, struct ACQUIRER_TBL *aDat)
{
  if (gAcqTblFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(gAcqTblFd, (DWORD) &gAcqTblOffset[aIdx], aDat, sizeof(struct ACQUIRER_TBL));
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdDescTbl
//  Description     : Update Descriptor Table File
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdDescTbl(DWORD aIdx, struct DESC_TBL *aDat)
{
  if (gDscTblFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(gDscTblFd, (DWORD) &gDscTblOffset[aIdx], aDat, sizeof(struct DESC_TBL));
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdLogoTbl
//  Description     : Update Logo Table File
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdLogoTbl(DWORD aIdx, struct LOGO_TBL *aDat)
{
  if (gLogoTblFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(gLogoTblFd, (DWORD) &gLogoTblOffset[aIdx], aDat, sizeof(struct LOGO_TBL));
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdExParam
//  Description     : Update Extra Parameter Table File
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdExtraParam(DWORD aIdx, struct EXTRA_PARAM *aDat)
{
  if (gExParamFd < 0)
    return FALSE;

  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  WriteSFile(gExParamFd, (DWORD) &gExParamOffset[aIdx], aDat, sizeof(struct EXTRA_PARAM));
  return TRUE;
}
//*****************************************************************************
//  Function        : GetCardCount
//  Description     : Get Number of Card data in the table.
//  Input           : N/A
//  Return          : count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetCardCount(void)
{
  if (gCrdTblFd == -1)
    return 0;

  return fLengthMW(gCrdTblFd) / sizeof(struct CARD_TBL);
}
//*****************************************************************************
//  Function        : GetIssuerCount
//  Description     : Get Number of Issuer data in the table.
//  Input           : N/A
//  Return          : count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetIssuerCount(void)
{
  if (gIssTblFd == -1)
    return 0;

  return fLengthMW(gIssTblFd) / sizeof(struct ISSUER_TBL);
}
//*****************************************************************************
//  Function        : GetAcqCount
//  Description     : Get Number of acquirer data in the table.
//  Input           : N/A
//  Return          : count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetAcqCount(void)
{
  if (gAcqTblFd == -1)
    return 0;

  return fLengthMW(gAcqTblFd) / sizeof(struct ACQUIRER_TBL);
}
//*****************************************************************************
//  Function        : GetDescCount
//  Description     : Get Number of product code data in the table.
//  Input           : N/A
//  Return          : count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetDescCount(void)
{
  if (gDscTblFd == -1)
    return 0;

  return fLengthMW(gDscTblFd) / sizeof(struct DESC_TBL);
}
//*****************************************************************************
//  Function        : GetLogoCount
//  Description     : Get Number of Logo data in the table.
//  Input           : N/A
//  Return          : count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetLogoCount(void)
{
  if (gLogoTblFd == -1)
    return 0;

  return fLengthMW(gLogoTblFd) / sizeof(struct LOGO_TBL);
}
//*****************************************************************************
//  Function        : GetExParamCount
//  Description     : Get Number of Extra Param data in the table.
//  Input           : N/A
//  Return          : count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetExParamCount(void)
{
  if (gExParamFd == -1)
    return 0;

  return fLengthMW(gExParamFd) / sizeof(struct EXTRA_PARAM);
}
//*****************************************************************************
//  Function        : CloseSTISFiles
//  Description     : Close STIS files
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CloseSTISFiles(void)
{
  if (gTermCfgFd >= 0) {
    fCloseMW(gTermCfgFd);
    gTermCfgFd = -1;
  }
  if (gCrdTblFd >= 0) {
    fCloseMW(gCrdTblFd);
    gCrdTblFd = -1;
  }
  if (gIssTblFd >= 0) {
    fCloseMW(gIssTblFd);
    gIssTblFd = -1;
  }
  if (gAcqTblFd >= 0) {
    fCloseMW(gAcqTblFd);
    gAcqTblFd = -1;
  }
  if (gDscTblFd >= 0) {
    fCloseMW(gDscTblFd);
    gDscTblFd = -1;
  }
  if (gLogoTblFd >= 0) {
    fCloseMW(gLogoTblFd);
    gLogoTblFd = -1;
  }
}
//*****************************************************************************
//  Function        : CreateEmptyFile
//  Description     : Create Empty File
//  Input           : aFilename;      // file name.
//  Return          : file handle;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int CreateEmptyFile(const char *aFilename)
{
  char filename[256];
  int  file_handle;

  strcpy(filename, aFilename);
  fDeleteMW(filename);
  file_handle = fCreateMW(filename, 0);
  return file_handle;
}
//*****************************************************************************
//  Function        : CreateSTISFiles
//  Description     : Create STIS files
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CreateSTISFiles(void)
{
  char   filename[256], tmpbuf[MW_MAX_LINESIZE+1];
  struct TERM_CFG     term_cfg;

  // Delete & Create Default Files
  // Term Config
  gTermCfgFd = CreateEmptyFile(KTermCfgFile);
  if (gTermCfgFd != -1) {
    memcpy(&term_cfg, &KDefaultTermCfg, sizeof(struct TERM_CFG));
    term_cfg.w_crc = (WORD) cal_crc((BYTE *)&term_cfg, (BYTE *) &term_cfg.w_crc - (BYTE *)&term_cfg);
    if (fWriteMW(gTermCfgFd, &term_cfg, sizeof(struct TERM_CFG)) != sizeof(struct TERM_CFG)) {
      sprintf(tmpbuf, "CErr: %s", filename);
      DispLineMW(tmpbuf, MW_MAX_LINE, MW_REVERSE|MW_SPFONT);
    }
  }

  // Card Table
  gCrdTblFd = CreateEmptyFile(KCrdTblFile);

  // Issuer Table
  gIssTblFd = CreateEmptyFile(KIssTblFile);

  // Acquirer Table
  gAcqTblFd = CreateEmptyFile(KAcqTblFile);

  // Product Code Table
  gDscTblFd = CreateEmptyFile(KDscTblFile);

  // Logo Table
  gLogoTblFd = CreateEmptyFile(KLogoTblFile);
}
//*****************************************************************************
//  Function        : CloseExParamFiles
//  Description     : Close Extra Param files
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CloseExParamFiles(void)
{
  if (gExParamFd >= 0) {
    fCloseMW(gExParamFd);
    gExParamFd = -1;
  }
}
//*****************************************************************************
//  Function        : CreateExParamFiles
//  Description     : Create Extra Param files
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CreateExParamFiles(void)
{
  // Extra Param
  gExParamFd = CreateEmptyFile(KExParamFile);
}
//*****************************************************************************
//  Function        : STISInit
//  Description     : Open STIS parameter files.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void STISInit(void)
{
  char filename[256];

  // Init TermCfg File
  strcpy(filename, KTermCfgFile);
  gTermCfgFd = fOpenMW(filename);

  strcpy(filename, KCrdTblFile);
  gCrdTblFd = fOpenMW(filename);

  strcpy(filename, KIssTblFile);
  gIssTblFd = fOpenMW(filename);

  strcpy(filename, KAcqTblFile);
  gAcqTblFd = fOpenMW(filename);

  strcpy(filename, KDscTblFile);
  gDscTblFd = fOpenMW(filename);

  strcpy(filename, KLogoTblFile);
  gLogoTblFd = fOpenMW(filename);

  while ( (gTermCfgFd < 0) || (gCrdTblFd < 0) || (gIssTblFd < 0) ||
          (gAcqTblFd < 0)  || (gLogoTblFd < 0) ) {
    CloseSTISFiles();
    CreateSTISFiles();
  }
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : ExParamInit
//  Description     : Open Extra Param File.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ExParamInit(void)
{
  char filename[256];

  strcpy(filename, KExParamFile);
  gExParamFd = fOpenMW(filename);
  while (gExParamFd < 0) {
    CloseExParamFiles();
    CreateExParamFiles();
  }
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : GetTermCfg
//  Description     : Get Terminal Config Data.
//  Input           : aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetTermCfg(struct TERM_CFG *aDat)
{
  CheckPointerAddr(aDat);
  if (!ReadSFile(gTermCfgFd, (DWORD) gTermCfgOffset, aDat, sizeof(struct TERM_CFG)))
    return FALSE;

  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memcpy(aDat, &KDefaultTermCfg, sizeof(struct TERM_CFG));
  aDat->w_crc = (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat);
  return FALSE;
}
//*****************************************************************************
//  Function        : GetCardTbl
//  Description     : Get Card Table Data.
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetCardTbl(DWORD aIdx, struct CARD_TBL *aDat)
{
  CheckPointerAddr(aDat);
  if (!ReadSFile(gCrdTblFd, (DWORD) &gCrdTblOffset[aIdx], aDat, sizeof(struct CARD_TBL)))
    return FALSE;

  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memset(aDat, 0xFF, sizeof(struct CARD_TBL));
  return FALSE;
}
//*****************************************************************************
//  Function        : GetIssuerTbl
//  Description     : Get Issuer Table Data.
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetIssuerTbl(DWORD aIdx, struct ISSUER_TBL *aDat)
{
  CheckPointerAddr(aDat);
  if (!ReadSFile(gIssTblFd, (DWORD) &gIssTblOffset[aIdx], aDat, sizeof(struct ISSUER_TBL)))
    return FALSE;

  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memset(aDat, 0xFF, sizeof(struct ISSUER_TBL));
  return FALSE;
}
//*****************************************************************************
//  Function        : GetAcqTbl
//  Description     : Get Acquirer Table Data.
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetAcqTbl(DWORD aIdx, struct ACQUIRER_TBL *aDat)
{
  CheckPointerAddr(aDat);
  if (!ReadSFile(gAcqTblFd, (DWORD) &gAcqTblOffset[aIdx], aDat, sizeof(struct ACQUIRER_TBL)))
    return FALSE;

  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memset(aDat, 0xFF, sizeof(struct ACQUIRER_TBL));
  return FALSE;
}
//*****************************************************************************
//  Function        : GetDescTbl
//  Description     : Get Descriptor Table Data.
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetDescTbl(DWORD aIdx, struct DESC_TBL *aDat)
{
  CheckPointerAddr(aDat);
  if (!ReadSFile(gDscTblFd, (DWORD) &gDscTblOffset[aIdx], aDat, sizeof(struct DESC_TBL)))
    return FALSE;

  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memset(aDat, 0xFF, sizeof(struct DESC_TBL));
  return FALSE;
}
//*****************************************************************************
//  Function        : GetLogoTbl
//  Description     : Get Logo Table Data.
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetLogoTbl(DWORD aIdx, struct LOGO_TBL *aDat)
{
  CheckPointerAddr(aDat);
  if (!ReadSFile(gLogoTblFd, (DWORD) &gLogoTblOffset[aIdx], aDat, sizeof(struct LOGO_TBL)))
    return FALSE;

  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memset(aDat, 0xFF, sizeof(struct LOGO_TBL));
  return FALSE;
}
//*****************************************************************************
//  Function        : GetExtraParam
//  Description     : Get Extra Param Data.
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetExtraParam(DWORD aIdx, struct EXTRA_PARAM *aDat)
{
  CheckPointerAddr(aDat);
  if (!ReadSFile(gExParamFd, (DWORD) &gExParamOffset[aIdx], aDat, sizeof(struct EXTRA_PARAM)))
    return FALSE;

  if (aDat->w_crc == (WORD) cal_crc((BYTE *)aDat, (BYTE *) &aDat->w_crc - (BYTE *) aDat))
    return TRUE;

  memset(aDat, 0xFF, sizeof(struct EXTRA_PARAM));
  return FALSE;
}
//*****************************************************************************
//  Function        : TelToAscStr
//  Description     : convert tel string to asc string, eg "\x23\x2F\xFF" => "232"
//  Input           : aAscOut
//                    aBcdIn
//                    aPairNum
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void TelToAscStr(BYTE *aAscOut, BYTE *aBcdIn, WORD aPairNum)
{
  WORD i;

  split(aAscOut, aBcdIn, aPairNum);
  aAscOut[aPairNum*2] = 0x00;
  for (i=0; i<aPairNum*2; i++) {
    if (aAscOut[i] == 'F') {
      aAscOut[i] = 0x00;
      break;
    }
  }
}
//*****************************************************************************
//  Function        : PrintTermCfg
//  Description     : pritn or display terminal config
//  Input           : aTermCfg
//                    aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintTermCfg(struct TERM_CFG aTermCfg, BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;

  // allocate large memory
  if ((p_mem = (char *)MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // copy item by item
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  // detail report
  if (aDetail) {
    // name location
    strcpy(p_mem+len, "APN: ");
    memcpy(data, aTermCfg.apnconfig, 32);
    data[32] = 0x00;
    len += PackInfoData(p_mem+len, data, pack_width);
    strcpy(p_mem+len, "APN2: ");
    memcpy(data, aTermCfg.apnconfig2, 32);
    data[32] = 0x00;
    len += PackInfoData(p_mem+len, data, pack_width);

    // merch name
    strcpy(p_mem+len, "MercName: ");
    memcpy(data, aTermCfg.sb_dflt_name, 23);
    data[23] = 0x00;
    len += PackInfoData(p_mem+len, data, pack_width);

    // currency
    strcpy(p_mem+len, "Currency: ");
    SprintfMW(data, "%c %c%c%c", aTermCfg.b_currency, aTermCfg.sb_currency_name[0], aTermCfg.sb_currency_name[1], aTermCfg.sb_currency_name[2]);
    len += PackInfoData(p_mem+len, data, pack_width);

    // amount length
    strcpy(p_mem+len, "AmtLen: ");
    SprintfMW(data, "%02X", aTermCfg.b_trans_amount_len);
    len += PackInfoData(p_mem+len, data, pack_width);

    // decimal position
    strcpy(p_mem+len, "DecimalPos: ");
    SprintfMW(data, "%02X", aTermCfg.b_decimal_pos);
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // reserve data
  strcpy(p_mem+len, "Reserve: ");
  len += PackInfoData(p_mem+len, "", pack_width);
  for (i=0; i<3; i++) {
    split(data, aTermCfg.sb_reserved+i*10, 10);
    data[20] = 0x00;
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // setup data info
  strcpy(data, " Term Cfg");    // header
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (aToDisplay) {
    if (DispInfoData(info_data) == MWKEY_CANCL) {
      ret_ok = FALSE;
    }
  }
  else  {
    PrintRcptLF(1);
    LptPutS(&data[1]);
    PrintRcptLF(1);
    ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintAcqTbl
//  Description     : pritn or display acquirer table
//  Input           : aAcqTbl
//                    aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintAcqTbl(struct ACQUIRER_TBL aAcqTbl, BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;

  // allocate large memory
  if ((p_mem = (char *)MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  // id & nii
  strcpy(p_mem+len, "ID&NII: ");
  SprintfMW(data, "%02X %01d%02X", aAcqTbl.b_id, aAcqTbl.sb_nii[0]&0x0F, aAcqTbl.sb_nii[1]);
  len += PackInfoData(p_mem+len, data, pack_width);

  // program & name
  memcpy(p_mem+len, aAcqTbl.sb_program, sizeof(aAcqTbl.sb_program));
  p_mem[len+sizeof(aAcqTbl.sb_program)] = 0x00;
  memcpy(data, aAcqTbl.sb_name, sizeof(aAcqTbl.sb_name));
  data[sizeof(aAcqTbl.sb_name)] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // terminal id
  strcpy(p_mem+len, "TermID: ");
  memcpy(data, aAcqTbl.sb_term_id, sizeof(aAcqTbl.sb_term_id));
  data[sizeof(aAcqTbl.sb_term_id)] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // merchant id
  strcpy(p_mem+len, "MercID: ");
  memcpy(data, aAcqTbl.sb_acceptor_id, sizeof(aAcqTbl.sb_acceptor_id));
  data[sizeof(aAcqTbl.sb_acceptor_id)] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // trans tel
  strcpy(p_mem+len, "TransTel: ");
  len += PackInfoData(p_mem+len, "", pack_width);
  TelToAscStr(data, aAcqTbl.sb_pri_trans_tel, sizeof(aAcqTbl.sb_pri_trans_tel));
  len += PackInfoData(p_mem+len, data, pack_width);
  TelToAscStr(data, aAcqTbl.sb_sec_trans_tel, sizeof(aAcqTbl.sb_sec_trans_tel));
  len += PackInfoData(p_mem+len, data, pack_width);
  SprintfMW(p_mem+len, "PriConn:%02X", aAcqTbl.b_pri_trans_conn_time);
  SprintfMW(data, "SecConn:%02X", aAcqTbl.b_sec_trans_conn_time);
  len += PackInfoData(p_mem+len, data, pack_width);

  // settle tel
  strcpy(p_mem+len, "SettleTel: ");
  len += PackInfoData(p_mem+len, "", pack_width);
  TelToAscStr(data, aAcqTbl.sb_pri_settle_tel, sizeof(aAcqTbl.sb_pri_settle_tel));
  len += PackInfoData(p_mem+len, data, pack_width);
  TelToAscStr(data, aAcqTbl.sb_sec_settle_tel, sizeof(aAcqTbl.sb_sec_settle_tel));
  len += PackInfoData(p_mem+len, data, pack_width);
  SprintfMW(p_mem+len, "PriConn:%02X", aAcqTbl.b_pri_settle_conn_time);
  SprintfMW(data, "SecConn:%02X", aAcqTbl.b_sec_settle_conn_time);
  len += PackInfoData(p_mem+len, data, pack_width);

  // response time
  strcpy(p_mem+len, "RespTime: ");
  SprintfMW(data, "%02X", aAcqTbl.b_timeout_val);
  len += PackInfoData(p_mem+len, data, pack_width);

  // detail report
  if (aDetail) {
    // reserve data
    strcpy(p_mem+len, "Reserve: ");
    len += PackInfoData(p_mem+len, "", pack_width);
    for (i=0; i<3; i++) {
      split(data, aAcqTbl.sb_reserved+i*10, 10);
      data[20] = 0x00;
      len += PackInfoData(p_mem+len, data, pack_width);
    }
    // visa1 term id
    strcpy(p_mem+len, "V1 ID: ");
    memcpy(data, aAcqTbl.sb_visa1_term_id, sizeof(aAcqTbl.sb_visa1_term_id));
    data[sizeof(aAcqTbl.sb_visa1_term_id)] = 0x00;
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // setup data info
  SprintfMW(data, " ACQ %02X", aAcqTbl.b_id);
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (aToDisplay) {
    if (DispInfoData(info_data) == MWKEY_CANCL) {
      ret_ok = FALSE;
    }
  }
  else  {
    PrintRcptLF(1);
    LptPutS(&data[1]);
    PrintRcptLF(1);
    ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintIssuerTbl
//  Description     : pritn or display issuer table
//  Input           : aIssuerTbl
//                    aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintIssuerTbl(struct ISSUER_TBL aIssuerTbl, BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;

  // allocate large memory
  if ((p_mem = (char *)MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  // card name
  strcpy(p_mem+len, "CardName: ");
  memcpy(data, aIssuerTbl.sb_card_name, sizeof(aIssuerTbl.sb_card_name));
  data[sizeof(aIssuerTbl.sb_card_name)] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // floor limit
  strcpy(p_mem+len, "FLR LMT: ");
  SprintfMW(data, "%02X%02X", aIssuerTbl.sb_floor_limit[0], aIssuerTbl.sb_floor_limit[1]);
  len += PackInfoData(p_mem+len, data, pack_width);

  // c-option
  strcpy(p_mem+len, "C_OPT: ");
  data[0] = '0';  // account select
  data[1] = (aIssuerTbl.sb_options[0]&0x02)? '1': '0'; // pin entry
  data[2] = (aIssuerTbl.sb_options[0]&0x04)? '1': '0'; // manual pan
  data[3] = (aIssuerTbl.sb_options[1]&0x10)? '1': '0'; // check exp date
  data[4] = (aIssuerTbl.sb_options[1]&0x08)? '1': '0'; // capture trans
  data[5] = (aIssuerTbl.sb_options[0]&0x10)? '1': '0'; // offline entry allowed
  data[6] = (aIssuerTbl.sb_options[0]&0x20)? '1': '0'; // voice referral allowed
  data[7] = (aIssuerTbl.sb_options[0]&0x40)? '1': '0'; // product code req.
  data[8] = (aIssuerTbl.sb_options[0]&0x80)? '1': '0'; // adjust allowed
  data[9] = (aIssuerTbl.sb_options[1]&0x01)? '1': '0'; // check PAN
  data[10] = (aIssuerTbl.sb_options[1]&0x02)? '1': '0'; // ECR reference req.
  data[11] = '0'; // cash?
  data[12] = '0'; // mac?
  data[13] = (aIssuerTbl.sb_options[1]&0x04)? '1': '0'; // Print receipt
  data[14] = (aIssuerTbl.sb_options[1]&0x80)? '1': '0'; // auth block
  data[15] = (aIssuerTbl.sb_options[1]&0x40)? '1': '0'; // refund block
  data[16] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // reserve data
  strcpy(p_mem+len, "Reserve: ");
  len += PackInfoData(p_mem+len, "", pack_width);
  for (i=0; i<2; i++) {
    split(data, aIssuerTbl.sb_reserved+i*10, 10);
    data[20] = 0x00;
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // setup data info
  SprintfMW(data, " ISSUER %02X", aIssuerTbl.b_id);
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (aToDisplay) {
    if (DispInfoData(info_data) == MWKEY_CANCL) {
      ret_ok = FALSE;
    }
  }
  else  {
    PrintRcptLF(1);
    LptPutS(&data[1]);
    PrintRcptLF(1);
    ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintCardTbl
//  Description     : pritn or display card table
//  Input           : aCardTbl
//                    aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintCardTbl(struct CARD_TBL aCardTbl, BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;

  // allocate large memory
  if ((p_mem = (char *)MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  // card range
  strcpy(p_mem+len, "From: ");
  split(data, aCardTbl.sb_pan_range_low, sizeof(aCardTbl.sb_pan_range_low));
  data[sizeof(aCardTbl.sb_pan_range_low)*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);
  strcpy(p_mem+len, "To: ");
  split(data, aCardTbl.sb_pan_range_high, sizeof(aCardTbl.sb_pan_range_high));
  data[sizeof(aCardTbl.sb_pan_range_high)*2] = 0x00;
  len += PackInfoData(p_mem+len, data, pack_width);

  // reserve data
  strcpy(p_mem+len, "Reserve: ");
  len += PackInfoData(p_mem+len, "", pack_width);
  for (i=0; i<2; i++) {
    split(data, aCardTbl.sb_reserved+i*10, 10);
    data[20] = 0x00;
    len += PackInfoData(p_mem+len, data, pack_width);
  }

  // setup data info
  SprintfMW(data, " CARD TBL");
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (aToDisplay) {
    if (DispInfoData(info_data) == MWKEY_CANCL) {
      ret_ok = FALSE;
    }
  }
  else  {
    PrintRcptLF(1);
    LptPutS(&data[1]);
    PrintRcptLF(1);
    ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintDescTbl
//  Description     : pritn or display descriptor (product code) table
//  Input           : aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintDescTbl(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;
  struct DESC_TBL desc_tbl;
  int desc_count;

  // allocate large memory
  if ((p_mem = (char *)MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  // get descriptor one by one
  desc_count = GetDescCount();
  for (i=0; i<desc_count; i++) {
    // get descriptor nok
    if (!GetDescTbl(i, &desc_tbl)) {
      break;
    }
    if (desc_tbl.b_key == 0xFF)
      break;

    // pack info data
    p_mem[len] = desc_tbl.b_key;
    p_mem[len+1] = ' ';
    p_mem[len+2] = desc_tbl.sb_host_tx_code[0];
    p_mem[len+3] = desc_tbl.sb_host_tx_code[1];
    p_mem[len+4] = ' ';
    memcpy(p_mem+len+5, desc_tbl.sb_text, sizeof(desc_tbl.sb_text));
    p_mem[len+5+sizeof(desc_tbl.sb_text)] = 0x00;
    len += PackInfoData(p_mem+len, "", pack_width);
  }

  // setup data info
  SprintfMW(data, " PRODUCT CODE");
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (len>0) {
    if (aToDisplay) {
      if (DispInfoData(info_data) == MWKEY_CANCL) {
        ret_ok = FALSE;
      }
    }
    else  {
      PrintRcptLF(2);
      LptPutS(&data[1]);
      PrintRcptLF(1);
      ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
    }
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintExtraParam
//  Description     : pritn or display extra parameter
//  Input           : aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN PrintExtraParam(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  char *p_mem, data[64];
  WORD i, len, pack_width;
  struct INFO_DATA info_data;
  struct EXTRA_PARAM extra_param;
  int exparam_count;

  // allocate large memory
  if ((p_mem = (char *)MallocMW(1024)) == NULL) {
    return FALSE;
  }

  // init
  len = 0;
  pack_width = (aToDisplay)? MW_MAX_LINESIZE : PRINT_WIDTH;

  // get descriptor one by one
  exparam_count = GetExParamCount();
  for (i=0; i<exparam_count && ret_ok; i++) {
    // get descriptor nok
    if (!GetExtraParam(i, &extra_param) || (extra_param.b_len>sizeof(extra_param.sb_content))) {
      break;
    }
    if (extra_param.b_app_id == 0xFF) {
      break;
    }
    // set description line
    if ((i==0) || ((len==0)&&(aToDisplay))) {
      strcpy(p_mem+len, "ID SQ LN Value ");
      len += PackInfoData(p_mem+len, "", pack_width);
    }
    // pack info data
    SprintfMW(p_mem+len, "%02X %02X %02d ", extra_param.b_app_id, extra_param.b_seq_no, extra_param.b_len);
    // hex = bit0 enabled, sensitive data = bit1 enabled
    if (extra_param.b_prefix & 0x02) {
      memset(p_mem+len+9, '*', extra_param.b_len);
      p_mem[len+9+extra_param.b_len] = 0x00;
    }
    else if (extra_param.b_prefix & 0x01) {
      split(p_mem+len+9, extra_param.sb_content, extra_param.b_len);
      p_mem[len+9+extra_param.b_len*2] = 0x00;
    }
    else {
      memcpy(p_mem+len+9, extra_param.sb_content, extra_param.b_len);
      p_mem[len+9+extra_param.b_len] = 0x00;
    }
    len += PackInfoData(p_mem+len, "", pack_width);

    // display or print first, to prevent p_mem overfall

//    if (!aToDisplay) {
//      SprintfMW(data, "EXTRA PARAM");
//      PrintRcptLF(1);
//      LptPutS(data);
//      PrintRcptLF(1);
//    }

    if (len>960) {
      // setup data info
      SprintfMW(data, " EXTRA PARAM >");
      info_data.b_line_width  = (unsigned char)pack_width;
      info_data.w_info_len    = len;
      info_data.pb_header     = data;
      info_data.pb_info       = p_mem;
      len = 0;    // reset len
      if (aToDisplay) {
        if (DispInfoData(info_data) == MWKEY_CANCL) {
          ret_ok = FALSE;
          break;
        }
      }
      else  {
        PrintRcptLF(2);
        LptPutS(data);
        PrintRcptLF(1);
        ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
      }
    }
  }

  // setup data info
  SprintfMW(data, " EXTRA PARAM");
  info_data.b_line_width  = (unsigned char)pack_width;
  info_data.w_info_len    = len;
  info_data.pb_header     = data;
  info_data.pb_info       = p_mem;

  if (len && ret_ok) {
    if (aToDisplay) {
      if (DispInfoData(info_data) == MWKEY_CANCL) {
        ret_ok = FALSE;
      }
    }
    else  {
      PrintRcptLF(2);
      LptPutS(data);
      PrintRcptLF(1);
      ret_ok = PrintBuf(info_data.pb_info, info_data.w_info_len, TRUE);
    }
  }

  FreeMW(p_mem);
  return ret_ok;
}
//*****************************************************************************
//  Function        : PrintSTISParam
//  Description     : print or display STIS data
//  Input           : aDetail
//                    aToDisplay
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PrintSTISParam(BOOLEAN aDetail, BOOLEAN aToDisplay)
{
  BOOLEAN ret_ok = TRUE;
  struct TERM_CFG term_cfg;
  struct ACQUIRER_TBL acq_tbl;
  struct ISSUER_TBL issuer_tbl;
  struct CARD_TBL card_tbl;
  int acq_idx, issuer_idx, card_idx;
  int prev_issuer_idx = -1;
  int card_count, issuer_count, acq_count;

  if (GetTermCfg(&term_cfg)) {
    ret_ok = PrintTermCfg(term_cfg, aDetail, aToDisplay);
  }

  acq_count = GetAcqCount();
  for (acq_idx=0; acq_idx<acq_count && ret_ok; acq_idx++) {
    // get acq nok
    if (!GetAcqTbl(acq_idx, &acq_tbl)) {
      break;
    }

    if (acq_tbl.b_id == 0xFF)
      break;

    // print acquirer info
    ret_ok = PrintAcqTbl(acq_tbl, aDetail, aToDisplay);
    if (!ret_ok) break;

    // search related card and issuer info
    card_count = GetCardCount();
    for (card_idx=0; card_idx<card_count && ret_ok; card_idx++) {
      // get card tbl nok
      if (!GetCardTbl(card_idx, &card_tbl)) {
        break;
      }
      if (card_tbl.b_acquirer_id == 0xFF)
        break;

      // not correct acqurier, find next
      if (card_tbl.b_acquirer_id != acq_tbl.b_id) {
        continue;
      }

      // get related issuer
      issuer_count = GetIssuerCount();
      for (issuer_idx=0; issuer_idx<issuer_count && ret_ok; issuer_idx++) {
        // get issuer tbl nok
        if (!GetIssuerTbl(issuer_idx, &issuer_tbl)) {
          issuer_idx = issuer_count;  // ignore print issuer info
          break;
        }
        if (issuer_tbl.b_id == 0xFF) {
          issuer_idx = issuer_count;  // ignore print issuer info
          break;
        }
        // correct issuer, exit
        if (card_tbl.b_issuer_id == issuer_tbl.b_id) {
          break;
        }
      }

      // print issuer info
      if ((prev_issuer_idx != issuer_idx) && (issuer_idx < issuer_count)) {
        ret_ok = PrintIssuerTbl(issuer_tbl, aDetail, aToDisplay);
        if (!ret_ok) break;
      }

      // print card info
      ret_ok = PrintCardTbl(card_tbl, aDetail, aToDisplay);
      if (!ret_ok) break;
    }
  }

  // print product code
  if (ret_ok) {
    ret_ok = PrintDescTbl(aDetail, aToDisplay);
  }

  // print extra param
  if (ret_ok) {
    ret_ok = PrintExtraParam(aDetail, aToDisplay);
  }

  return ret_ok;
}
//*****************************************************************************
//  Function        : GetPending
//  Description     : Get Acquirer Pending Flag.
//  Input           : Idx;         // Acquirer Table Index
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE GetPending(int aIdx)
{
  struct ACQUIRER_TBL acq_tbl;

  if (!GetAcqTbl(aIdx, &acq_tbl))
    return NOT_LOADED;

  return acq_tbl.b_pending;
}
//*****************************************************************************
//  Function        : SetPending
//  Description     : Set Acquirer Pending Flag.
//  Input           : aIdx;         // Table Index , -1 => All Table
//                    aVal;         // Flag
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetPending(int aIdx, BYTE aVal)
{
  struct ACQUIRER_TBL acq_tbl;
  int start_idx, end_idx;
  int acq_count = GetAcqCount();

  if (aIdx >= acq_count)
    return;

  if (aIdx == -1) {
    start_idx = 0;
    end_idx   = acq_count-1;
  }
  else {
    start_idx = aIdx;
    end_idx   = aIdx;
  }

  for (aIdx = start_idx; aIdx <= end_idx; aIdx++) {
    if (!GetAcqTbl(aIdx, &acq_tbl))
      return;
    acq_tbl.b_pending = aVal;
    UpdAcqTbl(aIdx, &acq_tbl);
  }
}
//*****************************************************************************
//  Function        : GetPending
//  Description     : Get Acquirer Pending Flag.
//  Input           : aIdx;         // Acquirer Table Index
//                    aVal;         // pointer to batch no.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetBatchNo(DWORD aIdx, BYTE *aVal)
{
  struct ACQUIRER_TBL acq_tbl;

  CheckPointerAddr(aVal);
  if (!GetAcqTbl(aIdx, &acq_tbl))
    return FALSE;
  memcpy(aVal, acq_tbl.sb_curr_batch_no, 6);
  return TRUE;
}
//*****************************************************************************
//  Function        : GetPending
//  Description     : Get Acquirer Pending Flag.
//  Input           : aIdx;         // Table Index
//                    aVal;         // Pointer to Current & Next Bat# (6 bytes)
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetBatchNo(DWORD aIdx, BYTE *aVal)
{
  struct ACQUIRER_TBL acq_tbl;

  CheckPointerAddr(aVal);
  if (!GetAcqTbl(aIdx, &acq_tbl))
    return;
  memcpy(acq_tbl.sb_curr_batch_no, aVal, 6);
  UpdAcqTbl(aIdx, &acq_tbl);
}
//*****************************************************************************
//  Function        : CleanStisTables
//  Description     : Reset all STIS table.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CleanStisTable(void)
{
  fCommitAllMW();
  CloseSTISFiles();
  CloseExParamFiles();

  CreateSTISFiles();
  CreateExParamFiles();
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : StisAdd
//  Description     : Add record to STIS table
//  Input           : aTableId;     // Table ID
//                    aDat;         // pointer to table data.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD StisAdd(int aTableId, void *aDat)
{
  int idx;
  DWORD ret;

  CheckPointerAddr(aDat);
  switch (aTableId) {
    case APM_TERM_CFG       :
      ret = UpdTermCfg(aDat);
      break;
    case APM_CARD_TBL       :
      idx = GetCardCount();
      ret = UpdCardTbl(idx, aDat);
      break;
    case APM_ISSUER_TBL     :
      idx = GetIssuerCount();
      ret = UpdIssuerTbl(idx, aDat);
      break;
    case APM_ACQUIRER_TBL   :
      idx = GetAcqCount();
      ret = UpdAcqTbl(idx, aDat);
      break;
    case APM_DESC_TBL       :
      idx = GetDescCount();
      ret = UpdDescTbl(idx, aDat);
      break;
    case APM_LOGO_TBL       :
      idx = GetLogoCount();
      ret = UpdLogoTbl(idx, aDat);
      break;
    default :
      ret = FALSE;
  }
  return ret;
}
//*****************************************************************************
//  Function        : SelectAcquirer
//  Description     : Build acquirer list and prompt user to select.
//  Input           : aInclAll;     // 1 => Include all option
//  Return          : -1;           // Cancel;
//                    others;       // Acquirer ID.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int SelectAcquirer(int aInclAll)
{
  int i, j, ret_val;
  struct ACQUIRER_TBL acq_tbl;
  struct MENU_ITEM *SelAcqItem;
  struct MENU_DAT  SelAcqMenu;
  int acq_count;

  // Check Acquirer Count
  acq_count = fLengthMW(gAcqTblFd) / sizeof(struct ACQUIRER_TBL);
  if (acq_count == 0)
    return -1;

  ret_val = -1;
  SelAcqItem = (struct MENU_ITEM *) MallocMW(sizeof(struct MENU_ITEM) * (acq_count + 2)); // +all + record end
  if (SelAcqItem != NULL) {
    i = 0;
    if (aInclAll) {
      SelAcqItem[i].iID = 0;
      SelAcqItem[i].pcDesc = NULL;
      memset(SelAcqItem[i].scDescDat, ' ', sizeof(SelAcqItem[i].scDescDat));
      memcpy(SelAcqItem[i].scDescDat, "All", 3);
      i++;
    }
    for (j = 0; j < acq_count; j++) {
      if (!GetAcqTbl(j, &acq_tbl))
        break;
      if (acq_tbl.b_status == NOT_LOADED)
        break;

      SelAcqItem[i].iID = j+1;
      SelAcqItem[i].pcDesc = NULL;
      memset(SelAcqItem[i].scDescDat, ' ', sizeof(SelAcqItem[i].scDescDat));
      memcpy(SelAcqItem[i].scDescDat, acq_tbl.sb_name, 10);
      i++;
    }
    SelAcqItem[i].iID = -1;
    SelAcqItem[i].pcDesc = NULL;

    if (j > 0) { // Some Acquirer loaded
      // Menu Setup
      strcpy(SelAcqMenu.scHeader, KSelAcquirer);
      SelAcqMenu.psMenuItem = SelAcqItem;
      i  = MenuSelect(&SelAcqMenu, 0);
      if (aInclAll && (i == 0))
        ret_val = -2;
      else
        if (i > 0)
          ret_val = i - 1;
    }
    FreeMW(SelAcqItem);
  }
  return ret_val;
}
//*****************************************************************************
//  Function        : ExtraMsgLen
//  Description     : Check config for extra message len byte in host msg.
//  Input           : aIdx;          // table index
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ExtraMsgLen(int aIdx)
{
  struct ACQUIRER_TBL acq_tbl;

  if (!GetAcqTbl(aIdx, &acq_tbl))
    return FALSE;

  return (acq_tbl.b_reserved1? TRUE: FALSE);
}
