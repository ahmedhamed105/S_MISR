//-----------------------------------------------------------------------------
//  File          : wavecom.h
//  Module        :
//  Description   : Declrartion & Defination for WAVECOM.C
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
//  01 Apr  2009  Lewis       - Port from Creon (Lastest copy from John)
//                            - Apply new naming covention.
//-----------------------------------------------------------------------------
#ifndef _INC_WAVECOM_H_
#define _INC_WAVECOM_H_
#include "common.h"

// Module Status
#if (T800)
#define CMDTX_READY          K_GsmAtCmdReady         // AT command send ready
#define CMDRX_READY          K_GsmAtRespReady        // AT command response ready
#define AT_READY1            K_GsmAtReady1           // initialized, ready to accept AT commands except photobooks, AOC, SMS
#define AT_READY2            K_GsmAtReady2           // ready to accept all AT commands
#define NEW_SMS_MSG          K_GsmSmsNewMsg          // sms NEW message, will be cleared if read if SMS in Q is empty
#endif 

#if (TIRO)
#define CMDTX_READY          K_GPRS_STATUS_TXRDY1        // AT command send ready
#define CMDRX_READY          K_GPRS_STATUS_RXRDY1        // AT command response ready
#define AT_READY1            K_GPRS_STATUS_READY         // initialized, ready to accept AT commands except photobooks, AOC, SMS
#define AT_READY2            (K_GPRS_STATUS_NETWORK_READY|K_GPRS_STATUS_PHONEBOOK_READY|K_GPRS_STATUS_SMS_READY|K_GPRS_STATUS_CBM_READY) // ready to accept all AT commands
#define NEW_SMS_MSG          K_GPRS_STATUS_SMSRXRDY
#endif

//----------------------------------------------------------------------
//       Calling State
//----------------------------------------------------------------------
enum {
  NOT_READY         ,
  NO_ANSWER         ,
  CONNECTED         ,
  NO_CARRIER        ,
  BUSY              ,
  CONNECTING        ,
};

//----------------------------------------------------------------------
//       Pin State
//----------------------------------------------------------------------
#define REQ_READY     0
#define REQ_PIN1      1
#define REQ_PUK1      2
#define REQ_PIN2      3
#define REQ_PUK2      4
#define PIN_ERROR     0xFF

//----------------------------------------------------------------------
//       Global Variables
//----------------------------------------------------------------------
#define MAX_PHBOOK_REC      256
#define OPTR_NAME_SIZE      20
#define MAX_NAME_SIZE       31
#define MAX_PHNUM_SIZE      31
#define MAX_SMS_REC         100
#define MAX_SMS_PSIZE       112     // 16*7 characters
#define MAX_SMS_SIZE        160
#define SMS_DATE_LEN        8
#define SMS_TIME_LEN        8

// Phone Book Type
#define PHB_SIM             0x00
#define PHB_MISSCALL        0x01
#define PHB_DIALOUT         0x02
#define PHB_RCVD            0x03

//08-12-06 JC ++
// coding scheme for SMS
#define C7BIT             0
#define C8BIT             1

// SMS Type
#define REC_UNREAD        0
#define REC_READ          1
#define STO_UNSENT        2
#define STO_SENT          3
#define LIST_ALL          4
#define DELETED           0xFF
//08-12-06 JC --

struct PHONE_LIST {
  char sb_name[MAX_NAME_SIZE+1];
  char sb_tel[MAX_PHNUM_SIZE+1];
  DWORD d_idx;
};

struct SMS_LIST {
  DWORD d_idx;
  BYTE sb_date_time[22];
  BYTE sb_tel[MAX_PHNUM_SIZE+1];
  BYTE b_status;
};

struct SMS_MSG {
  DWORD d_idx;
  BYTE  sb_date_time[22];
  BYTE  sb_tel[MAX_PHNUM_SIZE+1];
  BYTE  b_info;                          // dcs (data coding scheme)
  BYTE  sb_content[MAX_SMS_SIZE+1];      // content[0] = message len
};

struct GSM_DATA {
  volatile  DWORD d_call_status;
  char   sb_optr_name[OPTR_NAME_SIZE+1];
  DWORD  d_max_phone_len;
  DWORD  d_max_name_size;
  DWORD  d_total_count;               // phone book rec total count
  DWORD  d_used_count;
  BYTE   b_sms_mode;                  // message format (TEXT or PDU)
  DWORD  d_sms_total;                 // SMS rec total count in memory
  DWORD  d_sms_used;                  // SMS used count in memory
  DWORD  d_sms_recv;                  // SMS received count in memory
  struct PHONE_LIST s_PHBook[MAX_PHBOOK_REC];
  struct PHONE_LIST s_CurrPHB;
  struct SMS_LIST   s_SMSList[MAX_SMS_REC];     //08-12-06 JC ++
  struct SMS_MSG    s_CurrSMS;                  //08-12-06 JC ++
};

extern struct GSM_DATA gGSMData;
//extern T_SMS_PDU gSmsPdu;


//----------------------------------------------------------------------
//       Functions
//----------------------------------------------------------------------
// Common Module related API
extern BOOLEAN WaitModuReady(DWORD aTimeout);
extern BOOLEAN SimDetect(void);
extern BYTE RegisterNetwork(BYTE aGPRS);
extern BYTE AttachGPRS(void);
extern BOOLEAN ModuleInit(void);

// Voice Call related API
extern BOOLEAN GetOptrName(char *aName, DWORD aMaxLen);
extern BOOLEAN AllCallForward(char *aTel, BOOLEAN aCancel);
extern BOOLEAN MakeCall(char *aPhoneNum);
extern BOOLEAN CallStatusEnb(DWORD aEnable);
extern BOOLEAN MakeReqCall(char *aPhoneNum);
extern DWORD CheckCallStatus(void);
extern BOOLEAN AnswerCall(void);
extern BOOLEAN HangUp(void);
extern BOOLEAN GetCallerID(void);
extern BOOLEAN CheckCallEnd(void);
// type for device gain
#define SPK              'R'
#define MIC              'T'
#define MAX_GAIN_VAL      10
extern DWORD ReadGain(DWORD aType);
extern BOOLEAN WriteGain(DWORD aType, DWORD aVal);
extern BOOLEAN SendKeyPress(char aChar);

// Phone Book Related API
extern BOOLEAN GetPHBCapcity(DWORD *aUsed, DWORD *aEmpty);
extern BOOLEAN UpdPhList(void);
extern BOOLEAN SelectPHBook(DWORD aID);
extern BOOLEAN WriteContact(struct PHONE_LIST *aRec, DWORD aCurrRec);
extern BOOLEAN DelContact(DWORD aRecIdx, DWORD aCurrRec);

// PIN related API
extern DWORD FacilityEnable(BOOLEAN aEnable, char *aPin);
extern DWORD CheckPinState(void);
extern BOOLEAN EnterPin(DWORD aId, char *aPin);
extern DWORD ChangePin(DWORD aId, char *aOldPin, char *aNewPin);

// SMS related API
extern BOOLEAN SetNewMsgInd(BOOLEAN aEnable);
extern BOOLEAN GetSmsTotal(DWORD *aUsed, DWORD *aTotal);
extern void LoadSmsInbox(void);
extern BOOLEAN ReadSms(struct SMS_MSG *aMsg, DWORD aIdx);
extern BOOLEAN DelSms(DWORD aIdx);
BOOLEAN SmsSend(BYTE *aTel, BYTE *aMsg, DWORD aLen);

#endif // _INC_WAVECOM_H_
