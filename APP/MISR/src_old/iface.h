//-----------------------------------------------------------------------------
//  File          : iface.h
//  Module        :
//  Description   : header for iface_spt3.c
//  Author        : Pody
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g                                         |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |            Char size : Leading c                                         |
// |             Int size : Leading i                                         |
// |            Byte size : Leading b                                         |
// |            Word size : Leading w                                         |
// |           Dword size : Leading d                                         |
// |          DDword size : Leading dd                                        |
// |          Struct size : Leading s                                         |
// |                Array : Leading a, (ab = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  2008 Apr      Pody        Initial Version.
//-----------------------------------------------------------------------------


#ifndef _IFACE_H_
#define _IFACE_H_


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
// mode - bitwise
#define S3MODE_SETUP_ALLOW        (BYTE)0x01
#define S3MODE_TRANS_ALLOW        (BYTE)0x02

// instruction class
#define S3INS_CLASS_MASK      (BYTE)0xF0
#define S3INS_CLASS_GENERAL     (BYTE)0x00
#define S3INS_CLASS_SETUP       (BYTE)0x10
#define S3INS_CLASS_TRANS       (BYTE)0x20
// general commands
#define S3INS_ECHO            (BYTE)(S3INS_CLASS_GENERAL | 0)
#define S3INS_INIT_MODE       (BYTE)(S3INS_CLASS_GENERAL | 1)
#define S3INS_INIT_AUTH       (BYTE)(S3INS_CLASS_GENERAL | 2)
#define S3INS_MUTU_AUTH       (BYTE)(S3INS_CLASS_GENERAL | 3)
#if (IFACE_PP300) //PC1112
  #define S3INS_PP300           (BYTE)(S3INS_CLASS_GENERAL | 4)
#endif //(IFACE_PP300) //PC1112
#define S3INS_RDR2POS         (BYTE)(S3INS_CLASS_GENERAL | 5) //handle reader to pos
  //Data format = Type(4)|Tlv(x)
  #define R2P_TYPE_DE             (DWORD)0
  #define R2P_TYPE_MSG            (DWORD)1
  #define R2P_TYPE_OUT            (DWORD)2
#if (IFACE_SPAD)
  #define S3INS_SPAD            (BYTE)(S3INS_CLASS_GENERAL | 6)       // !LC Signature Pad DLL Interface  
#endif
// setup commands
#define S3INS_GEN_KEY         (BYTE)(S3INS_CLASS_SETUP | 0)
#define S3INS_ESETUP          (BYTE)(S3INS_CLASS_SETUP | 1)
#define S3INS_PARA_PUT        (BYTE)(S3INS_CLASS_SETUP | 2)
#define S3INS_PARA_GET        (BYTE)(S3INS_CLASS_SETUP | 3)
#define S3INS_ETAG_PUT        (BYTE)(S3INS_CLASS_SETUP | 4)
#define S3INS_ETAG_GET        (BYTE)(S3INS_CLASS_SETUP | 5)
#if (BUILD_MODE == 0xF0) //PC1112 ChinaReleaseMode
  #define S3INS_EMVSETUP        (BYTE)(S3INS_CLASS_SETUP | 6)  //++zx 111031 PC1112 for China
  #define S3INS_EMVCALL         (BYTE)(S3INS_CLASS_SETUP | 7)  //++zx 111031 PC1112 for China
  #define S3INS_ICCCMD          (BYTE)(S3INS_CLASS_SETUP | 8)  //++zx 111031 PC1112 for China
  #define S3INS_TAGCARD         (BYTE)(S3INS_CLASS_SETUP | 9)  //++zx 111101 PC1112 for China
#endif //(BUILD_MODE == 0xF0) //PC1112 ChinaReleaseMode
#define S3INS_SYS_FUNC          (BYTE)(S3INS_CLASS_SETUP | 10)  //++PC1202 ECR12-0005 add mifare func
// instruction
#define S3INS_TRANS           (BYTE)(S3INS_CLASS_TRANS | 0)  //ready for sale
#define S3INS_RESET           (BYTE)(S3INS_CLASS_TRANS | 1)  //reset
#define S3INS_SHOW_STAT       (BYTE)(S3INS_CLASS_TRANS | 2)  //show status
#define S3INS_FULL_EMV        (BYTE)(S3INS_CLASS_TRANS | 3)  //!2011-09 Full EMV Trans
#define S3INS_FEMV_CMD        (BYTE)(S3INS_CLASS_TRANS | 4)  //!2011-09 Full EMV Kernel Cmd

// rsp code
#define S3RC_OK               (BYTE)0x00
#define S3RC_CANCEL           (BYTE)0x40
#define S3RC_TIMEOUT          (BYTE)0x41
#define S3RC_MORE_CARDS       (BYTE)0x42
#define S3RC_ERR              (BYTE)0x80
#define S3RC_ERR_CSUM         (BYTE)0x81
#define S3RC_ERR_DATA         (BYTE)0x82
#define S3RC_ERR_FMT          (BYTE)0x83
#define S3RC_ERR_MEM          (BYTE)0x84
#define S3RC_ERR_KEY          (BYTE)0x85
#define S3RC_ERR_INS          (BYTE)0x86
#define S3RC_ERR_KVC          (BYTE)0x87
#define S3RC_ERR_SEQ          (BYTE)0x88
#define S3RC_ERR_PREL         (BYTE)0x89

// data encryption mode (4-bits)
#define S3DFMT_PLAIN          (BYTE)0x00
#define S3DFMT_ECB_DES        (BYTE)0x01
#define S3DFMT_ECB_TDES       (BYTE)0x02

// iface return value
#define IF_OK                     (int)0
#define IF_ERROR                  (int)-1
#define IF_CANCEL                 (int)-2
#define IF_ERR_FMT                (int)-3
#define IF_ERR_MEM                (int)-4
#define IF_ERR_COM                (int)-5
#define IF_ERR_INS                (int)-6
#define IF_ERR_CRC                (int)-7
#define IF_ERR_SEQ                (int)-8
#define IF_ERR_KEY                (int)-9

// key struct
#define S3KEY_INJECTED             (BYTE)0x01  //S3KEY bStatus

extern struct IFACE_DATA *gIfaceData;

// Tags defines follow 8583 0 => 7F
#define ITFTAG_PAN              0x5A        // packed BCD
#define ITFTAG_TXNAMT           0x9F02      // packed BCD
#define ITFTAG_AID              0x9F06      // packed BCD
#define ITFTAG_TRACE            0x9F41      // packed BCD
#define ITFTAG_TXNTIME          0x9F21      // packed BCD HHMMSS
#define ITFTAG_TXNDATE          0x9A        // packed BCD MMDD
#define ITFTAG_EXPDATE          0x5F24      // packed BCD YYMM
#define ITFTAG_AUTH_RESP        0x89        // ASCII
#define ITFTAG_RESP_CODE        0x8A        // ASCII
#define ITFTAG_TERMID           0x9F1C      // ASCII
#define ITFTAG_MERCHID          0x9F16      // ASCII
#define ITFTAG_ADDAMT           0x9F03      // packed BCD
#define ITFTAG_CARDHOLDERNAME   0x5F20      // ASCII[26];
#define ITFTAG_ENTRYMODE        0x9F39      // ASCII
#define ITFTAG_RRN              0xDF25      // ASCII
#define ITFTAG_BATCHNO          0xDF3C      // packed BCD

// Tags User define
#define ITFTAG_ECRREF           0xDF81      // ASCII[16];
#define ITFTAG_HOSTIDX          0xDF83      // binary
#define ITFTAG_HOSTNAME         0xDF84      // ASCII[10]
#define ITFTAG_HOSTTOT          0xDF85      //
#define ITFTAG_TXNTYPE          0xFF00      //

#define MAX_IFACE_BUF        MAX_ECR_BUF

typedef struct {
  BYTE abDKey[16];
  BYTE bType;
  BYTE bIdx;
  BYTE bRfu;
  BYTE bStatus;   //b0: inject or not; other RFU
} S3KEY;

// protected data struct
typedef struct {
  S3KEY sMrmk;          // Master Reader Message Key
  S3KEY sRmk;           // Reader Message Key
} S3PDATA;

struct IFACE_DATA {
  S3PDATA   sFS3PData;                  // S3 pdata
};

enum {
  TXN_CANCEL=0,
  TXN_ERR,
  TXN_OK
};

//----------------------------------------------------------------------------
// Constant
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Globals
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Generic API
//----------------------------------------------------------------------------
extern int Spt3MsgHdr(BYTE *aInOutMsg, WORD aInLen);
extern void IfaceInitData(void);
extern int IfaceProcess(void);

//-----------------------------------------------------------------------------
#endif //_IFACE_SPT3_H_



