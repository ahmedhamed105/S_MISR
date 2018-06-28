//-----------------------------------------------------------------------------
//  File          : phone.c
//  Module        :
//  Description   : Include Feature phone handling routine.
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
//  07 Jan  2010  Lewis       Add volume adjust during offhook mode.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if (R700)
//-----------------------------------------------------------------------------
#include "midware.h"
#include "util.h"
#include "sysutil.h"
#include "message.h"
#include "kbdutil.h"
#include "ringin.h"
#include "phlist.h"
#include "toggle.h"
#include "beep.h"
#include "mission3.h"
#include "phone.h"


//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
struct LAST_DIAL {
  WORD wLen;
  BYTE sbDigit[256];
}; 

#define MAX_HIST_DATA       20

struct CALL_HIST {
  struct PHONE_BOOK sPhoneBK;
  BYTE   sbDateTime[8];
};

//#define MAX_RECORD_SIZE   (24*1024)
#define MAX_RECORD_SIZE   (sizeof(KMission3))

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
static int  gBLCtrl=-1;
static BYTE *gRingIn=NULL;
static BYTE *gBeep=NULL;
static struct PHONE_CFG gPhoneCfg;
static struct LAST_DIAL gLastDial;
static struct CALL_HIST gCallHist[MAX_HIST_BOOK][MAX_HIST_DATA];
//static int    gDevHandle[APM_SDEV_MDM] = -1;
extern int gDevHandle[APM_SDEV_MDM];   // define in hardware.c

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
#define MAX_LEVEL           8
#define DEF_SPGAIN          128
#define SPGAIN_LEVEL        (15)
#define MICGAIN_LEVEL       (4)

static const DWORD KGainVal[MAX_GAIN_IDX] = { 6, 8, 1, 4 };

static const struct LIST_ITEM KListItem[] = {
   //12345678901234567
  { "CID ON/OFF       " },
  { "TAM ON/OFF       " },
  { "TAM Ring Count   " },
  { "OGM Record       " },
  { "OGM Playback     " },
  { "OGM Delete       " },
  { "Set Volume       " },
  { "Restore Default  " },
};
static struct LIST_DAT KSetupMenu = {
  (sizeof(KListItem)/sizeof(struct LIST_ITEM)),
  { "   Phone Setup   " },
  KListItem
};

// Filename
static BYTE KOGMName[] = { "OGM" };
static BYTE KCFGName[] = { "Phone" };

//static const struct CALL_HIST KCallHist[MAX_HIST_DATA] = {
//  {{ "Test1           ", "918013\x00      "}, "09-10-03" },
//  {{ "\x00               ", "18013\x00       "}, "09-10-02" },
//  {{ "Test2           ", "918033\x00      "}, "09-10-02" },
//  {{ "Test3           ", "18033\x00       "}, "09-10-02" },
//  {{ "Test4           ", "12345678        "}, "09-10-03" },
//  {{ "\x00               ", "918033\x00      "}, "09-10-02" },
//  {{ "Test5           ", "12345678        "}, "09-10-02" },
//  {{ "Test6           ", "12345678        "}, "09-10-02" },
//  {{ "Test7           ", "12345678        "}, "09-10-03" },
//  {{ "\x00               ", "12345678        "}, "09-10-02" },
//  {{ "Test8           ", "12345678        "}, "09-10-02" },
//  {{ "Test9           ", "12345678        "}, "09-10-02" },
//  {{ "Test10          ", "12345678        "}, "09-10-03" },
//  {{ "\x00               ", "12345678        "}, "09-10-02" },
//  {{ "Test12          ", "12345678        "}, "09-10-02" },
//  {{ "Test13          ", "12345678        "}, "09-10-02" },
//  {{ "Test14          ", "12345678        "}, "09-10-03" },
//  {{ "\x00               ", "12345678        "}, "09-10-02" },
//  {{ "Test15          ", "12345678        "}, "09-10-02" },
//  {{ "Test16          ", "12345678        "}, "09-10-02" },
//};

//*****************************************************************************
//  Function        : ReadPhoneCfg
//  Description     : Read Phone Config File.
//  Input           : aCfg;     // pointer to result buffer.
//  Return          : TRUE/FALSE;       // FALSE=> Read Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN ReadPhoneCfg(struct PHONE_CFG *aCfg)
{
  int i;

  i = os_file_open(KCFGName, K_O_RDONLY);
  if ((i >= 0) && (os_file_length(i)== sizeof(struct PHONE_CFG))) {
    os_file_read(i, aCfg, sizeof(struct PHONE_CFG));
    os_file_close(i);
    return TRUE;
  }
  DispLineMW("Read Err: CFG", MW_MAX_LINE, MW_REVERSE|MW_CENTER|MW_SPFONT);
  Delay10ms(100);
  return FALSE;
}
//*****************************************************************************
//  Function        : SavePhoneCfg
//  Description     : Save Phone Config File.
//  Input           : aCfg;     // pointer config data
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SavePhoneCfg(struct PHONE_CFG *aCfg)
{
  int i;

  i = os_file_open(KCFGName, K_O_CREAT|K_O_RDWR|K_O_TRUNC);
  if (i >= 0) {
    os_file_write(i, &gPhoneCfg, sizeof(struct PHONE_CFG));
    os_file_close(i);
  }
}
//*****************************************************************************
//  Function        : SetDefaultCfg
//  Description     : Set Default Phone Config.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SetDefaultCfg(struct PHONE_CFG *aPhoneCfg)
{
  int fd;

  memset(aPhoneCfg, 0, sizeof(struct PHONE_CFG));
  aPhoneCfg->bCallerIDEnb = TRUE;
  aPhoneCfg->bAutoAnsEnb  = TRUE;
  aPhoneCfg->bAutoAnsRing = 1;
  aPhoneCfg->dwVMailCount = 0;
  memset(aPhoneCfg->sbVMailFile, 0, sizeof(gPhoneCfg.sbVMailFile));
  memcpy(aPhoneCfg->sdwGainVal, KGainVal, sizeof(aPhoneCfg->sdwGainVal));

  // Save Default OGM
  fd = os_file_open(KOGMName, K_O_CREAT|K_O_TRUNC|K_O_RDWR);
  if (fd >= 0) {
    os_file_write(fd, KMission3, sizeof(KMission3));
    os_file_close(fd);
  }
}
//*****************************************************************************
//  Function        : SetSpeakerVolume
//  Description     : Set Speaker volume, base on the input value.
//  Input           : N/A
//  Return          : N/A
//  Note            : Speaker Phone : range 0 to 120
//                    Hand Set      : range 135 to 255
//  Globals Changed : N/A
//*****************************************************************************
void SetSpeakerVolume(void)
{
  DWORD status = bs_phone_status(K_MdmStatus);
  DWORD gain;

  gain = (BYTE)(DEF_SPGAIN+gPhoneCfg.sdwGainVal[SPEAKER_GAIN]*SPGAIN_LEVEL);
  if ((status == K_VoiceHs) || (status == K_VoiceFrSpPh))
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPR_GAIN, gain);
  else
  if ((status == K_VoiceSpPh) || (status == K_VoiceSpPhFrHs)) 
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPR_GAIN, gain); 
}
//*****************************************************************************
//  Function        : SetAllVolume
//  Description     : Set all gains values.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SetAllVolume(void)
{
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPR_GAIN, (BYTE)(DEF_SPGAIN+gPhoneCfg.sdwGainVal[SPEAKER_GAIN]*SPGAIN_LEVEL)); 
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_MPH_GAIN, (BYTE)(gPhoneCfg.sdwGainVal[MIC_GAIN]*MICGAIN_LEVEL)); 
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PB_GAIN,  (BYTE)(DEF_SPGAIN+gPhoneCfg.sdwGainVal[PLAYBACK_GAIN]*SPGAIN_LEVEL)); 
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_REC_GAIN, (BYTE)(DEF_SPGAIN+gPhoneCfg.sdwGainVal[RECORD_GAIN]*SPGAIN_LEVEL)); 
}
//*****************************************************************************
//  Function        : PhoneInit
//  Description     : Initialize for feature phone.
//  Input           : aCtrl;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PhoneInit(void)
{
  BYTE lcd_timeout;
  int i;
  char filename[32];

  if (gDevHandle[APM_SDEV_MDM] < 0) {
    strcpy(filename, DEV_MDM1);
    gDevHandle[APM_SDEV_MDM] = OpenMW(filename, MW_RDWR);
  }
  i = 40;
  while (!(StatMW(gDevHandle[APM_SDEV_MDM], 0, NULL)&MW_LINE_READY) && i>0) {
     Delay10ms(10);
     i--;
  }
  gRingIn = MallocMW(sizeof(KRingIn));
  if (gRingIn != NULL) {
    memcpy(gRingIn, KRingIn, sizeof(KRingIn));
  }
  os_config_read(K_CF_LcdTimeout, &lcd_timeout);
  gBLCtrl = lcd_timeout < 30? 30: lcd_timeout;

  if (!ReadPhoneCfg(&gPhoneCfg)) {
    SetDefaultCfg(&gPhoneCfg);
    SavePhoneCfg(&gPhoneCfg);
  }

  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SET_OPR_MODE, 21);  // set voice mode
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPKPH_STATE, 0);    // Speaker Phone Off
  if (gPhoneCfg.bCallerIDEnb)
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE, 1);    // Enable Caller ID
  else
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE, 0);    // Disable Caller ID

  if (gPhoneCfg.bAutoAnsEnb) {
    gBeep = MallocMW(sizeof(KBeep));
    if (gBeep != NULL) {
      memcpy(gBeep, KBeep, sizeof(KBeep));
    }
  }

  memset(&gLastDial, 0, sizeof(gLastDial));
  memset(&gCallHist, 0, sizeof(gCallHist));
  //for (i = 0; i <MAX_HIST_DATA; i++) {
  //  memcpy(&gCallHist[DIAL_IN][i], &KCallHist[i], sizeof(KCallHist[i]));
  //}
  SetAllVolume();
}
//*****************************************************************************
//  Function        : PhoneHouseKeep
//  Description     : Disable the feature phone function.
//  Input           : aCtrl;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PhoneHouseKeep(void)
{
  if (gBLCtrl != -1) {
    os_disp_bl_control(gBLCtrl);
  }

  if (gRingIn != NULL) {
    FreeMW(gRingIn);
    gRingIn = NULL;
  }

  if (gBeep != NULL) {
    FreeMW(gBeep);
    gBeep = NULL;
  }

  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPKPH_STATE , 0);     // Speaker Phone OFf
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE   , 0);     // Disable Caller ID
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SET_OPR_MODE, 22);    // Set Data Mode
  //CloseMW(gDevHandle[APM_SDEV_MDM]);  // always open
  //gDevHandle[APM_SDEV_MDM] = -1;
}
//*****************************************************************************
//  Function        : OffHookState
//  Description     : Reture TRUE if Phone is in Off Hook State.
//                    (Off Hook or Speaker Phone On)
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN OffHookState(void)
{
  DWORD status;

  status = IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_GET_STATUS, 0);
  if ((status == MWMDM_VoiceHs) || (status == MWMDM_VoiceFrSpPh) ||
      (status == MWMDM_VoiceSpPh) || (status == MWMDM_VoiceSpPhFrHs) ) {
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : UpdatePhoneDisp
//  Description     : Update Phone Display.
//  Input           : aFirstDigit;      // TRUE=>Wait for 1st digit entry.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void UpdatePhoneDisp(BOOLEAN aFirstDigit, BOOLEAN aCurDialOnly)
{
  struct CALL_HIST *call_hist;

  if (aFirstDigit == TRUE)
    DispPutStr("\x11\f");
  else
    PrintfMW("\x11\x0B\x05\x0B%16s", gLastDial.sbDigit);
  if (!aCurDialOnly) {
    DispClrBelowMW(MW_LINE4);
    DispPutStr("\x10\x1B\x40\x1C\x05\x1D\x12\x1B\x60Last Call:");
    if (memcmp(gCallHist[DIAL_IN]->sbDateTime, gCallHist[DIAL_OUT]->sbDateTime, 8) >= 0) 
      call_hist = &gCallHist[DIAL_IN][0];
    else 
      call_hist = &gCallHist[DIAL_OUT][0];
    DispPutNCMW(call_hist[0].sbDateTime, 8);
    DispPutStr("\x05\x11\x1B\x80");
    DispPutStr(call_hist->sPhoneBK.sbNumber);
    DispPutStr("\x05");
  }
}
//*****************************************************************************
//  Function        : MakeCall
//  Description     : Call the input number.
//  Input           : aDialStr;     // pointer to dial digit string.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void MakeCall(char *aDialStr)
{
  DWORD i, status;

  if (aDialStr == NULL)
    return;

  status = IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_GET_STATUS, 0);
  if (status == K_VoiceReady) {  // Not Off Hook
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPKPH_STATE, 1);  // Speaker Phone ON
    do {
     SleepMW();
     status = IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_GET_STATUS, 0);
     //PrintfMW("\x0E\x12\x1B\xE0%04X\x0F", status);
    } while (status != MWMDM_VoiceSpPh);
    Delay10ms(20);
  }
  if (OffHookState()) {
    for (i = 0; i < strlen(aDialStr); i++) {
      IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_DIALCHAR, aDialStr[i]);
    }
    SetSpeakerVolume();  // !2010-12-03 Set vol to avoid vol. reg change after AT Cmd
  }
}
//*****************************************************************************
//  Function        : CallHist
//  Description     : Perform Operation on Call History Book.
//  Input           : aPhoneBk;     // phone book index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void CallHist(DWORD aPhoneBk)
{
  struct CALL_HIST *call_hist;
  struct LIST_DAT hist_list;
  struct LIST_ITEM *list_item;
  int  no_of_item, i, len;

  if (aPhoneBk == DIAL_IN) {
    memcpy(hist_list.sbHeader, "     IN LIST     ", 17);
  }
  else {
    memcpy(hist_list.sbHeader, "     OUT LIST    ", 17);
  }

  // Check No of valid call history data.
  call_hist = gCallHist[aPhoneBk];
  for (no_of_item = 0; no_of_item < MAX_HIST_DATA; no_of_item++) {
    if (call_hist[no_of_item].sbDateTime[0] == 0)
      break;
  }

  list_item = (struct LIST_ITEM *)MallocMW(sizeof(struct LIST_ITEM) * no_of_item);
  if (list_item != NULL) {
    // pack item list
    for (i = 0; i < no_of_item; i++) {
      memset(list_item[i].sbDesc, ' ', sizeof(list_item[i].sbDesc));
      len = strlen(call_hist[i].sPhoneBK.sbName);
      len = len > 10 ? 10: len;
      memcpy(list_item[i].sbDesc, call_hist[i].sPhoneBK.sbName, len);
      memcpy(&list_item[i].sbDesc[11], &call_hist[i].sbDateTime, 8);
    }
    hist_list.psListItem  = list_item;
    hist_list.dwListCount = no_of_item;
    i = ListSelect(&hist_list);
    if (i != -1) {
      memset(gLastDial.sbDigit, 0, sizeof(gLastDial.sbDigit));
      strcpy(gLastDial.sbDigit, call_hist[i].sPhoneBK.sbNumber);
      gLastDial.wLen = strlen(call_hist[i].sPhoneBK.sbNumber);
      UpdatePhoneDisp(FALSE, FALSE);
      MakeCall(gLastDial.sbDigit);
    }
    FreeMW(list_item);
  }
}
//*****************************************************************************
//  Function        : Select1or2
//  Description     : Selection between 2 options.
//  Input           : N/A
//  Return          : 0:            // Cancel
//                    other;        // Select Id
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD Select1or2(const BYTE *aMsg1, const BYTE *aMsg2)
{
  BYTE select;
  DWORD keyin;

  select = 0;
  while (1) {
    DispPutStr("\x11\x1B\xC0\x05");
    if (select == 0)
      DispPutStr(aMsg1);
    else 
      DispPutStr(aMsg2);
    keyin = GetCharMW();
    if (keyin == MWKEY_CLR)
      select = (select+1)%2;
    if (keyin == MWKEY_ENTER)
      break;
    if (keyin == MWKEY_CANCL)
      return 0;
    SleepMW();
  }
  return (select+1);
}
//*****************************************************************************
//  Function        : EditPhoneItem
//  Description     : Edit the input Phone Item.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void EditPhoneItem(struct PHONE_BOOK *aPhoneItem)
{
  struct PHONE_BOOK rec;
  BYTE   tmp[32];

  memcpy(&rec, aPhoneItem, sizeof(rec));

  DispLineMW("PHONE BOOK", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  DispLineMW("Name :", MW_LINE5, MW_BIGFONT);
  tmp[0] = strlen(rec.sbName);
  memcpy(&tmp[1], rec.sbName, tmp[0]);
  if (!GetKbd(ALPHA_INPUT+MW_LINE7+ECHO+NULL_ENB, sizeof(rec.sbName)-1, tmp))
    return;
  memset(rec.sbName, 0, sizeof(rec.sbName));
  memcpy(rec.sbName, &tmp[1], tmp[0]);

  DispClrBelowMW(MW_LINE3);
  DispLineMW("Phone Number :", MW_LINE5, MW_BIGFONT);
  tmp[0] = strlen(rec.sbNumber);
  memcpy(&tmp[1], rec.sbNumber, tmp[0]);
  if (!GetKbd(ALPHA_INPUT+MW_LINE7+ECHO+NULL_ENB, sizeof(rec.sbNumber)-1, tmp))
    return;
  memset(rec.sbNumber, 0, sizeof(rec.sbNumber));
  memcpy(rec.sbNumber, &tmp[1], tmp[0]);

  memcpy(aPhoneItem, &rec, sizeof(rec));
}
//*****************************************************************************
//  Function        : PhoneBook
//  Description     : Phone Book operation.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void PhoneBook(void)
{
  static const BYTE KDial[] = {"\x1B\xC0""EDIT\x1B\xCC\x1C""DIAL\x1D"};
  static const BYTE KEdit[] = {"\x1B\xC0\x1C""EDIT\x1D\x1B\xCC""DIAL"};
  struct LIST_DAT phonebk_list;
  struct LIST_ITEM *list_item;
  int  i, len, select;

  memcpy(phonebk_list.sbHeader, "   PHONE  BOOK   ", 17);

  list_item = (struct LIST_ITEM *)MallocMW(sizeof(struct LIST_ITEM) * MAX_PB_ITEM);
  if (list_item != NULL) {
    phonebk_list.psListItem  = list_item;
    phonebk_list.dwListCount = MAX_PB_ITEM;
    do {
      // pack item list
      for (i = 0; i < MAX_PB_ITEM; i++) {
        memset(list_item[i].sbDesc, ' ', sizeof(list_item[i].sbDesc));
        len = strlen(gPhoneCfg.sPhoneBk[i].sbName);
        len = len > 10 ? 10: len;
        memcpy(list_item[i].sbDesc, gPhoneCfg.sPhoneBk[i].sbName, len);
        len = strlen(gPhoneCfg.sPhoneBk[i].sbNumber);
        len = len > 8 ? 8: len;
        memcpy(&list_item[i].sbDesc[11], gPhoneCfg.sPhoneBk[i].sbNumber, len);
      }
      i = ListSelect(&phonebk_list);
      if (i != -1) {
        if (strlen(gPhoneCfg.sPhoneBk[i].sbName) != 0) 
          PrintfMW("\x11\f\x1B\x40%s", gPhoneCfg.sPhoneBk[i].sbName);
        else
          PrintfMW("\x11\f\x1B\x40%s", gPhoneCfg.sPhoneBk[i].sbNumber);
        select = Select1or2(KDial, KEdit);
        if (select == 1) { // DialOut
          memset(gLastDial.sbDigit, 0, sizeof(gLastDial.sbDigit));
          strcpy(gLastDial.sbDigit, gPhoneCfg.sPhoneBk[i].sbNumber);
          gLastDial.wLen = strlen(gPhoneCfg.sPhoneBk[i].sbNumber);
          UpdatePhoneDisp(FALSE, FALSE);
          MakeCall(gLastDial.sbDigit);
          break;
        }
        else if (select == 2)  {  // Edit 
          EditPhoneItem(&gPhoneCfg.sPhoneBk[i]);
          SavePhoneCfg(&gPhoneCfg);
        }
        else  // Cancel
          break;
      }
    } while(i != -1);
    FreeMW(list_item);
  }
}
//*****************************************************************************
//  Function        : SetCallTime
//  Description     : Update the call time buf from system time.
//  Input           : aDat;     // pointer to 8 byte buffer.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SetCallTime(BYTE *aDat)
{
  BYTE tmp[15];

  RtcGetMW(tmp);
  memset(aDat, '/', 8);
  memcpy(&aDat[0], &tmp[2], 2);
  memcpy(&aDat[3], &tmp[4], 2);
  memcpy(&aDat[6], &tmp[6], 2);
}
//*****************************************************************************
//  Function        : ShowVolume
//  Description     : Visualize the input value value.
//  Input           : aRow;     // Show On Row
//                    aValue
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ShowVolume(DWORD aRow, DWORD aValue)
{
  //int i;
  //// set position and reverse display 
  //if (aRow == 0) 
  //  PrintfMW("\x0B\x1C"); 
  //else
  //  PrintfMW("\x1B%c\x1C", aRow); 


  //// Set Visual Value
  //for (i = 0; i < aValue; i++) DispPutNCMW("  ", 2);
  //DispPutCMW(MWREV_OFF);
  //// Fill remaining space
  //for (i = aValue; i < 8; i++) DispPutNCMW("  ", 2);
  //PrintfMW("%3d%%", (aValue * 100 + 4) / 8);
  BYTE tmp[MW_MAXCOLOR_LINESIZE+1];

  memset(tmp, ' ', sizeof(tmp));
  tmp[2*aValue] = 0;
  DispLineMW(tmp, aRow, MW_REVERSE|MW_SPFONT);
  sprintf(tmp, "%3d%%", (aValue * 100 + 4) / 8);
  DispLineMW(tmp, aRow, MW_RIGHT|MW_SPFONT);

}
//*****************************************************************************
//  Function        : AdjustSPKVolume
//  Description     : Adjust Speaker Volume
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void AdjustSPKVolume(void)
{
  BOOLEAN disp_chgd;
  int keyin, timer_hdl;

  disp_chgd = TRUE;
  timer_hdl = TimerOpenMW();
  TimerSetMW(timer_hdl, 200);  // 2 sec timeout
  while (TimerGetMW(timer_hdl)) {
    if (disp_chgd) {
      DispLineMW("Speaker", MW_LINE7, MW_SPFONT);
      ShowVolume(MW_LINE8, gPhoneCfg.sdwGainVal[SPEAKER_GAIN]);
      SetSpeakerVolume();
    }
    keyin = GetCharMW();
    if (keyin == MWKEY_CANCL)
      break;

    switch (keyin) {
      case MWKEY_CLR:
        gPhoneCfg.sdwGainVal[SPEAKER_GAIN] -= gPhoneCfg.sdwGainVal[SPEAKER_GAIN] > 0 ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_ENTER:
        gPhoneCfg.sdwGainVal[SPEAKER_GAIN] += gPhoneCfg.sdwGainVal[SPEAKER_GAIN] < MAX_LEVEL ? 1 : 0;
        disp_chgd = true;
        break;
    }

    if (keyin != 0) 
      TimerSetMW(timer_hdl, 200);
  }
  SavePhoneCfg(&gPhoneCfg);
  DispClrBelowMW(MW_LINE7);
}
//*****************************************************************************
//  Function        : PhoneFunc
//  Description     : Base Phone Functions
//  Input           : aCtrl;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PhoneFunc(DWORD aCtrl)
{
  BOOLEAN first_digit, disp_chgd;
  DWORD   keyin, state;

  SetSpeakerVolume();  // !2010-12-03 Set vol to avoid vol. reg change after AT Cmd
  first_digit = TRUE;
  disp_chgd   = TRUE;
  do {
    SleepMW();

    if (aCtrl != 0) {
      keyin = aCtrl;
      aCtrl = 0;
    }
    else
      keyin = GetCharMW();

    switch (keyin) {
      case MWKEY_RIGHT1: // Phone Book
        PhoneBook();
        first_digit = FALSE;
        break;
      case MWKEY_RIGHT2: // Incomming Call
        CallHist(DIAL_IN);
        first_digit = FALSE;
        break;
      case MWKEY_RIGHT3: // OutGoing Call
        CallHist(DIAL_OUT);
        first_digit = FALSE;
        break;
      case MWKEY_RIGHT4: // Redial
        UpdatePhoneDisp(FALSE, TRUE);
        MakeCall(gLastDial.sbDigit);
        break;
      case MWKEY_RIGHT5 :   // Right5 HandFree
        state = IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPKPH_STATUS, 0);
        IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_SPKPH_STATE, state?0:1);
        SetSpeakerVolume();
        break;
      case MWKEY_0:  case MWKEY_1:  case MWKEY_2:  case MWKEY_3:  case MWKEY_4:  
      case MWKEY_5:  case MWKEY_6:  case MWKEY_7:  case MWKEY_8:  case MWKEY_9:  
      case MWKEY_ASTERISK:  case MWKEY_SHARP:
        if (keyin == MWKEY_ASTERISK)  keyin = '*';
        if (keyin == MWKEY_SHARP)     keyin = '#';
        if (OffHookState()) {
          if (first_digit) {
            gLastDial.wLen = 0;
            first_digit = FALSE;
            memmove(&gCallHist[DIAL_OUT][1], &gCallHist[DIAL_OUT][0], sizeof(struct CALL_HIST)*(MAX_HIST_DATA -1));
            SetCallTime(gCallHist[DIAL_OUT][0].sbDateTime);
          }
          if (IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_DIALCHAR, keyin)&&(gLastDial.wLen<sizeof(gLastDial.sbDigit)-1)) {
            gLastDial.sbDigit[gLastDial.wLen++] = (BYTE) keyin;
            gLastDial.sbDigit[gLastDial.wLen] = 0;
            SetSpeakerVolume();  // !2010-12-03 Set vol to avoid vol. reg change after AT Cmd
          }
          memcpy(gCallHist[DIAL_OUT][0].sPhoneBK.sbName,   gLastDial.sbDigit, gLastDial.wLen+1);
          memcpy(gCallHist[DIAL_OUT][0].sPhoneBK.sbNumber, gLastDial.sbDigit, gLastDial.wLen+1);
        }
        disp_chgd = TRUE;
        break;
      case MWKEY_ENTER:
      case MWKEY_CLR:
        AdjustSPKVolume();
        break;
      default:
        LongBeep();
    }   
    if (disp_chgd) {
      UpdatePhoneDisp(first_digit, (first_digit?FALSE:TRUE));
      disp_chgd = FALSE;
    }
  } while (OffHookState());
}
//*****************************************************************************
//  Function        : RingRing
//  Description     : Generate "Ring Ring" Sound.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void RingRing(BYTE *aCallerID)
{
  struct PLAYBACK playback;
  BYTE KUnknown[] = {"UNKNOWN"};

  if (aCallerID[0] == 0x00)
    aCallerID = KUnknown;
  DispPutStr("\f\x10\x1B\x60");
  PrintfMW("Call From:\x11\x1B\x80%s", aCallerID);
  os_disp_bl_control(1);
  if (gRingIn != NULL) {
    playback.bMode  = MW_MDM_PLAYRAM;
    playback.pbSrc  = gRingIn;
    playback.dwOffset = 0;
    playback.dwLen  = sizeof(KRingIn);
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY, &playback);
  }
  else
    BeepMW(70,30,2);
}
//*****************************************************************************
//  Function        : SetCallInRec
//  Description     : Update the incomming call record.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SetCallInRec(BYTE *aCallerId)
{
  int i;

  for (i = MAX_HIST_DATA - 1; i > 0;  i--) {
    memcpy(&gCallHist[DIAL_IN][i], &gCallHist[DIAL_IN][i-1], sizeof(gCallHist[DIAL_IN][i]));
  }
  memset(&gCallHist[DIAL_IN][0], 0, sizeof(gCallHist[DIAL_IN][0]));
  SetCallTime(gCallHist[DIAL_IN][0].sbDateTime);
  i = sizeof(gCallHist[DIAL_IN][0].sPhoneBK.sbNumber);
  memcpy(gCallHist[DIAL_IN][0].sPhoneBK.sbNumber, aCallerId, i);
  strcpy(gCallHist[DIAL_IN][0].sPhoneBK.sbName, gCallHist[DIAL_IN][0].sPhoneBK.sbNumber);
  for (i = 0; i < MAX_PB_ITEM; i++) {
    if (strcmp(gPhoneCfg.sPhoneBk[i].sbNumber, aCallerId)==0) {
      strcpy(gCallHist[DIAL_IN][0].sPhoneBK.sbName, gPhoneCfg.sPhoneBk[i].sbName);
      break;
    }
  }
}
//*****************************************************************************
//  Function        : PlayBack
//  Description     : Playback ram data until end
//  Input           : aBuf;       // pointer to ADPCM data
//                    aLen;       // len of input buffer
//                    aCancelEnb; // TRUE=>cancel allow
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PlayBack(BYTE *aBuf, DWORD aLen, BOOLEAN aCancelEnb)
{
  struct PLAYBACK playback;
  DWORD keyin;

  if (aBuf == NULL)
    return TRUE;

  playback.bMode  = MW_MDM_PLAYRAM;
  playback.pbSrc  = aBuf;
  playback.dwOffset = 0;
  playback.dwLen  = aLen;
  if (!IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY, &playback))
    return FALSE;

  do {
    keyin = GetCharMW();
    if ((keyin == MWKEY_CANCL) && aCancelEnb)
      break;
    switch (keyin) {
      case MWKEY_8:
        IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY_VOL, 1);  // increase the playback volume
        break;
      case MWKEY_0:
        IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY_VOL, 0);  // decrease the playback volume
        break;
    }
    SleepMW();
  } while (!IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY_END, 0));
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PBREC_ABORT, 0);
  return TRUE;
}
//*****************************************************************************
//  Function        : Recording
//  Description     : Record Sound.
//  Input           : aBuf;       // pointer to record buffer.
//                    aLen;       // len of record buffer
//                    aCancelEnb; // TRUE=>cancel allow
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD Recording(BYTE *aBuf, DWORD aLen, BOOLEAN aCancelEnb)
{
  struct MDM_RECORD mdm_record;
  DWORD keyin, len;

  len = 0;
  mdm_record.pbDat  = aBuf;
  mdm_record.dwLen  = aLen;
  mdm_record.dwCtrl = 0;
  if (!IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_REC, &mdm_record))
    return len;

  do {
    keyin = GetCharMW();
    if ((keyin == MWKEY_CANCL) && aCancelEnb) {
      len = IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PBREC_ABORT, 0);
      break;
    }
    SleepMW();
  // !TT Len lost after appcall_return();
  //} while ((len = IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_REC_END, 0)==0));
  } while ((len = bs_phone_status(K_MdmRecordEnd))==0);
  return len;
}
//*****************************************************************************
//  Function        : PlayFromFile
//  Description     : Play the adpcm file.
//  Input           : aBuf;       // filename
//                    aLen;       // max len of data to be play
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PlayFromFile(BYTE *aFileName, DWORD aLen)
{
  struct PLAYBACK playback;
  DWORD keyin;
  BYTE  filename[32];
  strcpy(filename, aFileName);
  playback.bMode    = MW_MDM_PLAYFILE;
  playback.pbSrc    = filename;
  playback.dwOffset = 0;
  playback.dwLen    = aLen;
  //keyin = IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY, &playback);
  keyin = bs_phone_playback(filename, 0, aLen);
  DispPutStr("\x0E\x12\x1B\xC0\x1CKey8: Vol Up\x05\nKey0: Vol Dn\x05\x1D\x0F");
  do {
    keyin = GetCharMW();
    if (keyin == MWKEY_CANCL)
      break;
    switch (keyin) {
      case MWKEY_8:
        IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY_VOL, 1);  // increase the playback volume
        break;
      case MWKEY_0:
        IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY_VOL, 0);  // decrease the playback volume
        break;
    }
    SleepMW();
  } while (!IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY_END, 0));
  IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PBREC_ABORT, 0);
  return;
}
//*****************************************************************************
//  Function        : RecordToFile
//  Description     : Record Message to file.
//  Input           : aFileName;        // filename
//  Return          : TRUE/FALSE;       // TRUE => SUCCESS
//  Note            : cannot use the midware to handle it, because we need to 
//                    pass the filename to system for playback
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN RecordToFile(BYTE *aFileName)
{
  BYTE *ptr;
  DWORD len;
  int   fd, err;
  BOOLEAN ret;

  ret = FALSE;
  ptr = MallocMW(MAX_RECORD_SIZE);
  if (ptr != NULL) {
    len  = Recording(ptr, MAX_RECORD_SIZE, FALSE);
    if (len > 0) {
      fd = os_file_open(aFileName, K_O_CREAT|K_O_TRUNC|K_O_RDWR);
      if (fd >= 0) {
        err = os_file_write(fd, ptr, len);
        os_file_close(fd);
        ret = TRUE;
      }
    }
    FreeMW(ptr);
  }
  return ret;
}
//*****************************************************************************
//  Function        : AutoAnswer
//  Description     : Auto Answer the Incomming Call.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void AutoAnswer(void)
{
  BYTE filename[32];

  DispPutStr("\x11\f\x1B\x80 Auto Answering ");
  while (!IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_PLAY_END, 0)) SleepMW();
  if (gPhoneCfg.dwVMailCount >= MAX_VMAIL_SUPPORT)
    return;

  // Ready record
  memset(filename, 0, sizeof(filename));
  RtcGetMW(filename);

  if (IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_ANS, 1)) {
    PlayFromFile(KOGMName, MAX_RECORD_SIZE);
    if (gBeep != NULL)
      PlayBack(gBeep, sizeof(KBeep), FALSE);
    else
      LongBeep();

    if (RecordToFile(filename)) {
      memmove(gPhoneCfg.sbVMailFile[1], gPhoneCfg.sbVMailFile[0], sizeof(gPhoneCfg.sbVMailFile)*(MAX_VMAIL_SUPPORT-1));
      memset(gPhoneCfg.sbVMailFile[0], 0, sizeof(gPhoneCfg.sbVMailFile[0]));
      memcpy(gPhoneCfg.sbVMailFile[0], filename, 14);
      gPhoneCfg.dwVMailCount++;
      SavePhoneCfg(&gPhoneCfg);
    }
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_ANS, 0);
    DispPutStr("\x11\f\x1B\x80  Record Ended  ");
  }
}
//*****************************************************************************
//  Function        : AnswerCall
//  Description     : Answer Incomming call if necessary.
//  Input           : N/A
//  Return          : TRUE/FALSE;       // TRUE => Ring Detected
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AnswerCall(void)
{
  int wait_timer, ring_count, len;
  BYTE caller_id[32], tmp[512], *ptr;
  DWORD keyin;

  if (IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_RINGIN, 0)==0)
    return FALSE;

  wait_timer = TimerOpenMW();
  memset(caller_id, 0, sizeof(caller_id));
  ring_count = 0;
  if (!gPhoneCfg.bCallerIDEnb)
    ring_count += MIN_CID_RINGCOUNT;

  memset(tmp, 0, sizeof(tmp));
  TimerSetMW(wait_timer, 1000); // wait 10 sec for next ring 
  do {
    if (IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_RINGIN, 0)) {
      ring_count++;  
      TimerSetMW(wait_timer, 1000); // wait 10 sec for next ring 
      if (IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATUS,0)) {
        if ((IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_READ, tmp) > 0) && (caller_id[0] == 0x00)) {
          ptr = strstr(tmp, "NMBR");
          ptr += ptr == NULL? 0 : 7;
          if (ptr != NULL) {
            len = strlen(ptr);
            if (len > sizeof(caller_id)) len = sizeof(caller_id);
            memcpy(caller_id, ptr, len);
            caller_id[31] = 0;
          }
        }
      }
      if (ring_count >= MIN_CID_RINGCOUNT) {
        IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE, 0);
        RingRing(caller_id);
      }
    }
    if (gPhoneCfg.bAutoAnsEnb && (ring_count > (gPhoneCfg.bAutoAnsRing + MIN_CID_RINGCOUNT))) {
      AutoAnswer();
      break;
    }
    keyin = KbdScodeMW();
    keyin &= 0x7F;
    if (OffHookState()||(keyin==MWKEY_RIGHT5)) {
      PhoneFunc(keyin);
      break;
    }
    SleepMW();
  } while (TimerGetMW(wait_timer)!=0);

  SetCallInRec(caller_id);

  if (gPhoneCfg.bCallerIDEnb)
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE, 1);    // Enable Caller ID
  else
    IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE, 0);    // Disable Caller ID
  os_disp_bl_control(gBLCtrl);
  return TRUE;
}
//*****************************************************************************
//  Function        : SetVolume
//  Description     : Set Phone's related volume.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SetVolume(void)
{
  BOOLEAN disp_chgd;
  int i, keyin;

  DispLineMW("Speaker",  MW_LINE1, MW_CLRDISP|MW_SPFONT);
  DispLineMW("Mic",      MW_LINE3, MW_SPFONT);
  DispLineMW("PlayBack", MW_LINE5, MW_SPFONT);
  DispLineMW("Record",   MW_LINE5, MW_SPFONT);

  disp_chgd = TRUE;
  while (1) {
    for (i=SPEAKER_GAIN; i < MAX_GAIN_IDX; i++) {
      ShowVolume(MW_LINE2+i*MW_LINE3, gPhoneCfg.sdwGainVal[i]);
    }
    keyin = GetCharMW();
    if ((keyin == MWKEY_ENTER) || (keyin == MWKEY_CANCL))  // Exit ?
      break;

    switch (keyin) {
      case MWKEY_LEFT2:
        gPhoneCfg.sdwGainVal[SPEAKER_GAIN] -= gPhoneCfg.sdwGainVal[SPEAKER_GAIN] > 0 ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_LEFT3:
        gPhoneCfg.sdwGainVal[MIC_GAIN] -= gPhoneCfg.sdwGainVal[MIC_GAIN] > 0 ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_LEFT4:
        gPhoneCfg.sdwGainVal[PLAYBACK_GAIN] -= gPhoneCfg.sdwGainVal[PLAYBACK_GAIN] > 0 ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_LEFT5:
        gPhoneCfg.sdwGainVal[RECORD_GAIN] -= gPhoneCfg.sdwGainVal[RECORD_GAIN] > 0 ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_RIGHT2:
        gPhoneCfg.sdwGainVal[SPEAKER_GAIN] += gPhoneCfg.sdwGainVal[SPEAKER_GAIN] < MAX_LEVEL ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_RIGHT3:
        gPhoneCfg.sdwGainVal[MIC_GAIN] += gPhoneCfg.sdwGainVal[MIC_GAIN] < MAX_LEVEL ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_RIGHT4:
        gPhoneCfg.sdwGainVal[PLAYBACK_GAIN] += gPhoneCfg.sdwGainVal[PLAYBACK_GAIN] < MAX_LEVEL ? 1 : 0;
        disp_chgd = true;
        break;
      case MWKEY_RIGHT5:
        gPhoneCfg.sdwGainVal[RECORD_GAIN] += gPhoneCfg.sdwGainVal[RECORD_GAIN] < MAX_LEVEL ? 1 : 0;
        disp_chgd = true;
        break;
    }
    if (disp_chgd) {
      SavePhoneCfg(&gPhoneCfg);
    }
  }
  SetAllVolume();
}
//*****************************************************************************
//  Function        : PhoneSetup
//  Description     : Phone Setup Function
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PhoneSetup(void)
{
  int select;
  char choice;
  BYTE kbdbuf[32];

  while (1) {
    select = ListSelect(&KSetupMenu);
    if (select == -1)
      return;

    switch (select) {
      case 0:         // Caller ID Support
        choice = ToggleOption("Caller ID :", KEnable, gPhoneCfg.bCallerIDEnb);
        if (choice != -1) {
          gPhoneCfg.bCallerIDEnb = choice;
          if (gPhoneCfg.bCallerIDEnb)
            IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE, 1);    // Enable Caller ID
          else
            IOCtlMW(gDevHandle[APM_SDEV_MDM], IO_MDM_CID_STATE, 0);    // Disable Caller ID
          SavePhoneCfg(&gPhoneCfg);
        }
        break;
      case 1:
        choice = ToggleOption("Auto Answer :", KEnable, gPhoneCfg.bAutoAnsEnb);
        if (choice != -1) {
          gPhoneCfg.bAutoAnsEnb = choice;
          SavePhoneCfg(&gPhoneCfg);
        }
        break;
      case 2:
        DispLineMW("Ring Count ", MW_LINE5, MW_CLRDISP|MW_BIGFONT);
        SprintfMW(&kbdbuf[1], "%d", gPhoneCfg.bAutoAnsRing);
        kbdbuf[0] = strlen(&kbdbuf[1]);
        if (GetKbd(NUMERIC_INPUT+MW_LINE7+ECHO, 0x03, kbdbuf)) {
          gPhoneCfg.bAutoAnsRing = (BYTE) dec2bin(&kbdbuf[1], kbdbuf[0]);
          SavePhoneCfg(&gPhoneCfg);
        }
        break;
      case 3:
        DispLineMW("Record OG MSG.", MW_LINE3, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
        if (RecordToFile(KOGMName)) {
          DispLineMW("Play Back MSG.", MW_LINE3, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
          PlayFromFile(KOGMName, MAX_RECORD_SIZE);
          break;
        }
        LongBeep();
        break;
      case 4:
        DispLineMW("Play Back MSG.", MW_LINE3, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
        PlayFromFile(KOGMName, MAX_RECORD_SIZE);
        break;
      case 5:
        if (os_file_delete(KOGMName)) {
          DispClrBelowMW(MW_LINE3);
          DispLineMW("DATA RESET !", MW_LINE5, MW_CENTER|MW_BIGFONT);
          AcceptBeep();
          Delay10ms(300);
          break;
        }
        DispLineMW("Delete Error!", MW_LINE3, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
        LongBeep();
        Delay10ms(300);
        break;
      case 6:
        SetVolume();
        break;
      case 7:
        DispLineMW("Restore Setting", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
        DispLineMW("CONFIRM ? (Y/N)", MW_LINE5, MW_CENTER|MW_BIGFONT);
        if (YesNo() == 2) { // Enter Pressed
          SetDefaultCfg(&gPhoneCfg);
          SavePhoneCfg(&gPhoneCfg);
          DispClrBelowMW(MW_LINE3);
          DispLineMW("DATA RESET !", MW_LINE5, MW_CENTER|MW_BIGFONT);
          AcceptBeep();
          Delay10ms(300);
        }
        break;
    }
  }
}
//*****************************************************************************
//  Function        : VMailCount
//  Description     : Return the current Voice Mail Message count.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD VMailCount(void)
{
  return gPhoneCfg.dwVMailCount;
}
//*****************************************************************************
//  Function        : VMailFunc
//  Description     : Voice Mail Message handling functions.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void VMailFunc(void)
{
  static const BYTE KPlayB[]  = { "\x1B\xC0REMOVE\x1B\xCC\x1CPLAY\x1D"};
  static const BYTE KRemove[] = { "\x1B\xC0\x1CREMOVE\x1D\x1B\xCCPLAY"};
  struct LIST_DAT mail_list;
  struct LIST_ITEM *mail_item;
  int  no_of_item, i;
  DWORD select;

  memcpy(mail_list.sbHeader, "    Voice Mail   ", 17);
  no_of_item = MAX_VMAIL_SUPPORT;

  mail_item = (struct LIST_ITEM *)MallocMW(sizeof(struct LIST_ITEM) * no_of_item);
  if (mail_item != NULL) {
    while (1) {
      // pack item list
      for (i = 0; i < no_of_item; i++) {
        memset(mail_item[i].sbDesc, ' ', sizeof(mail_item[i].sbDesc));
        if (gPhoneCfg.sbVMailFile[i][0] != 0x00) {
          memcpy(mail_item[i].sbDesc, gPhoneCfg.sbVMailFile[i], sizeof(gPhoneCfg.sbVMailFile[i]));
        }
      }
      mail_list.psListItem  = mail_item;
      mail_list.dwListCount = no_of_item;
      i = ListSelect(&mail_list);
      if (i == -1)
        break;
      if ((i != -1)&&(i<gPhoneCfg.dwVMailCount)) {
        select = Select1or2(KPlayB, KRemove);
        if (select == 1) {
          PlayFromFile(gPhoneCfg.sbVMailFile[i], MAX_RECORD_SIZE);
        }
        else if (select == 2) {
          os_file_delete(gPhoneCfg.sbVMailFile[i]);
          for (i; i < MAX_VMAIL_SUPPORT-2; i++) {
            memmove(gPhoneCfg.sbVMailFile[i], gPhoneCfg.sbVMailFile[i+1], sizeof(gPhoneCfg.sbVMailFile[i]));
          }
          memset(gPhoneCfg.sbVMailFile[MAX_VMAIL_SUPPORT-1], 0, sizeof(gPhoneCfg.sbVMailFile[i]));
          gPhoneCfg.dwVMailCount--;
          SavePhoneCfg(&gPhoneCfg);
        }
        else if (select == 0) {
          break;
        }
      } 
    } // end while(1);
    FreeMW(mail_item);
  }
}
//-----------------------------------------------------------------------------
#endif // (R700)
//-----------------------------------------------------------------------------
