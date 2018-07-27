//-----------------------------------------------------------------------------
//  File          : WAVECOM.c
//  Module        :
//  Description   : Include API for WAVECOM Module (WMP100)
//  Author        : Lewis
//  Notes         : N/A
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
//  01 Apr  2009  Lewis       - Port from Creon (Lastest copy from John)
//                            - Apply new naming covention.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "System.h"
#include "util.h"
#include "apm.h"
#include "sysutil.h"
#include "hardware.h"
#include "wavecom.h"

#if (T300|T810|A5T1000|T1000|T800|TIRO)
//----------------------------------------------------------------------
//      Defines
//----------------------------------------------------------------------
#define ATCMD_TIMEOUT   600*2  // 6 secs timeout

#if (T800)
#define SMS_PDU_MODE           K_WPduMode
#define SMS_TXT_MODE           K_WTextMode
#define SMS_TP_MTI_SMS_SUBMIT  K_SmsSubmit
#else
#define SMS_PDU_MODE           0
#define SMS_TXT_MODE           1
#define SMS_TP_MTI_SMS_SUBMIT  K_GPRS_SMS_TP_MTI_SMS_SUBMIT
#endif

//----------------------------------------------------------------------
//       Command Code
//----------------------------------------------------------------------
#define NO_WAIT           0   // not wait any response
#define WAIT_RSP          1   // wait until response comes
#define WAIT_OK           2   // wait until "OK"/"ERROR"

//----------------------------------------------------------------------
//      Constant
//----------------------------------------------------------------------
static const char *KBookID[] = { "SM", "MC", "LD", "RC"};   // Phone Book
//static const char *KListType[] = {"REC UNREAD","REC READ","STO UNSENT","STO SENT","ALL"};  // SMS

//----------------------------------------------------------------------
//       Global variables
//----------------------------------------------------------------------
#define MAX_BUF_SIZE     1500  // max buffer according to boot system
static char gAtCmdIn[MAX_BUF_SIZE+1];
static char gAtCmdOut[MAX_BUF_SIZE+1];

struct GSM_DATA gGSMData;   // Use By Phone Book
//#if (T800)
//static T_SMS_PDU gSmsPdu;
//#endif
//#if (TIRO)
//static T_GPRS_SMS_PDU gSmsPdu;
//#endif

//*****************************************************************************
//  Function        : SendWaitRsp
//  Description     : Send & wait response.
//  Input           : aCmdStr;  // Wavecom AT command string.
//                    aWaitRsp; // NO_WAIT/WAIT_RSP/WAIT_OK
//  Return          : TRUE;     // valid AT response received
//                    FALSE;    // error in sending/receiving AT response
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SendWaitRsp(void *aCmdStr, BYTE aWaitRsp)
{
  struct GPRSATCMD  at_cmd;
  struct GPRSATRESP at_resp;
  DWORD last_tick, curr_tick;
  DWORD rx_len;
  char  *ptr;

  last_tick = FreeRunMW();
  while (1) {
    curr_tick = FreeRunMW();
    if ((curr_tick - last_tick) > ATCMD_TIMEOUT)
      break;
    if (StatMW(gGprsHandle, MW_GSTS_ATCMDRDY, NULL)&MW_GSTS_ATCMDRDY)
      break;
  }
  if (!(StatMW(gGprsHandle, MW_GSTS_ATCMDRDY, NULL)&MW_GSTS_ATCMDRDY))
    return FALSE;

  at_cmd.d_timeout = ATCMD_TIMEOUT;
  strcpy(at_cmd.sb_content, aCmdStr);
  if (!IOCtlMW(gGprsHandle, IO_GPRS_CMDSEND, &at_cmd))
    return FALSE;

  if (aWaitRsp == NO_WAIT)
    return TRUE;

  memset(gAtCmdOut, 0, sizeof(gAtCmdOut));
  ptr = gAtCmdOut;
  last_tick = FreeRunMW();
  rx_len = 0;
  while (1) {
    curr_tick = FreeRunMW();
    if ((curr_tick>last_tick) && ((curr_tick-last_tick) >= ATCMD_TIMEOUT))
      break;
    if ((curr_tick<last_tick) && (((DWORD)-1-last_tick+curr_tick) >= ATCMD_TIMEOUT))
      break;
    if (StatMW(gGprsHandle, MW_GSTS_ATRESPRDY, NULL)&MW_GSTS_ATRESPRDY) {
      at_resp.d_maxlen = MAX_BUF_SIZE-strlen(gAtCmdOut);
      rx_len = IOCtlMW(gGprsHandle, IO_GPRS_CMDRESP, &at_resp);
      memcpy(ptr, at_resp.sb_content, rx_len);
      if (aWaitRsp == WAIT_OK) {
        if (strstr((char const *)ptr, "OK") != NULL)
          return TRUE;
        if (strstr((char const *)ptr, "ERROR") != NULL)
          return TRUE;
      }
      if (strlen(gAtCmdOut) == MAX_BUF_SIZE)
        break;
      ptr += rx_len;
    }
    else
      if ((aWaitRsp==WAIT_RSP)&&strlen(gAtCmdOut))
        return TRUE;
    SleepMW();
  }
  if ((aWaitRsp==WAIT_RSP)&&strlen(gAtCmdOut))
    return TRUE;

  return FALSE;
}
#if (T800)
//*****************************************************************************
//  Function        : WaitRsp
//  Description     : Wait response from Wavecom module
//  Input           : aType;   // WAIT_RSP/WAIT_OK
//  Return          : TRUE;    // valid AT response received
//                    FALSE;   // error in sending/receiving AT response
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN WaitRsp(DWORD aType)
{
  struct GPRSATRESP  at_resp;

  DWORD last_tick, curr_tick;
  DWORD rx_len;
  char  *ptr;

  memset(gAtCmdOut, 0, sizeof(gAtCmdOut));
  ptr = gAtCmdOut;
  last_tick = FreeRunMW();
  rx_len = 0;
  while (1) {
    curr_tick = FreeRunMW();
    if ((curr_tick - last_tick) > ATCMD_TIMEOUT)
      break;
    if (StatMW(gGprsHandle, MW_GSTS_ATRESPRDY, NULL)&MW_GSTS_ATRESPRDY) {
      at_resp.d_maxlen = MAX_BUF_SIZE-strlen(gAtCmdOut);
      rx_len = IOCtlMW(gGprsHandle, IO_GPRS_CMDRESP, &at_resp);
      strcpy(ptr, at_resp.sb_content);
      if (aType == WAIT_OK) {
        if (strstr((char const *)ptr, "OK") != NULL)
          return TRUE;
        if (strstr((char const *)ptr, "ERROR") != NULL)
          return TRUE;
      }
      if (strlen(gAtCmdOut) == MAX_BUF_SIZE)
        break;
      ptr += rx_len;
    }
    else
      if ((aType==WAIT_RSP)&&strlen(gAtCmdOut))
        return TRUE;
    SleepMW();
  }
  if ((aType==WAIT_RSP)&&strlen(gAtCmdOut))
    return TRUE;
  return FALSE;
}
#endif
//*****************************************************************************
//  Function        : ClearCmdBuf
//  Description     : Clead Response buffer from WAVE module.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ClearCmdBuf(void)
{
  struct GPRSATRESP  at_resp;
  // clear AT response
  do {
    at_resp.d_maxlen = sizeof(at_resp.sb_content);
    IOCtlMW(gGprsHandle, IO_GPRS_CMDRESP, &at_resp);
    SleepMW();
  } while (StatMW(gGprsHandle, MW_GSTS_ATRESPRDY, NULL)&MW_GSTS_ATRESPRDY);
}
//*****************************************************************************
//  Function        : WaitModuReady
//  Description     : Wait for module ready to accept command.
//  Input           : aTimeout;      // timeout in 10ms
//  Return          : TRUE/FALSE;   // Ready/not Ready
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WaitModuReady(DWORD aTimeout)
{
  DWORD status;
  DWORD last_tick, curr_tick;

  last_tick = FreeRunMW();
  do {
    curr_tick = FreeRunMW();
    if ((curr_tick - last_tick) > aTimeout)
      break;
    status = StatMW(gGprsHandle, MW_GSTS_ATREADY1|MW_GSTS_ATREADY2, NULL);
    if (status&(MW_GSTS_ATREADY1|MW_GSTS_ATREADY2))
      return TRUE;
    SleepMW();
  } while (1);

  return FALSE;
}
//*****************************************************************************
//  Function        : SimDetect
//  Description     : Detect whether sim card Really inserted.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SimDetect(void)
{
  BOOLEAN detected;

  detected = FALSE;
  ClearCmdBuf();
  if (SendWaitRsp("AT+CPIN?\r\n", WAIT_RSP)) {
    if (strstr(gAtCmdOut, "CPIN"))
      detected = TRUE;
  }
  ClearCmdBuf();

  return detected;
}
//*****************************************************************************
//  Function        : RegisterNetwork
//  Description     : Check network registration.
//  Input           : 0:            // GSM
//                    1;            // GPRS
//  Return          : TRUE/FALSE;
//  Note            : 0             // done no reg.
//                    1             // registered home network
//                    2             // processing
//                    3             // denied
//                    4             // unknow err
//                    5             // registered roaming
//  Globals Changed : N/A
//*****************************************************************************
BYTE RegisterNetwork(BYTE aGPRS)
{
  const char cmd[2][6] = { "CREG", "CGREG" };
  char *ptr;

  ClearCmdBuf();
  sprintf(gAtCmdIn, "AT+%s=1\r\n", cmd[aGPRS]);

  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return 4;

  ClearCmdBuf();
  sprintf(gAtCmdIn, "AT+%s?\r\n", cmd[aGPRS]);
  if (SendWaitRsp(gAtCmdIn, WAIT_RSP)) {
    ptr = strstr(gAtCmdOut, cmd[aGPRS]);
    if (ptr!=NULL) {
      return ptr[strlen(cmd[aGPRS])+2];
    }
  }
  ClearCmdBuf();
  return '4';
}
//*****************************************************************************
//  Function        : AttachGPRS
//  Description     : Attach GPRS
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE AttachGPRS(void)
{
  ClearCmdBuf();
  if (SendWaitRsp("AT+CGATT=1\r\n", WAIT_OK)) {
    if (strstr(gAtCmdOut, "OK"))
      return TRUE;
  }
  ClearCmdBuf();
  return FALSE;
}
//*****************************************************************************
//  Function        : Asc2DWVAL
//  Description     : Convert ASCII to number.
//  Input           : *aPtr;    // Buffer start
//                  : aMaxLen;  // max length of string
//  Return          : value;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD Asc2DWVAL(BYTE *aPtr, BYTE aMaxLen)
{
  BYTE temp;
  DWORD val;

  val = 0;
  while (aMaxLen--) {
    temp = *aPtr - '0';
    if (temp <= 9) {
      val = val*10 + temp;
      aPtr++;
    }
    else
      break;
  }
  return val;
}
//*****************************************************************************
//  Function        : GetPHBConfig
//  Description     : Get Phone num size & name size for phone book
//  Input           : aTelSize;     // pointer to storage
//                    aNameSize;    // pointer to storage
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN GetPHBConfig(DWORD *aTelSize, DWORD *aNameSize)
{
  BYTE *ptr;
  DWORD idx1, idx2, idx3, r_len;

  // GSM
  // get current phonebook status (entries used and available)
  if (!SendWaitRsp("AT+CPBF=?\r\n", WAIT_OK))
    return FALSE;

  // Expected "+CPBF: 60,14"
  ptr = (BYTE *)strstr((char const *)gAtCmdOut, "+CPBF:");
  if (ptr == NULL)
    return FALSE;

  r_len = strlen(gAtCmdOut) - (ptr - (BYTE *)gAtCmdOut);


  idx1 = fndb(ptr, ' ', r_len);             // find space
  if (idx1 == r_len)
    return FALSE;

  r_len = r_len - idx1;
  idx2 = fndb(ptr+idx1+1, ',', r_len);      // find 1st comma
  if (idx2 == r_len)
    return FALSE;

  r_len = r_len - idx2;
  idx3 = fndb(ptr+idx1+idx2+1, '\r', r_len);     // find '\r'
  if (idx3 == r_len)
    return FALSE;

  idx2 = idx1 + idx2 + 1;

  *aTelSize =  (WORD)Asc2DWVAL(ptr+idx1+1, 3);
  *aNameSize= (WORD)Asc2DWVAL(ptr+idx2+1, 3);
  return TRUE;
}
//*****************************************************************************
//  Function        : GetPHBCapcity
//  Description     : Get SIM card phone book capacity
//  Input           : aUsed;     // pointer to storage
//                    aTotal;    // pointer to storage
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetPHBCapcity(DWORD *aUsed, DWORD *aTotal)
{
  BYTE *ptr;
  DWORD idx1, idx2, idx3, r_len;

  // get current phonebook status (entries used and available)
  if (!SendWaitRsp("AT+CPBS?\r\n", WAIT_OK))
    return FALSE;

  ptr = (BYTE *)strstr((char const *)gAtCmdOut, "+CPBS:");
  if (ptr == NULL)
    return FALSE;

  r_len = strlen(gAtCmdOut) - (ptr - (BYTE *)gAtCmdOut);

  idx1 = fndb(ptr, ',', r_len);             // find first comma
  if (idx1 == r_len)
    return FALSE;

  r_len = r_len - idx1;
  idx2 = fndb(ptr+idx1+1, ',', r_len);      // find second comma
  if (idx2 == r_len)
    return FALSE;

  r_len = r_len - idx2;
  idx3 = fndb(ptr+idx1+idx2+1, '\r', r_len);     // find '\r'
  if (idx3 == r_len)
    return FALSE;

  idx2 = idx1 + idx2 + 1;
  idx3 = idx2 + idx3 + 1;

  *aUsed =  (WORD)Asc2DWVAL(ptr+idx1+1, 3);
  *aTotal = (WORD)Asc2DWVAL(ptr+idx2+1, 3);

  *aUsed  = *aUsed > MAX_PHBOOK_REC ? MAX_PHBOOK_REC: *aUsed;
  *aTotal = gGSMData.d_total_count > MAX_PHBOOK_REC ? MAX_PHBOOK_REC: *aTotal;

  return TRUE;
}
//*****************************************************************************
//  Function        : GetContactInfo
//  Description     : Get tel number and name
//  Input           : *tel;     // ptr to tel number buffer
//                  : *name;    // ptr to name buffer
//                  : *aStr;   // starting ptr to search
//                  : len;      // length of string to search
//  Return          : 0;        // Not found
//                    1;        // TEL READY
//                    2;        // TEL & NAME ready
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
static BYTE GetContactInfo(struct PHONE_LIST *aContact, BYTE *aStr, DWORD aLen)
{
  BYTE tel_len, name_len;
  BYTE *ptr;
  BYTE state;

  tel_len = name_len = 0;
  state = 0;
  ptr = aStr;
  while ((DWORD)(ptr - aStr) < aLen) {
    if (*ptr == '\"') {
      state++;
      ptr++;
      continue;
    }
    switch (state) {
      case 1:
        if (tel_len < MAX_PHNUM_SIZE)
          aContact->sb_tel[tel_len++] = *ptr;
        break;
      case 3:
        if (name_len < MAX_NAME_SIZE)
          aContact->sb_name[name_len++] = *ptr;
        break;
      default:
        break;
    }
    ptr++;
  }
  // no tel. no. found
  if (!tel_len)
    return 0;

  // only tel. no. found
  if (!name_len)
    return 1;

  // tel. no. & name found
  return 2;
}
//*****************************************************************************
//  Function        : GetContact
//  Description     : Get 1 record from phone book
//  Input           : aRec;         // pointer to phone book rec.
//                    aRecIdx;      // record to read
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetContact(struct PHONE_LIST *aRec, DWORD aRecIdx)
{
  BYTE *ptr;
  BYTE buf[16];
  DWORD rlen;

  memset(buf, 0, sizeof(buf));
  sprintf((char *)buf, "AT+CPBR=%d\r\n", aRecIdx);
  if (!SendWaitRsp(buf, WAIT_RSP))
    return FALSE;

  ptr = (BYTE *)strstr((char const *)gAtCmdOut, "+CPBR:");
  if (ptr == NULL)
    return FALSE;

  rlen = strlen(gAtCmdOut) - (ptr - (BYTE *)gAtCmdOut);
  memset(aRec, 0, sizeof(struct PHONE_LIST));
  if (GetContactInfo(aRec, ptr, rlen) == 0)
    return FALSE;

  aRec->d_idx = aRecIdx;
  return TRUE;
}
//*****************************************************************************
//  Function        : LoadPhoneBook
//  Description     : Load selected phone book into internal menory.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void LoadPhoneBook(void)
{
  DWORD i, rec_idx, max_rec;

  rec_idx = 0;
  max_rec = gGSMData.d_total_count > MAX_PHBOOK_REC ? MAX_PHBOOK_REC: gGSMData.d_total_count;
  for (i=0; i< gGSMData.d_total_count; i++) {
    if (rec_idx == max_rec)  // full
      break;
    if (rec_idx == gGSMData.d_used_count)  // got enough
      break;
    if (!GetContact(&gGSMData.s_PHBook[rec_idx], i+1))
      continue;
    rec_idx++;
  }
}
//*****************************************************************************
//  Function        : SortPhoneBook
//  Description     : Sort the Phone List by name.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SortPhoneBook(void)
{
  WORD i, j;
  WORD len1, len2;

  if (gGSMData.d_used_count < 2)
    return;

  // Sort Record
  for (i=0; i<gGSMData.d_used_count-1; i++) {
    for (j=i+1; j<gGSMData.d_used_count; j++) {
      len1 = strlen(gGSMData.s_PHBook[i].sb_name);
      len2 = strlen(gGSMData.s_PHBook[j].sb_name);
      len1 = len1>len2?len2:len1;
      if (memcmp(gGSMData.s_PHBook[i].sb_name, gGSMData.s_PHBook[j].sb_name, len1)<0)
        continue;
      memcpy((BYTE *) &gGSMData.s_CurrPHB,   (BYTE *) &gGSMData.s_PHBook[j], sizeof(struct PHONE_LIST));
      memcpy((BYTE *) &gGSMData.s_PHBook[j], (BYTE *) &gGSMData.s_PHBook[i], sizeof(struct PHONE_LIST));
      memcpy((BYTE *) &gGSMData.s_PHBook[i], (BYTE *) &gGSMData.s_CurrPHB,   sizeof(struct PHONE_LIST));
    }
  }
}
//*****************************************************************************
//  Function        : SelectPHBook
//  Description     : Read the Selected Phone book record.
//  Input           : id;       // Phone BOOK id.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SelectPHBook(DWORD aID)
{
  if (gGSMData.d_used_count != 0)
    return TRUE;

  ClearCmdBuf();
  sprintf(gAtCmdIn, "AT+CPBS=\"%s\"\r\n", KBookID[aID]);

  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  if (strstr(gAtCmdOut, "OK")==NULL)
    return FALSE;

  if (!GetPHBConfig(&gGSMData.d_max_phone_len, &gGSMData.d_max_name_size))
    return FALSE;

  gGSMData.d_max_phone_len = gGSMData.d_max_phone_len > MAX_PHNUM_SIZE ? MAX_PHNUM_SIZE: gGSMData.d_max_phone_len;
  gGSMData.d_max_name_size = gGSMData.d_max_name_size > MAX_NAME_SIZE ? MAX_PHNUM_SIZE: gGSMData.d_max_name_size;

  if (!GetPHBCapcity(&gGSMData.d_used_count, &gGSMData.d_total_count))
    return FALSE;

  LoadPhoneBook();
  SortPhoneBook();

  return TRUE;
}
//*****************************************************************************
//  Function        : WriteContact
//  Description     : Add/Update Phone record depend on record index.
//  Input           : aRec;     // P
//  Return          : TRUE;    // valid AT response received
//                    FALSE;   // error in sending/receiving AT response
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WriteContact(struct PHONE_LIST *aRec, DWORD aCurrRec)
{
  if (aRec->d_idx) {
    sprintf((char *)gAtCmdIn, "AT+CPBW=%d,\"", aRec->d_idx);
    memcpy((BYTE *)&gGSMData.s_PHBook[aCurrRec], (BYTE *)&gGSMData.s_CurrPHB,
           sizeof(struct PHONE_LIST));
  }
  else {
    strcpy(gAtCmdIn, "AT+CPBW=,\"");
    memcpy((BYTE *)&gGSMData.s_PHBook[aCurrRec], (BYTE *)&gGSMData.s_CurrPHB,
           sizeof(struct PHONE_LIST));
  }

  strcat(gAtCmdIn, aRec->sb_tel);
  strcat(gAtCmdIn, "\",129,\"");
  strcat(gAtCmdIn, aRec->sb_name);
  strcat(gAtCmdIn, "\"\r\n");

  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  if (aRec->d_idx==0) {
    gGSMData.d_used_count++;
    gGSMData.d_total_count++;
    LoadPhoneBook();
  }
  SortPhoneBook();
  return TRUE;
}
//*****************************************************************************
//  Function        : DelContact
//  Description     : Delete Phone book entry.
//  Input           : aRecIdx;      // Phone book record index
//  Return          : TRUE;    // valid AT response received
//                    FALSE;   // error in sending/receiving AT response
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DelContact(DWORD aRecIdx, DWORD aCurrRec)
{
  DWORD i;
  sprintf((char *)gAtCmdIn, "AT+CPBW=%d\r\n", aRecIdx);
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  // move record
  for (i=aCurrRec; i<gGSMData.d_used_count; i++) {
    memcpy((BYTE *)&gGSMData.s_PHBook[i], (BYTE *)&gGSMData.s_PHBook[i+1],
           sizeof(struct PHONE_LIST));
  }
  gGSMData.d_used_count--;
  gGSMData.d_total_count--;
  SortPhoneBook();
  return TRUE;
}
//*****************************************************************************
//  Function        : AllCallForward
//  Description     : Set/Cancel All Call Forward.
//  Input           : pTel;     // pointer to forward phone num.
//                    cancel;   // TRUE => cancel all call forward.
//  Return          : None;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AllCallForward(char *pTel, BOOLEAN cancel)
{
  ClearCmdBuf();
  if (cancel)
    strcpy(gAtCmdIn, "AT+CCFC=0,4\r\n");
  else   // divert to a telephone number
#if (TIRO)      
    sprintf(gAtCmdIn, "AT+CCFC=0,3,\"%s\",129\r\n", pTel);
#else  
    sprintf(gAtCmdIn, "AT+CCFC=0,3,\"%s\"\r\n", pTel);
#endif    
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  if (strstr(gAtCmdOut, "OK"))
    return TRUE;
  return FALSE;
}
//*****************************************************************************
//  Function        : MakeCall
//  Description     : Make a voice Call.
//  Input           : pPhoneNum;        // Phone num string.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MakeCall(char *pPhoneNum)
{
  strcpy(gAtCmdIn, "ATD");
  strcat(gAtCmdIn, pPhoneNum);
  strcat(gAtCmdIn, ";\r\n");

  ClearCmdBuf();
#if (TIRO)  
  if (SendWaitRsp(gAtCmdIn, WAIT_OK))
#else      
  if (SendWaitRsp(gAtCmdIn, NO_WAIT))
#endif      
    return TRUE;

  return FALSE;
}
//*****************************************************************************
//  Function        : MakeReqCall
//  Description     : Make a Request Call
//  Input           : pPhoneNum;        // Phone num string.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MakeReqCall(char *pPhoneNum)
{
  strcpy(gAtCmdIn, "ATD");
  strcat(gAtCmdIn, pPhoneNum);
  strcat(gAtCmdIn, "\r\n");

  if (SendWaitRsp(gAtCmdIn, WAIT_OK)) {
    if (strstr(gAtCmdOut, "OK"))
      return TRUE;
  }

  return FALSE;
}
//*****************************************************************************
//  Function        : CheckCallStatus
//  Description     : Check Call Status
//  Input           : N/A
//  Return          : status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD CheckCallStatus(void)
{
#if (T800)
  if (!WaitRsp(WAIT_RSP))
    return NOT_READY;

  if (strstr((char const *)gAtCmdOut, "NO CARRIER") != NULL) {
    return NO_CARRIER;
  }
  else
    if (strstr((char const *)gAtCmdOut, "NO ANSWER") != NULL) {
      return NO_ANSWER;
    }
    else
      if (strstr((char const *)gAtCmdOut, "BUSY") != NULL) {
        return BUSY;
      }
      else
        if (strstr((char const *)gAtCmdOut, "OK") != NULL) {
          return CONNECTED;
        }

  return NOT_READY;
#endif
#if (T300|T810|A5T1000|T1000|TIRO)  
  DWORD state, status;
  BYTE  tmpbuf[MW_MAX_LINESIZE+1];

  status = StatMW(gGprsHandle, -1, NULL);
  sprintf(tmpbuf, "Status:%04X", status);
  DispLineMW(tmpbuf, MW_MAX_LINE, MW_CENTER|MW_SPFONT);
  switch (status & 0x3F) {
    case 4:
      state = CONNECTING;
      break;
    case 5:
      state = CONNECTED;
      break;
    case 20:        // Disconnect network
    case 21:        // Disconnect user
    case 22:        // Reject user
      state = NO_CARRIER;
      break;
    case 0:
    default:
      state = NOT_READY;
      break;
  }
  return state;
#endif  
}
//*****************************************************************************
//  Function        : AnswerCall
//  Description     : Answer Call.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AnswerCall(void)
{
  strcpy(gAtCmdIn, "ATA\r\n");
  ClearCmdBuf();
  if (SendWaitRsp(gAtCmdIn, WAIT_OK))
    return TRUE;
  return FALSE;
}
//*****************************************************************************
//  Function        : SendKeyPress
//  Description     : Send Key Press data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SendKeyPress(char aChar)
{
#if (TIRO)
  sprintf(gAtCmdIn, "AT+VTS=\"%c\"\r\n", aChar);
#endif
#if (T800)  
  sprintf(gAtCmdIn, "AT+VTS=%c\r\n", aChar);
#endif  
  ClearCmdBuf();
  if (SendWaitRsp(gAtCmdIn, WAIT_OK))
    return TRUE;
  return FALSE;
}
//*****************************************************************************
//  Function        : HangUp
//  Description     : Disconnect Calls.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN HangUp(void)
{
  strcpy(gAtCmdIn, "ATH\r\n");
  ClearCmdBuf();
  if (SendWaitRsp(gAtCmdIn, WAIT_OK))
    return TRUE;
  return FALSE;
}
//*****************************************************************************
//  Function        : CheckCallEnd
//  Description     : Check call disconnected remotely.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckCallEnd(void)
{
  struct GPRSATRESP  at_resp;
  BYTE *ptr;

  if (!(StatMW(gGprsHandle, MW_GSTS_ATRESPRDY, NULL)&MW_GSTS_ATRESPRDY)) 
    return FALSE;

  at_resp.d_maxlen = sizeof(at_resp.sb_content);
  if (IOCtlMW(gGprsHandle, IO_GPRS_CMDRESP, &at_resp)) {
    ptr = (BYTE *)strstr((char const *)at_resp.sb_content, "NO CARRIER");
    if (ptr == NULL)
      return FALSE;
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : GetCallerID
//  Description     : Get callerl ID from the module
//  Input           : None;
//  Return          : TRUE;    // valid access code entered
//                    FALSE;   // user cancel or invalid access code.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetCallerID(void)
{
#if (T800)
  BYTE *ptr;

  if (!WaitRsp(WAIT_RSP))
    return FALSE;

  // search for << +CLIP: "tel_num",129,1,,"caller_name" >> for GSM
  // search for << +CLIP:"tel_num",129" >> for CDMA
  ptr = (BYTE *)strstr((char const *)gAtCmdOut, "+CLIP:");
  if (ptr == NULL)
    return FALSE;

  memset(&gGSMData.s_CurrPHB, 0, sizeof(struct PHONE_LIST));
  return GetContactInfo(&gGSMData.s_CurrPHB, (BYTE *)gAtCmdOut, strlen(gAtCmdOut));
#endif  
#if (T300|T810|A5T1000|T1000|TIRO)   
  struct GPRSATRESP  at_resp;

  if (StatMW(gGprsHandle, MW_GSTS_CID, NULL)==0)
    return FALSE;
  memset(&gGSMData.s_CurrPHB, 0, sizeof(struct PHONE_LIST));
  at_resp.d_maxlen = sizeof(gGSMData.s_CurrPHB.sb_name);
  at_resp.d_maxlen = IOCtlMW(gGprsHandle, IO_GPRS_GET_CID, &at_resp);
  memcpy(gGSMData.s_CurrPHB.sb_name, at_resp.sb_content, at_resp.d_maxlen);

  return TRUE;
#endif  
}
//*****************************************************************************
//  Function        : ReadGain
//  Description     : Read GAIN
//  Input           : None;
//  Return          : TRUE;    // valid AT response received
//                    FALSE;   // error in sending/receiving AT response
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ReadGain(DWORD aType)
{
  BYTE cmd[16];
#if (T800|TIRO)
  DWORD val=0;
#endif
  BYTE idx=8;

  sprintf((char *)cmd, "AT+VG%c?\r\n", aType);

  if (!SendWaitRsp(cmd, WAIT_OK))
    return FALSE;
  cmd[0] = '\r';
  cmd[1] = '\n';
  cmd[6] = ':';
  cmd[7] = ' ';
  if (memcmp(gAtCmdOut, cmd, idx) == 0) {
#if (T800|TIRO)
    val = Asc2DWVAL((BYTE *)&gAtCmdOut[8], 3);
#endif
#if (T800)
    if (gAtCmdOut[5] == SPK)
      return 8 - val / 32;      // speaker gain scale 1 - 8
    else
      return val / 32 + 1;      // mic gain scale 1 - 8
#endif
#if (TIRO)
    val = gAtCmdOut[8]-'0';
	return val;
#endif
  }
  return 0;
}
//*****************************************************************************
//  Function        : WriteGain
//  Description     : display AT command response if in debug mode
//  Input           : None;
//  Return          : TRUE;    // valid AT response received
//                    FALSE;   // error in sending/receiving AT response
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN WriteGain(DWORD aType, DWORD aVal)
{
  BYTE *ptr;

#if (T800)
  if (aType == SPK)
    aVal = (8 - aVal) * 32;       // scale from 1 - 8
  else
    aVal = (aVal - 1) * 32;
#endif

  sprintf((char *)gAtCmdIn, "AT+VG%c=%d\r\n", aType, aVal);
  ClearCmdBuf();
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  ptr = (BYTE *)strstr((char const *)gAtCmdOut, "OK");
  if (ptr != NULL) {
    return TRUE;
  }

  return FALSE;
}
//*****************************************************************************
//  Function        : GetOptrName
//  Description     : Get the short form operator name
//  Input           : aName;        // Pointer to name buffer
//                    aMaxLen;      // Max Output size
//  Return          : None;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetOptrName(char *aName, DWORD aMaxLen)
{
  char *ptr;
  DWORD len;

  // GSM
  *aName = 0x00;
  //ClearCmdBuf();
  //if (!SendWaitRsp("AT+COPS=3,0\r\n", WAIT_OK)) // long alpha format
  //  return FALSE;

  ClearCmdBuf();
  if (!SendWaitRsp("AT+COPS?\r\n", WAIT_OK))
    return FALSE;

  ptr = strstr((char const *)gAtCmdOut, "+COPS:");
  if (ptr == NULL)
    return FALSE;
  ptr = strstr((char const *)gAtCmdOut, "\"");
  if (ptr == NULL)
    return FALSE;
  ptr++;
  while (*ptr!='"'&& aMaxLen--) {
    *aName++ = *ptr++;
  }
  *aName = 0x00;

  //Skip trailing blank
  len = strlen(aName);
  while (aName[len-1] == ' ') {
    aName[len-1] = 0x00;
    len--;
  }

  return TRUE;
}
//*****************************************************************************
//  Function        : FacilityEnable
//  Description     : Enable/Disable PIN
//  Input           : aEnable;              // TRUE => Enable PIN
//                    aPin;                 // Current PIN value
//  Return          : 0: Fail
//                    1: Success
//                    2: Sim Busy
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD FacilityEnable(BOOLEAN aEnable, char *aPin)
{
  DWORD ret;

  ret = 0;
  ClearCmdBuf();
  SendWaitRsp("AT+CMEE=1", WAIT_OK);
  ClearCmdBuf();

  if (aEnable)
    sprintf(gAtCmdIn, "AT+CLCK=\"%s\",%s,%s\r\a", "SC", "1", aPin);
  else
    sprintf(gAtCmdIn, "AT+CLCK=\"%s\",%s,%s\r\a", "SC", "0", aPin);

  ClearCmdBuf();
  if (SendWaitRsp(gAtCmdIn, WAIT_OK)) {
    if (strstr(gAtCmdOut, "OK"))
      ret = 1;
    else
      if (strstr(gAtCmdOut, "515"))
        ret = 2;
  }
  ClearCmdBuf();
  SendWaitRsp("AT+CMEE=0", WAIT_OK);
  ClearCmdBuf();
  return ret;
}
//*****************************************************************************
//  Function        : CheckPinState
//  Description     : Check Pin Requirement.
//  Input           : N/A
//  Return          : status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD CheckPinState(void)
{
  DWORD req_pin;
  req_pin = PIN_ERROR;
  ClearCmdBuf();
  if (SendWaitRsp("AT+CPIN?\r", WAIT_RSP)) {
    if (strstr(gAtCmdOut, "READY"))
      req_pin = REQ_READY;
    else if (strstr(gAtCmdOut, "PUK2"))
      req_pin = REQ_PUK2;
    else if (strstr(gAtCmdOut, "PUK"))
      req_pin = REQ_PUK1;
    else if (strstr(gAtCmdOut, "PIN2"))
      req_pin = REQ_PIN2;
    else if (strstr(gAtCmdOut, "PIN"))
      req_pin = REQ_PIN1;
  }
  ClearCmdBuf();
  return req_pin;
}
//*****************************************************************************
//  Function        : EnterPin
//  Description     : Enter PIN
//  Input           : aId;              // Pin Input Option;
//                    aPin;             // Pointer to Pin Data string;
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EnterPin(DWORD aId, char *aPin)
{
  BOOLEAN ret;

  ret = FALSE;
  if (aId == REQ_PIN1)
    sprintf(gAtCmdIn, "AT+CPIN=\"%s\"\r\n", aPin);
  else
    sprintf(gAtCmdIn, "AT+CPIN2=\"%s\"\r\n", aPin);

  if (SendWaitRsp(gAtCmdIn, WAIT_RSP)) {
    if (strstr(gAtCmdOut, "OK"))
      ret = TRUE;
  }
  ClearCmdBuf();
  return ret;
}
//*****************************************************************************
//  Function        : ChangePin
//  Description     : Change PIN value.
//  Input           : aId;              // PIN/PIN2
//                    aOldPin;          // Pointer to Org. Pin Data string;
//                    aNewPin;          // Pointer to New Pin Data string;
//  Return          : 0: Fail
//                    1: Success
//                    2: Sim Busy
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD ChangePin(DWORD aId, char *aOldPin, char *aNewPin)
{
  DWORD ret;

  ret = 0;
  ClearCmdBuf();
  SendWaitRsp("AT+CMEE=1", WAIT_OK);
  ClearCmdBuf();

  if (aId == REQ_PIN1)
    sprintf(gAtCmdIn, "AT+CPWD=\"%s\",%s,%s\r", "SC", aOldPin, aNewPin);
  else
    sprintf(gAtCmdIn, "AT+CPWD=\"%s\",%s,%s\r", "P2", aOldPin, aNewPin);

  if (SendWaitRsp(gAtCmdIn, WAIT_OK)) {
    if (strstr(gAtCmdOut, "OK"))
      ret = 1;
    else
      if (strstr(gAtCmdOut, "515"))
        ret = 2;
  }
  ClearCmdBuf();
  SendWaitRsp("AT+CMEE=0", WAIT_OK);
  ClearCmdBuf();
  return ret;
}
//*****************************************************************************
//  Function        : SetNewMsgInd
//  Description     : Enable/Disable SMS new message indicator.
//  Input           : aEnable;      // TRUE/FALSE
//  Return          : None;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SetNewMsgInd(BOOLEAN aEnable)
{
  if (aEnable) {
    if (!SendWaitRsp("AT+CNMI=0,1,1,1,0\r\n", WAIT_OK))
      return FALSE;
  }
  else
    if (!SendWaitRsp("AT+CNMI=0,0,0,0,0\r\n", WAIT_OK))
      return FALSE;
  return TRUE;
}
//*****************************************************************************
//  Function        : GetSmsMode
//  Description     : Get the current SMS mode
//  Input           : None
//  Return          : Either PDU or text mode will be returned
//                    0xFF - error
//  Note            : ++ Jason ++ 22-01-09
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetSmsMode(void)
{
  strcpy(gAtCmdIn, "AT+CMGF?\r\n");
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return 0xFF;

  if (strstr(gAtCmdOut, "+CMGF: 0"))
    return SMS_PDU_MODE;
  else if (strstr(gAtCmdOut, "+CMGF: 1"))
    return SMS_TXT_MODE;

  return 0xFF;
}
//*****************************************************************************
//  Function        : SetSmsMode
//  Description     : Set SMS mode
//  Input           : aMode;
//  Return          : None;
//  Note            : //08-12-06 JC++
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SetSmsMode(BYTE aMode)
{
  if (aMode > SMS_TXT_MODE)
    return FALSE;


  gGSMData.b_sms_mode = aMode;
  strcpy(gAtCmdIn, "AT+CMGF=0\r\n");
  gAtCmdIn[8] = '0' + aMode;
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  return TRUE;
}
#if (T800)
//*****************************************************************************
//  Function        : KeepSmsStatus
//  Description     : Enable/Disable SMS status change after read.
//  Input           : TRUE: keep status, FALSE: update status
//  Return          : None;
//  Note            : //08-12-06 JC++
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN KeepSmsStatus(BOOLEAN aKeep)
{
  sprintf(gAtCmdIn, "AT+WUSS=%d\r\n", aKeep);
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  return TRUE;
}
#endif
//*****************************************************************************
//  Function        : GetSmsTotal
//  Description     : Get the short form operator name
//  Input           : aUsed : used count
//                    aTotal: available count
//  Return          : None;
//  Note            : //08-12-06 JC++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetSmsTotal(DWORD *aUsed, DWORD *aTotal)
{
  BYTE *ptr;
  DWORD idx1, idx2, r_len;

  ClearCmdBuf();
  strcpy(gAtCmdIn, "AT+CPMS?\r\n");
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  ptr = (BYTE *)strstr((char const *)gAtCmdOut, "+CPMS:");
  if (ptr == NULL)
    return FALSE;

  r_len = strlen(gAtCmdOut) - (ptr - (BYTE *)gAtCmdOut);

  idx1 = fndb(ptr, ',', r_len);             // find first comma
  if (idx1 == r_len)
    return FALSE;

  r_len = r_len - idx1;
  idx2 = fndb(ptr+idx1+1, ',', r_len);      // find second comma
  if (idx2 == r_len)
    return FALSE;

  idx2 = idx1 + idx2 + 1;

  *aUsed =  (WORD)Asc2DWVAL(ptr+idx1+1, 3);
  *aTotal = (WORD)Asc2DWVAL(ptr+idx2+1, 3);

  *aTotal = *aTotal > MAX_SMS_REC ? MAX_SMS_REC: *aTotal;

  return TRUE;
}
//*****************************************************************************
//  Function        : GetSmsInfo
//  Description     : Get tel number and name
//  Input           : *tel;     // ptr to tel number buffer
//                  : *name;    // ptr to name buffer
//                  : *s_ptr;   // starting ptr to search
//                  : len;      // length of string to search
//  Return          : 0;        // Not found
//                    1;        // TEL READY
//                    2;        // TEL & NAME ready
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
static BOOLEAN GetSmsInfo(struct SMS_LIST *msg, BYTE *s_ptr, DWORD len)
{
#if (T300|T810|A5T1000|T1000|TIRO|T800)
  BYTE sts_buf[12];
  BYTE sts_len, tel_len, dtime_len;
  BYTE *ptr;
  BYTE state;

  sts_len = tel_len = dtime_len = 0;
  state = 0;
  ptr = strstr(s_ptr, "+CMGR:");
  while ((DWORD)(ptr - s_ptr) < len) {
    if (*ptr == '\"') {
      // break if end of capture date/time
      if (state >= 7)
        break;
      state++;
      ptr++;
      continue;
    }
    switch (state) {
      case 1:     // message status
        sts_buf[sts_len++] = *ptr;
        break;
      case 3:     // telephone number
        if (tel_len < MAX_PHNUM_SIZE)
          msg->sb_tel[tel_len++] = *ptr;
        break;
      case 7:     // date / time
        if (dtime_len < sizeof(msg->sb_date_time))
          msg->sb_date_time[dtime_len++] = *ptr;
        break;
      default:
        break;
    }
    ptr++;
  }
  // decode message status
  if (!memcmp(sts_buf, "REC UNREAD", 10))
    msg->b_status = REC_UNREAD;
  else if (!memcmp(sts_buf, "REC READ", 8))
    msg->b_status = REC_READ;
  else if (!memcmp(sts_buf, "STO UNSENT", 10))
    msg->b_status = STO_UNSENT;
  else if (!memcmp(sts_buf, "STO SENT", 8))
    msg->b_status = STO_SENT;
  else
    return FALSE;

  // no tel. or date/time found
  if (!tel_len || !dtime_len)
    return FALSE;

  return TRUE;
#endif
}
//*****************************************************************************
//  Function        : SortSmsList
//  Description     : Sort the SMS List by time.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SortSmsList(void)
{
  WORD i, j;

  // Sort Record
  for (i=0; i<gGSMData.d_sms_used-1; i++) {
    for (j=i+1; j<gGSMData.d_sms_used; j++) {
      if (memcmp(gGSMData.s_SMSList[i].sb_date_time, gGSMData.s_SMSList[j].sb_date_time, SMS_DATE_LEN+SMS_TIME_LEN+1)>=0)
        continue;
      memcpy((BYTE *)&gGSMData.s_CurrSMS, (BYTE *)&gGSMData.s_SMSList[j], sizeof(struct SMS_LIST));
      memcpy((BYTE *)&gGSMData.s_SMSList[j], (BYTE *)&gGSMData.s_SMSList[i], sizeof(struct SMS_LIST));
      memcpy((BYTE *)&gGSMData.s_SMSList[i], (BYTE *)&gGSMData.s_CurrSMS, sizeof(struct SMS_LIST));
    }
  }
}
//*****************************************************************************
//  Function        : LoadSmsInbox
//  Description     : Load SMS messages
//  Input           : N/A
//  Return          : None;
//  Note            : //08-12-06 JC++
//  Globals Changed : N/A
//*****************************************************************************
void LoadSmsInbox(void)
{
  char *ptr;
  BYTE i;
  WORD count;

  ClearCmdBuf();
  GetSmsTotal(&gGSMData.d_sms_used, &gGSMData.d_sms_total);
#if (T800)
  KeepSmsStatus(TRUE);        // keep message status after LIST command
#endif
  if (gGSMData.b_sms_mode == SMS_PDU_MODE)
    SetSmsMode(SMS_TXT_MODE);    // set TEXT mode for getting date/time info

  memset(gGSMData.s_SMSList, 0, sizeof(gGSMData.s_SMSList));
  gGSMData.d_sms_recv = count = 0;

  for (i=1; i<=gGSMData.d_sms_total; i++) {
    // send READ command to read every message
    sprintf(gAtCmdIn, "AT+CMGR=%d\r\n", i);
    if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
      continue;
    // find status info from each message
    ptr = strstr((char const *)gAtCmdOut, "+CMGR:");
    if (ptr == NULL)
      continue;
    count++;
    // get SMS info (date/time & tel no)
    if (GetSmsInfo(&gGSMData.s_SMSList[gGSMData.d_sms_recv], ptr, strlen(ptr))==FALSE)
      continue;
    // update list for READ/UNREAD messages only
    if ((gGSMData.s_SMSList[gGSMData.d_sms_recv].b_status != REC_UNREAD) &&
        (gGSMData.s_SMSList[gGSMData.d_sms_recv].b_status != REC_READ))
      continue;

    gGSMData.s_SMSList[gGSMData.d_sms_recv].d_idx = i;
    gGSMData.d_sms_recv++;

    if (count >= gGSMData.d_sms_used)
      break;

    if (gGSMData.d_sms_recv >= MAX_SMS_REC)
      break;
  }

  if (gGSMData.b_sms_mode == SMS_PDU_MODE)
    SetSmsMode(SMS_PDU_MODE);
#if (T800)  
  KeepSmsStatus(FALSE);
#endif  
  SortSmsList();  // Sort by time
}
//*****************************************************************************
//  Function        : ReadSms
//  Description     : Read SMS from memory
//  Input           : aMsg;     // pointer to sturct SMS_MSG
//                    aListId;  // List Idx
//  Return          : None;
//  Note            : //08-12-06 JC++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ReadSms(struct SMS_MSG *aMsg, DWORD aListId)
{
  struct SMS_PDU sms_pdu;
  BYTE tmpstr[256];
  DWORD len;
  struct GPRSSMSDATA sms_data;
  BYTE *ptr, *mptr;

  sms_data.s_pdu = &sms_pdu;
  sms_data.s_str = tmpstr;
  sms_data.d_len = &len;  

  ClearCmdBuf();
  if (gGSMData.b_sms_mode == SMS_TXT_MODE)
    SetSmsMode(SMS_PDU_MODE);    // set PDU mode for reading/decoding SMS

  // send READ command to read every message
  sprintf(gAtCmdIn, "AT+CMGR=%d\r\n", gGSMData.s_SMSList[aListId].d_idx);
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK))
    return FALSE;

  // find status info from each message
  ptr = strstr((char const *)gAtCmdOut, "+CMGR:");
  if (ptr == NULL)
    return FALSE;

  // change status from UNREAD to READ
  if (gGSMData.s_SMSList[aListId].b_status == REC_UNREAD)
    gGSMData.s_SMSList[aListId].b_status = REC_READ;

  mptr = strchr(ptr, '\n');
  if (mptr == NULL)
    return FALSE;

  mptr++;
  len = strlen(gAtCmdOut) - (mptr - ptr);
  strcpy(sms_data.s_str, mptr);
  if (IOCtlMW(gGprsHandle, IO_GPRS_SMS_DECODE, &sms_data)) {
    aMsg->sb_content[0] = sms_pdu.b_tp_udl;
    memcpy(&aMsg->sb_content[1], sms_pdu.sb_tp_ud, sms_pdu.b_tp_udl);
  }
  else
    return FALSE;

  memcpy(&aMsg->d_idx, &gGSMData.s_SMSList[aListId].d_idx, (BYTE *)&aMsg->b_info-(BYTE *)aMsg);
  aMsg->b_info = sms_pdu.b_tp_dcs;

  if (gGSMData.b_sms_mode == SMS_TXT_MODE)
    SetSmsMode(SMS_TXT_MODE);

  return TRUE;
}
//*****************************************************************************
//  Function        : DelSms
//  Description     : Delete SMS from memory
//  Input           : aIdx:         // SMS index in memory
//                                  // -1 => Delete All
//  Return          : None;
//  Note            : //08-12-06 JC++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DelSms(DWORD aIdx)
{
  if (aIdx != (DWORD) -1)
    sprintf(gAtCmdIn, "AT+CMGD=%d\r\n", aIdx);
  else
#if (T800)      
    sprintf(gAtCmdIn, "AT+CMGD=1,4\r\n", aIdx);
#else  
    sprintf(gAtCmdIn, "AT+CMGD=0,4\r\n");
#endif    

  ClearCmdBuf();
  if (!SendWaitRsp(gAtCmdIn, WAIT_OK)) {
    DispLineMW("Del ERR!", MW_LINE8, MW_SPFONT);
    APM_WaitKey(KBD_TIMEOUT, 0);
    return FALSE;
  }

  return TRUE;
}
//*****************************************************************************
//  Function        : SmsSend
//  Description     : Send out the SMS message.
//  Input           : aTel;       // pointer to telephone string.
//                    aMsg;       // pointer SMS message buf
//                    aLen;       // SMS message buffer len;
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SmsSend(BYTE *aTel, BYTE *aMsg, DWORD aLen)
{
  struct SMS_PDU sms_pdu;
  BYTE tmpstr[256];
  DWORD len;
  struct GPRSSMSDATA sms_data;

  sms_data.s_pdu = &sms_pdu;
  sms_data.s_str = tmpstr;
  sms_data.d_len = &len;  
  sms_data.d_dest = MW_SMS_DEST;
  sms_data.d_text_mode = SMS_PDU_MODE;

  if ((strlen(aTel) > sizeof(sms_pdu.s_tp_a.sb_addr)) 
      || (aLen > sizeof(sms_pdu.sb_tp_ud))) { // tel no or msg too long
    return FALSE;
  }
  sms_pdu.s_smsc.b_len = 0;
  if (gGSMData.b_sms_mode == SMS_PDU_MODE) {
    sms_pdu.b_tp_flags    = SMS_TP_MTI_SMS_SUBMIT;
    sms_pdu.b_tp_pid      = 0;               // Short Message
    //sms_pdu.b_tp_dcs      = 0x04;            // 8 bit coding
    sms_pdu.b_tp_dcs      = 0;               // 7 bit coding
    sms_pdu.s_tp_a.b_type = (aTel[0]=='+')? 0x91:0x81;     // international or unknown mode
  }
  sms_pdu.s_tp_a.b_len = strlen(aTel);
  if (aTel[0] == '+') {
    sms_pdu.s_tp_a.b_len -= 1;
    memcpy(sms_pdu.s_tp_a.sb_addr, &aTel[1], sms_pdu.s_tp_a.b_len);
  }
  else
    memcpy(sms_pdu.s_tp_a.sb_addr, aTel, sms_pdu.s_tp_a.b_len);


  sms_pdu.b_tp_udl = (BYTE) aLen;
  memcpy(sms_pdu.sb_tp_ud, aMsg, sms_pdu.b_tp_udl);
  return IOCtlMW(gGprsHandle, IO_GPRS_SMS_SEND, &sms_data);
//#if (T800)
//  return os_gprs_sms_send(gGSMData.b_sms_mode, &gSmsPdu, K_SmsDestination);
//#else
//  return os_gprs_sms_send(&sms_pdu, (gGSMData.b_sms_mode << 4));
//#endif  
}
//*****************************************************************************
//  Function        : ModuleInit
//  Description     : Initialize of the Module
//  Input           : None;
//  Return          : TRUE/FALSE;
//  Note            : - Caller ID Enable = TRUE;
//                    - SMS in PDU Mode
//                    - SMS new indication = FALSE;
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ModuleInit(void)
{
  // SMS enable
  IOCtlMW(gGprsHandle, IO_GPRS_SMS_ENB, FALSE);
  
  // Get Operator Name
  gGSMData.sb_optr_name[0]=0;
  GetOptrName(gGSMData.sb_optr_name, OPTR_NAME_SIZE);
  // Set PDU mode as default, 22-01-09 ++ Jason ++
  if (!SetSmsMode(SMS_PDU_MODE))
    return FALSE;

  return TRUE;
}
#endif // (T800|TIRO)
