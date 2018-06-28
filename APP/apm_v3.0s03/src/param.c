//-----------------------------------------------------------------------------
//  File          : param.c
//  Module        :
//  Description   : Graphic Libary Test Program.
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
//  08 Jul  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "midware.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "util.h"
#include "stis.h"
#include "emv2dll.h"
#include "emvcl2dll.h"
#include "emvtrans.h"
#include "ftptran.h"
#include "mxml.h"
#include "termdata.h"
#include "ctlcfg.h"
#include "toggle.h"
#include "comm.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
enum {
  ST_TERM_CLEAR=0,
  ST_TERM_CFG,
  ST_TERM_ACQ,
  ST_TERM_ISSUER,
  ST_TERM_CARD,
  ST_EMV_APP,
  ST_CTL_APP,
  ST_EMV_KEY,
  ST_END,
};
enum {
  TYPE_ASC,
  TYPE_VAL,
  TYPE_HEX,
  TYPE_DDWORD,
};

struct XML_PARAM {
  BYTE *sbXmlTag;
  BYTE bType;
  WORD wSize;
};

typedef struct {
  BYTE      b_key_type;
  KEY_ROOM  s_key_room;
} KEY_ROOM_T;

//-----------------------------------------------------------------------------
// Constant
//-----------------------------------------------------------------------------
const struct XML_PARAM TermCfgParam[] = {
  { "dll",                  TYPE_VAL,   1   },        //BYTE b_dll;
  { "init_control",         TYPE_VAL,   1   },        //BYTE b_init_control;
  { "rsvd0",                TYPE_HEX,   1   },        //BYTE b_rsvd0;
  { "rsvd1",                TYPE_HEX,   1   },        //BYTE b_rsvd1;
  { "date_time",            TYPE_HEX,   6   },        //BYTE sb_date_time[6];
  { "dial_option",          TYPE_HEX,   1   },        //BYTE b_dial_option;
  { "password",             TYPE_ASC,   2   },        //BYTE sb_password[2];
  { "help_tel",             TYPE_ASC,   12  },        //BYTE sb_help_tel[12];
  { "Option-TermCfg",       TYPE_HEX,   4   },        //BYTE sb_options[4];
  { "NameLoc",              TYPE_ASC,   46  },        //BYTE sb_name_loc[46];
  { "MerchantName",         TYPE_ASC,   23  },        //BYTE sb_dflt_name[23];
  { "CurrSym",              TYPE_ASC,   1   },        //BYTE b_currency;
  { "TransAmntLen",         TYPE_VAL,   1   },        //BYTE b_trans_amount_len;
  { "DecPoint",             TYPE_VAL,   1   },        //BYTE b_decimal_pos;
  { "rsvd2",                TYPE_HEX,   1   },        //BYTE b_rsvd2;
  { "SettAmntLen",          TYPE_VAL,   1   },        //BYTE b_settle_amount_len;
  { "CurrName",             TYPE_ASC,   3   },        //BYTE sb_currency_name[3];
  { "local_option",         TYPE_HEX,   1   },        //BYTE b_local_option;
  { "additional_prompt",    TYPE_ASC,   20  },        //BYTE sb_additional_prompt[20];
  { "Reserved-TermCfg",     TYPE_HEX,   30  },        //BYTE sb_reserved[30];
  { "PswRefund",            TYPE_ASC,   16  },        //BYTE sb_psw_refund[16];
  { "PswVoid",              TYPE_ASC,   16  },        //BYTE sb_psw_void[16];
  { "apn_username",         TYPE_ASC,   32  },        //BYTE apn_username[32];
  { "apn_password",         TYPE_ASC,   32  },        //BYTE apn_password[32];
  { "apn2_username",        TYPE_ASC,   32  },        //BYTE apn2_username[32];
  { "apn2_password",        TYPE_ASC,   32  },        //BYTE apn2_password[32];
  { "apnconfig",            TYPE_ASC,   32  },        //BYTE apnconfig[32];
  { "apnconfig2",           TYPE_ASC,   32  },        //BYTE apnconfig2[32];
  { "apn2_ip",              TYPE_HEX,   4   },        //BYTE apn2_ip[4];
  { "apn2_port",            TYPE_HEX,   2   },        //BYTE apn2_port[2];
  { NULL,                   TYPE_ASC,   0   },
};

const struct XML_PARAM AcqTableParam[] = {
  { "ID",                   TYPE_VAL,   1   },        //BYTE b_id;
  { "ProgName",             TYPE_ASC,   10  },        //BYTE sb_program[10];
  { "AcqName",              TYPE_ASC,   10  },        //BYTE sb_name[10];
  { "PrimTelNo",            TYPE_HEX,   12  },        //BYTE sb_pri_trans_tel[12];
  { "PrimTelConnectTO",     TYPE_VAL,   1   },        //BYTE b_pri_trans_conn_time;
  { "PrimTelDialAttempt",   TYPE_VAL,   1   },        //BYTE b_pri_trans_redial;
  { "SecTelNo",             TYPE_HEX,   12  },        //BYTE sb_sec_trans_tel[12];
  { "SecTelConnectTO",      TYPE_VAL,   1   },        //BYTE b_sec_trans_conn_time;
  { "SecTelDialAttempt",    TYPE_VAL,   1   },        //BYTE b_sec_trans_redial;
  { "pri_settle_tel",       TYPE_HEX,   12  },        //BYTE sb_pri_settle_tel[12];
  { "pri_settle_conn_time", TYPE_VAL,   1   },        //BYTE b_pri_settle_conn_time;
  { "pri_settle_redial",    TYPE_VAL,   1   },        //BYTE b_pri_settle_redial;
  { "sec_settle_tel",       TYPE_HEX,   12  },        //BYTE sb_sec_settle_tel[12];
  { "sec_settle_conn_time", TYPE_VAL,   1   },        //BYTE b_sec_settle_conn_time;
  { "sec_settle_redial",    TYPE_VAL,   1   },        //BYTE b_sec_settle_redial;
  { "ModemMode",            TYPE_VAL,   1   },        //BYTE b_trans_mdm_mode;
  { "Option-Acquirer",      TYPE_HEX,   4   },        //BYTE sb_options[4];
  { "NII",                  TYPE_HEX,   2   },        //BYTE sb_nii[2];
  { "TermID",               TYPE_ASC,   8   },        //BYTE sb_term_id[8];
  { "MerchantID",           TYPE_ASC,   15  },        //BYTE sb_acceptor_id[15];
  { "HostRspTO",            TYPE_VAL,   1   },        //BYTE b_timeout_val;
  { "CurrBatchNo",          TYPE_HEX,   3   },        //BYTE sb_curr_batch_no[3];
  { "NextBatchNo",          TYPE_HEX,   3   },        //BYTE sb_next_batch_no[3];
  { "visa1_term_id",        TYPE_ASC,   23  },        //BYTE sb_visa1_term_id[23];
  { "settle_mdm_mode",      TYPE_VAL,   1   },        //BYTE b_settle_mdm_mode;
#if 1
  { "Reserved-Acquirer",    TYPE_HEX,   30  },        //BYTE sb_reserved[21];
#else
  { "Reserved-Acquirer",    TYPE_HEX,   21  },        //BYTE sb_reserved[21];
  { "host_type",            TYPE_HEX,   1   },        //BYTE b_host_type;
  { "reserved1",            TYPE_HEX,   1   },        //BYTE b_reserved1;
  { "ssl_key_idx",          TYPE_HEX,   1   },        //BYTE b_ssl_key_idx;
  { "ip",                   TYPE_HEX,   4   },        //BYTE sb_ip[4];
  { "port",                 TYPE_HEX,   2   },        //BYTE sb_port[2];
#endif
  { NULL,                   TYPE_ASC,   0   },
};

const struct XML_PARAM IssuerTableParam[] = {
  { "ID",                   TYPE_VAL,   1   },        //BYTE b_id;
  { "IssName",              TYPE_ASC,   10  },        //BYTE sb_card_name[10];
  { "ref_tel_no",           TYPE_ASC,   12  },        //BYTE sb_ref_tel_no[12];
  { "Option-Issuer",        TYPE_HEX,   4   },        //BYTE sb_options[4];
  { "DefaultAcc",           TYPE_HEX,   1   },        //BYTE b_default_acc;
  { "pan_format",           TYPE_HEX,   2   },        //BYTE sb_pan_format[2];
  { "FlrLimit",             TYPE_HEX,   2   },        //BYTE sb_floor_limit[2];
  { "Reserved-Issuer",      TYPE_HEX,   20  },        //BYTE sb_reserved[20];
  { NULL,                   TYPE_ASC,   0   },
};

const struct XML_PARAM CardTableParam[] = {
  { "CardRangeLow",         TYPE_HEX,   5   },        //BYTE sb_pan_range_low[5];
  { "CardRangeHigh",        TYPE_HEX,   5   },        //BYTE sb_pan_range_high[5];
  { "IssuerID",             TYPE_VAL,   1   },        //BYTE b_issuer_id;
  { "AcquirerID",           TYPE_VAL,   1   },        //BYTE b_acquirer_id;
  { "pan_len",              TYPE_VAL,   1   },        //BYTE b_pan_len;
  { "reserved",             TYPE_HEX,   20  },        //BYTE sb_reserved[20];
  { NULL,                   TYPE_ASC,   0   },
};

const struct XML_PARAM EmvAppParam[] = {
  { "eType",                TYPE_HEX,   1   },        //BYTE eType;
  { "eBitField",            TYPE_HEX,   1   },        //BYTE eBitField;
  { "eRSBThresh",           TYPE_VAL,   4   },        //DWORD eRSBThresh;
  { "eRSTarget",            TYPE_VAL,   1   },        //BYTE eRSTarget;
  { "eRSBMax",              TYPE_VAL,   1   },        //BYTE eRSBMax;
  { "eTACDenial",           TYPE_HEX,   5   },        //BYTE eTACDenial[5];
  { "eTACOnline",           TYPE_HEX,   5   },        //BYTE eTACOnline[5];
  { "eTACDefault",          TYPE_HEX,   5   },        //BYTE eTACDefault[5];
  { "eACFG",                TYPE_HEX,   200 },        //BYTE eACFG[APPL_TLEN];
  { NULL,                   TYPE_ASC,   0   },
};

const struct XML_PARAM CtlAppParam[] = {
  { "bEType",               TYPE_HEX,   1   },        //BYTE  bEType;
  { "bBitField",            TYPE_HEX,   1   },        //BYTE  bBitField;            
  { "dRSBThresh",           TYPE_VAL,   4   },        //DWORD dRSBThresh;
  { "bRSTarget",            TYPE_VAL,   1   },        //BYTE  bRSTarget;
  { "bRSBMax",              TYPE_VAL,   1   },        //BYTE  bRSBMax;
  { "abTACDenial",          TYPE_HEX,   5   },        //BYTE  abTACDenial[5];
  { "abTACOnline",          TYPE_HEX,   5   },        //BYTE  abTACOnline[5];
  { "abTACDefault",         TYPE_HEX,   5   },        //BYTE  abTACDefault[5];
  { "abTLV",                TYPE_HEX,   512 },        //BYTE  abTLV[CLCFGP_TLV_LEN];
  { NULL,                   TYPE_ASC,   0   },
};

const struct XML_PARAM EmvKeyParam[] = {
  { "eKeyIdx",              TYPE_HEX,   1   },        //BYTE KeyIdx;
  { "eRID",                 TYPE_HEX,   5   },        //BYTE RID[5];
  { "eExponent",            TYPE_HEX,   4   },        //DWORD Exponent;
  { "eKeySize",             TYPE_VAL,   4   },        //DWORD KeySize;
  { "eKey",                 TYPE_HEX,   256 },        //BYTE Key[256];
  { "eExpiryDate",          TYPE_HEX,   2   },        //WORD ExpiryDate;  // in MMYY
  { "eEffectiveDate",       TYPE_HEX,   2   },        //WORD EffectDate;  // in MMYY
  { "eKeyType",             TYPE_HEX,   1   },        //To distinguish keys for EMV contact & contactless
  { NULL,                   TYPE_ASC,   0   },
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static BYTE gCardIdx;

  
//*****************************************************************************
//  Function        : CheckNodeName
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CheckNodeName(struct XML_PARAM *aXmlParam, BYTE *aName, BYTE *aType, WORD *aSize, WORD *aOffset)
{
  //BYTE buf[32];
  WORD offset=0;
  
  while (aXmlParam->sbXmlTag != NULL) {
    if (!strcmp(aXmlParam->sbXmlTag, aName))
      break;
    offset += aXmlParam->wSize;
    aXmlParam++;
  }
  if (aXmlParam->sbXmlTag != NULL) {
    *aType = aXmlParam->bType;
    *aSize = aXmlParam->wSize;
    *aOffset = offset;
#if 0
    SprintfMW(buf, "ChkF[%d]:\n%s", offset, aXmlParam->sbXmlTag);
    DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
#endif
    return TRUE;
  }
  *aType = 0;
  *aSize = 0;
  *aOffset = 0;
  return FALSE;
}
//*****************************************************************************
//  Function        : UpdateNodeParam
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdateNodeParam(BYTE *aParamAddr, BYTE aType, WORD aMaxSize, BYTE *aParam)
{
  //BYTE buf[128];
  BYTE odd=0;
  DDWORD value;
  WORD len;
  
  len = strlen(aParam);
  if (aType == TYPE_ASC) {
    if (aMaxSize < len)
      len = aMaxSize;
    memset(aParamAddr, ' ', aMaxSize);
    memcpy(aParamAddr, aParam, len);
  }
  else if (aType == TYPE_VAL) {
    value = atoi((char *)aParam);
    if (aMaxSize == 1) {
      // 1 Byte
      *aParamAddr = (BYTE)value;
    }
    else if (aMaxSize == 2) {
      // 2 Bytes (WORD)
      *aParamAddr = (BYTE)value;
      *(aParamAddr+1) = (BYTE)(value>>8);
    }
    else if (aMaxSize == 4) {
      // 4 Bytes (DWORD)
      *aParamAddr = (BYTE)value;
      *(aParamAddr+1) = (BYTE)(value>>8);
      *(aParamAddr+2) = (BYTE)(value>>16);
      *(aParamAddr+3) = (BYTE)(value>>24);
    }
  }
  else if (aType == TYPE_HEX) {
    if (len%2)
      odd = 1;
    len /= 2;
    if (aMaxSize < len)
      len = aMaxSize;
    compress(aParamAddr, aParam, len);
    if (odd && (len < aMaxSize)) {
      aParamAddr[len] = (aParamAddr[len] & 0x0F) | (hex_value(aParam[len*2]) << 4);
    }
  }
  else if (aType == TYPE_DDWORD) {
    if (aMaxSize != 8) {
      return FALSE;
    }
    else {
      value = decbin8b(aParam, aMaxSize);
      *aParamAddr = (BYTE)value;
      *(aParamAddr+1) = (BYTE)(value>>8);
      *(aParamAddr+2) = (BYTE)(value>>16);
      *(aParamAddr+3) = (BYTE)(value>>24);
      *(aParamAddr+4) = (BYTE)(value>>32);
      *(aParamAddr+5) = (BYTE)(value>>40);
      *(aParamAddr+6) = (BYTE)(value>>48);
      *(aParamAddr+7) = (BYTE)(value>>56);
    }
  }
  else
    return FALSE;

#if 0
  SprintfMW(buf, "Upd-%02x[%d|%d]%02X", *aParamAddr, aType, len, aParam[0]);
  DispLineMW(buf, MW_LINE3, MW_CLREOL|MW_SPFONT);
  WaitKey(KBD_TIMEOUT);
#endif
  return TRUE;
}
//*****************************************************************************
//  Function        : UpdateParamEmvApp
//  Description     : Parse node data & update APP_CFG structure.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdateParamEmvApp(APP_CFG *aAppCfg, struct XML_PARAM *aXmlParam, BYTE *aName, BYTE *aContent)
{
  //BYTE buf[64];
  WORD offset=0;
  DWORD value, len;
  
  while (aXmlParam->sbXmlTag != NULL) {
    if (!strcmp(aXmlParam->sbXmlTag, aName))
      break;
    offset += aXmlParam->wSize;
    aXmlParam++;
  }
  if (aXmlParam->sbXmlTag != NULL) {
    //*aType = aXmlParam->bType;
    //*aSize = aXmlParam->wSize;
#if 0
    SprintfMW(buf, "ChkF[%d]:\n%s", offset, aXmlParam->sbXmlTag);
    DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
#endif
    if (!strcmp(aName, "eType"))
      compress(&aAppCfg->eType, aContent, 1);
    else if (!strcmp(aName, "eBitField"))
      compress(&aAppCfg->eBitField, aContent, 1);
    else if (!strcmp(aName, "eRSBThresh")) {
      value = atoi((char *)aContent);
      aAppCfg->eRSBThresh = value;
    }
    else if (!strcmp(aName, "eRSTarget")) {
      value = atoi((char *)aContent);
      aAppCfg->eRSTarget = (BYTE)value;
    }
    else if (!strcmp(aName, "eRSBMax")) {
      value = atoi((char *)aContent);
      aAppCfg->eRSBMax = (BYTE)value;
    }
    else if (!strcmp(aName, "eTACDenial"))
      compress(aAppCfg->eTACDenial, aContent, 5);
    else if (!strcmp(aName, "eTACOnline"))
      compress(aAppCfg->eTACOnline, aContent, 5);
    else if (!strcmp(aName, "eTACDefault"))
      compress(aAppCfg->eTACDefault, aContent, 5);
    else if (!strcmp(aName, "eACFG")) {
      len = strlen(aContent)/2;
      if (len > sizeof(aAppCfg->eACFG))
        len = sizeof(aAppCfg->eACFG);
      compress(aAppCfg->eACFG, aContent, len);
    }
    
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : UpdateParamCtlApp
//  Description     : Parse node data & update CL_CFGP structure.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdateParamCtlApp(CL_CFGP *aAppCfg, struct XML_PARAM *aXmlParam, BYTE *aName, BYTE *aContent)
{
  //BYTE buf[64];
  WORD offset=0;
  DWORD value, len;
  
  while (aXmlParam->sbXmlTag != NULL) {
    if (!strcmp(aXmlParam->sbXmlTag, aName))
      break;
    offset += aXmlParam->wSize;
    aXmlParam++;
  }
  if (aXmlParam->sbXmlTag != NULL) {
    //*aType = aXmlParam->bType;
    //*aSize = aXmlParam->wSize;
#if 0
    SprintfMW(buf, "ChkF[%d]:\n%s", offset, aXmlParam->sbXmlTag);
    DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
#endif
    if (!strcmp(aName, "bEType"))
      compress(&aAppCfg->bEType, aContent, 1);
    else if (!strcmp(aName, "bBitField"))
      compress(&aAppCfg->bBitField, aContent, 1);
    else if (!strcmp(aName, "dRSBThresh")) {
      value = atoi((char *)aContent);
      aAppCfg->dRSBThresh = value;
    }
    else if (!strcmp(aName, "bRSTarget")) {
      value = atoi((char *)aContent);
      aAppCfg->bRSTarget = (BYTE)value;
    }
    else if (!strcmp(aName, "bRSBMax")) {
      value = atoi((char *)aContent);
      aAppCfg->bRSBMax = (BYTE)value;
    }
    else if (!strcmp(aName, "abTACDenial"))
      compress(aAppCfg->abTACDenial, aContent, 5);
    else if (!strcmp(aName, "abTACOnline"))
      compress(aAppCfg->abTACOnline, aContent, 5);
    else if (!strcmp(aName, "abTACDefault"))
      compress(aAppCfg->abTACDefault, aContent, 5);
    else if (!strcmp(aName, "abTLV")) {
      len = strlen(aContent)/2;
      if (len > sizeof(aAppCfg->abTLV))
        len = sizeof(aAppCfg->abTLV);
      compress(aAppCfg->abTLV, aContent, len);
    }
    
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : UpdateParamEmvKey
//  Description     : Parse node data & update KEY_ROOM structure.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdateParamEmvKey(KEY_ROOM_T *aKeyRoom, struct XML_PARAM *aXmlParam, BYTE *aName, BYTE *aContent)
{
  BYTE buf[64];
  WORD offset=0;
  DWORD value, len;
  
  while (aXmlParam->sbXmlTag != NULL) {
    if (!strcmp(aXmlParam->sbXmlTag, aName))
      break;
    offset += aXmlParam->wSize;
    aXmlParam++;
  }
  if (aXmlParam->sbXmlTag != NULL) {
    //*aType = aXmlParam->bType;
    //*aSize = aXmlParam->wSize;
#if 0
    SprintfMW(buf, "ChkF[%d]:\n%s", offset, aXmlParam->sbXmlTag);
    DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
#endif
    if (!strcmp(aName, "eKeyIdx"))
      compress(&aKeyRoom->s_key_room.KeyIdx, aContent, 1);
    else if (!strcmp(aName, "eRID"))
      compress(aKeyRoom->s_key_room.RID, aContent, 5);
    else if (!strcmp(aName, "eExponent")) {
      compress(buf, aContent, 4);
      aKeyRoom->s_key_room.CAKey.Exponent = (buf[0]<<24) + (buf[1]<<16) + (buf[2]<<8) + buf[3];
    }
    else if (!strcmp(aName, "eKeySize")) {
      value = atoi((char *)aContent);
      aKeyRoom->s_key_room.CAKey.KeySize = value;
    }
    else if (!strcmp(aName, "eKey")) {
      len = strlen(aContent)/2;
      if (len > sizeof(aKeyRoom->s_key_room.CAKey.Key))
        len = sizeof(aKeyRoom->s_key_room.CAKey.Key);
      compress(aKeyRoom->s_key_room.CAKey.Key, aContent, len);
    }
    else if (!strcmp(aName, "eExpiryDate"))
      compress((BYTE *)&aKeyRoom->s_key_room.ExpiryDate, aContent, 2);
    else if (!strcmp(aName, "eEffectiveDate"))
      compress((BYTE *)&aKeyRoom->s_key_room.EffectDate, aContent, 2);
    else if (!strcmp(aName, "eKeyType"))
      compress(&aKeyRoom->b_key_type, aContent, 1);
    
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : ParseTermCfg
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
mxml_node_t *ParseTermCfg(mxml_node_t *aTree, mxml_node_t *aCurrNode, mxml_node_t *aEndNode)
{
  BYTE buf[256];
  BYTE *string;
  BYTE n_type;
  WORD n_size, n_offset;
  mxml_node_t *curr_node, *end_node;
  struct TERM_CFG *pTable;

  if ((pTable = (struct TERM_CFG *)MallocMW(sizeof(struct TERM_CFG))) == NULL)
    return FALSE;
  memset(pTable, 0, sizeof(struct TERM_CFG));
  
  end_node = mxmlWalkNext(aCurrNode, aTree, MXML_NO_DESCEND);
  while (1) {
    curr_node = mxmlWalkNext(aCurrNode, aTree, MXML_DESCEND);
    if (curr_node == end_node)
      break;                    // break if end of current level
    aCurrNode = curr_node;
    if (aCurrNode != NULL) {
      if (aCurrNode->type == MXML_TEXT)
        continue;
      if (aCurrNode->type == MXML_ELEMENT) {
        //SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
        string = (BYTE *)mxmlGetText(aCurrNode, NULL);
        if (string != NULL) {
#if 0
          SprintfMW(buf, "n->text[%d]: %s", strlen(string), string);
          DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
#endif
          if (CheckNodeName((struct XML_PARAM *)&TermCfgParam[0], aCurrNode->value.element.name, &n_type, &n_size, &n_offset)) {
            // XML tag found in Term Cfg Table
            UpdateNodeParam((BYTE *)pTable+n_offset, n_type, n_size, string);
          }
        }
      }
    }
    else {
      //DispLineMW("End of Walk!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SMFONT);
      //WaitKey(KBD_TIMEOUT);
      break;
    }
  }
  UpdTermCfg(pTable);
  FreeMW(pTable);
  return aCurrNode;
}
//*****************************************************************************
//  Function        : ParseAcquirer
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
mxml_node_t *ParseAcquirer(mxml_node_t *aTree, mxml_node_t *aCurrNode, mxml_node_t *aEndNode)
{
  //BYTE buf[256];
  BYTE *string;
  BYTE n_type;
  WORD n_size, n_offset;
  mxml_node_t *curr_node, *end_node;
  struct ACQUIRER_TBL *pTable;

  if ((pTable = (struct ACQUIRER_TBL *)MallocMW(sizeof(struct ACQUIRER_TBL))) == NULL)
    return FALSE;
  memset(pTable, 0, sizeof(struct ACQUIRER_TBL));
  memset(pTable->sb_pri_trans_tel, 0xFF, sizeof(pTable->sb_pri_trans_tel));
  memset(pTable->sb_sec_trans_tel, 0xFF, sizeof(pTable->sb_sec_trans_tel));
  memset(pTable->sb_pri_settle_tel, 0xFF, sizeof(pTable->sb_pri_settle_tel));
  memset(pTable->sb_sec_settle_tel, 0xFF, sizeof(pTable->sb_sec_settle_tel));
  
  end_node = mxmlWalkNext(aCurrNode, aTree, MXML_NO_DESCEND);
  while (1) {
    curr_node = mxmlWalkNext(aCurrNode, aTree, MXML_DESCEND);
    if (curr_node == end_node)
      break;                    // break if end of current level
    aCurrNode = curr_node;
    if (aCurrNode != NULL) {
      if (aCurrNode->type == MXML_TEXT)
        continue;
      if (aCurrNode->type == MXML_ELEMENT) {
        //SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
        string = (BYTE *)mxmlGetText(aCurrNode, NULL);
        if (string != NULL) {
#if 0
          SprintfMW(buf, "n->text[%d]: %s", strlen(string), string);
          DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
#endif
          if (CheckNodeName((struct XML_PARAM *)&AcqTableParam[0], aCurrNode->value.element.name, &n_type, &n_size, &n_offset)) {
            // XML tag found in Acquirer Table
            UpdateNodeParam((BYTE *)pTable+n_offset, n_type, n_size, string);
          }
        }
      }
    }
    else {
      //DispLineMW("End of Walk!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SMFONT);
      //WaitKey(KBD_TIMEOUT);
      break;
    }
  }
#if 0
  SprintfMW(buf, "AcqTable->id=%d", pTable->b_id);
  DispLineMW(buf, MW_LINE3, MW_CLREOL|MW_SPFONT);
  WaitKey(KBD_TIMEOUT);
#endif
  UpdAcqTbl(pTable->b_id, pTable);
  FreeMW(pTable);
  return aCurrNode;
}
//*****************************************************************************
//  Function        : ParseIssuer
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
mxml_node_t *ParseIssuer(mxml_node_t *aTree, mxml_node_t *aCurrNode, mxml_node_t *aEndNode)
{
  //BYTE buf[256];
  BYTE *string;
  BYTE n_type;
  WORD n_size, n_offset;
  mxml_node_t *curr_node, *end_node;
  struct ISSUER_TBL *pTable;

  if ((pTable = (struct ISSUER_TBL *)MallocMW(sizeof(struct ISSUER_TBL))) == NULL)
    return FALSE;
  memset(pTable, 0, sizeof(struct ISSUER_TBL));
  
  end_node = mxmlWalkNext(aCurrNode, aTree, MXML_NO_DESCEND);
  while (1) {
    curr_node = mxmlWalkNext(aCurrNode, aTree, MXML_DESCEND);
    if (curr_node == end_node)
      break;                    // break if end of current level
    aCurrNode = curr_node;
    if (aCurrNode != NULL) {
      if (aCurrNode->type == MXML_TEXT)
        continue;
      if (aCurrNode->type == MXML_ELEMENT) {
        //SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
        string = (BYTE *)mxmlGetText(aCurrNode, NULL);
        if (string != NULL) {
#if 0
          SprintfMW(buf, "Iss->text[%d]: %s", strlen(string), string);
          DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
#endif
          if (CheckNodeName((struct XML_PARAM *)&IssuerTableParam[0], aCurrNode->value.element.name, &n_type, &n_size, &n_offset)) {
            // XML tag found in Issuer Table
            UpdateNodeParam((BYTE *)pTable+n_offset, n_type, n_size, string);
          }
        }
      }
    }
    else {
      //DispLineMW("End of Walk!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SMFONT);
      //WaitKey(KBD_TIMEOUT);
      break;
    }
  }
  UpdIssuerTbl(pTable->b_id, pTable);
  FreeMW(pTable);
  return aCurrNode;
}
//*****************************************************************************
//  Function        : ParseCard
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
mxml_node_t *ParseCard(mxml_node_t *aTree, mxml_node_t *aCurrNode, mxml_node_t *aEndNode)
{
  //BYTE buf[256];
  BYTE *string;
  BYTE n_type;
  WORD n_size, n_offset;
  mxml_node_t *curr_node, *end_node;
  struct CARD_TBL *pTable;

  if ((pTable = (struct CARD_TBL *)MallocMW(sizeof(struct CARD_TBL))) == NULL)
    return FALSE;
  memset(pTable, 0, sizeof(struct CARD_TBL));
  
  end_node = mxmlWalkNext(aCurrNode, aTree, MXML_NO_DESCEND);
  while (1) {
    curr_node = mxmlWalkNext(aCurrNode, aTree, MXML_DESCEND);
    if (curr_node == end_node)
      break;                    // break if end of current level
    aCurrNode = curr_node;
    if (aCurrNode != NULL) {
      if (aCurrNode->type == MXML_TEXT)
        continue;
      if (aCurrNode->type == MXML_ELEMENT) {
        //SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
        string = (BYTE *)mxmlGetText(aCurrNode, NULL);
        if (string != NULL) {
#if 0
          SprintfMW(buf, "Card->text[%d]: %s", strlen(string), string);
          DispLineMW(buf, MW_LINE5, MW_CLREOL|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
#endif
          if (CheckNodeName((struct XML_PARAM *)&CardTableParam[0], aCurrNode->value.element.name, &n_type, &n_size, &n_offset)) {
            // XML tag found in Card Table
            UpdateNodeParam((BYTE *)pTable+n_offset, n_type, n_size, string);
          }
        }
      }
    }
    else {
      //DispLineMW("End of Walk!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SMFONT);
      //WaitKey(KBD_TIMEOUT);
      break;
    }
  }
  UpdCardTbl(gCardIdx++, pTable);
  FreeMW(pTable);
  return aCurrNode;
}
//*****************************************************************************
//  Function        : ParseEmvApp
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
mxml_node_t *ParseEmvApp(mxml_node_t *aTree, mxml_node_t *aCurrNode, mxml_node_t *aEndNode)
{
  //BYTE buf[2048];
  BYTE *string;
  mxml_node_t *curr_node, *end_node;
  APP_CFG *app_cfg;

  if ((app_cfg = (APP_CFG *)MallocMW(sizeof(APP_CFG))) == NULL)
    return FALSE;
  memset(app_cfg, 0, sizeof(APP_CFG));
  
  SetEMVLoaded(0x80);
  end_node = mxmlWalkNext(aCurrNode, aTree, MXML_NO_DESCEND);
  while (1) {
    curr_node = mxmlWalkNext(aCurrNode, aTree, MXML_DESCEND);
    if (curr_node == end_node)
      break;                    // break if end of current level
    aCurrNode = curr_node;
    if (aCurrNode != NULL) {
      if (aCurrNode->type == MXML_TEXT)
        continue;
      if (aCurrNode->type == MXML_ELEMENT) {
        //SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
        string = (BYTE *)mxmlGetText(aCurrNode, NULL);
        if (string != NULL) {
#if 0
          SprintfMW(buf, "EmvApp->text[%d]: %s", strlen(string), string);
          DispLineMW(buf, MW_LINE5, MW_CLREOL|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
#endif
          UpdateParamEmvApp(app_cfg, (struct XML_PARAM *)&EmvAppParam[0], aCurrNode->value.element.name, string);
        }
      }
    }
    else {
      //DispLineMW("End of Walk!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SMFONT);
      //WaitKey(KBD_TIMEOUT);
      break;
    }
  }
  EMVData2DnloadBuf(IOCMD_LOAD_AID, (BYTE *)app_cfg, sizeof(APP_CFG));
  SetEMVLoaded(0x01); // lower nibble for status
  FreeMW(app_cfg);
  return aCurrNode;
}
//*****************************************************************************
//  Function        : ParseEmvApp
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
mxml_node_t *ParseCtlApp(mxml_node_t *aTree, mxml_node_t *aCurrNode, mxml_node_t *aEndNode)
{
  //BYTE buf[2048];
  BYTE *string;
  mxml_node_t *curr_node, *end_node;
  CL_CFGP *app_cfg;

  if ((app_cfg = (CL_CFGP *)MallocMW(sizeof(CL_CFGP))) == NULL)
    return FALSE;
  memset(app_cfg, 0, sizeof(CL_CFGP));
  
  SetCTLLoaded(0x80);
  end_node = mxmlWalkNext(aCurrNode, aTree, MXML_NO_DESCEND);
  while (1) {
    curr_node = mxmlWalkNext(aCurrNode, aTree, MXML_DESCEND);
    if (curr_node == end_node)
      break;                    // break if end of current level
    aCurrNode = curr_node;
    if (aCurrNode != NULL) {
      if (aCurrNode->type == MXML_TEXT)
        continue;
      if (aCurrNode->type == MXML_ELEMENT) {
        //SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
        string = (BYTE *)mxmlGetText(aCurrNode, NULL);
        if (string != NULL) {
#if 0
          SprintfMW(buf, "CtlApp->text[%d]: %s", strlen(string), string);
          DispLineMW(buf, MW_LINE5, MW_CLREOL|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
#endif
          UpdateParamCtlApp(app_cfg, (struct XML_PARAM *)&CtlAppParam[0], aCurrNode->value.element.name, string);
        }
      }
    }
    else {
      //DispLineMW("End of Walk!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SMFONT);
      //WaitKey(KBD_TIMEOUT);
      break;
    }
  }
  CTLData2DnloadBuf(CLCMD_LOAD_AID, (BYTE *)app_cfg, sizeof(CL_CFGP));
  SetCTLLoaded(0x01); // lower nibble for status
  FreeMW(app_cfg);
  return aCurrNode;
}
//*****************************************************************************
//  Function        : ParseEmvKey
//  Description     : Parse node data at current level.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
mxml_node_t *ParseEmvKey(mxml_node_t *aTree, mxml_node_t *aCurrNode, mxml_node_t *aEndNode)
{
  //BYTE buf[2048];
  BYTE *string;
  mxml_node_t *curr_node, *end_node;
  KEY_ROOM_T *key_room;

  if ((key_room = (KEY_ROOM_T *)MallocMW(sizeof(KEY_ROOM_T))) == NULL)
    return FALSE;
  memset(key_room, 0, sizeof(KEY_ROOM_T));
  
  SetEMVLoaded(0x40);
  SetCTLLoaded(0x40);
  end_node = mxmlWalkNext(aCurrNode, aTree, MXML_NO_DESCEND);
  while (1) {
    curr_node = mxmlWalkNext(aCurrNode, aTree, MXML_DESCEND);
    if (curr_node == end_node)
      break;                    // break if end of current level
    aCurrNode = curr_node;
    if (aCurrNode != NULL) {
      if (aCurrNode->type == MXML_TEXT)
        continue;
      if (aCurrNode->type == MXML_ELEMENT) {
        //SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
        string = (BYTE *)mxmlGetText(aCurrNode, NULL);
        if (string != NULL) {
#if 0
          SprintfMW(buf, "n->name: %s", aCurrNode->value.element.name);
          DispLineMW(buf, MW_LINE4, MW_CLREOL|MW_SPFONT);
          SprintfMW(buf, "EmvKey->text[%d]: %s", strlen(string), string);
          DispLineMW(buf, MW_LINE5, MW_CLREOL|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
#endif
          /*
          if (CheckNodeName((struct XML_PARAM *)&EmvKeyParam[0], aCurrNode->value.element.name, &n_type, &n_size, &n_offset)) {
            // XML tag found in EMVL2 Key Table
            UpdateNodeParam((BYTE *)pTable+n_offset, n_type, n_size, string);
          }
          */
          UpdateParamEmvKey(key_room, (struct XML_PARAM *)&EmvKeyParam[0], aCurrNode->value.element.name, string);
        }
      }
    }
    else {
      //DispLineMW("End of Walk!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SMFONT);
      //WaitKey(KBD_TIMEOUT);
      break;
    }
  }
#if 0
  SprintfMW(buf, "EmvKey[%02x]:\n", key_room->s_key_room.KeyIdx);
  DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
  SprintfMW(buf, "  RID: %02X%02X%02X%02X%02X\n", key_room->s_key_room.RID[0],key_room->s_key_room.RID[1],key_room->s_key_room.RID[2],
                                                  key_room->s_key_room.RID[3],key_room->s_key_room.RID[4]);
  DispLineMW(buf, MW_LINE2, MW_SPFONT);
  SprintfMW(buf, "  Exp: 0x%X\n", key_room->s_key_room.CAKey.Exponent);
  DispLineMW(buf, MW_LINE3, MW_SPFONT);
  SprintfMW(buf, "  Size:%d\n", key_room->s_key_room.CAKey.KeySize);
  DispLineMW(buf, MW_LINE4, MW_SPFONT);
  SprintfMW(buf, "  Key: %02x %02x %02x %02x\n", key_room->s_key_room.CAKey.Key[0],key_room->s_key_room.CAKey.Key[1],
                                                 key_room->s_key_room.CAKey.Key[2],key_room->s_key_room.CAKey.Key[3]);
  DispLineMW(buf, MW_LINE5, MW_SPFONT);
  SprintfMW(buf, "  Type:%d\n", key_room->b_key_type);
  DispLineMW(buf, MW_LINE6, MW_SPFONT);
  WaitKey(KBD_TIMEOUT);
#endif
  if (key_room->s_key_room.KeyIdx && key_room->s_key_room.CAKey.KeySize) {
    if (key_room->b_key_type == 0x01) {           // EMV Contact Key only
      EMVData2DnloadBuf(IOCMD_KEY_LOADING, (BYTE *)&key_room->s_key_room, sizeof(KEY_ROOM));
    }
    else if (key_room->b_key_type == 0x02) {      // EMV Contactless Key only
      CTLData2DnloadBuf(CLCMD_KEY_LOADING, (BYTE *)&key_room->s_key_room, sizeof(KEY_ROOM));
    }
    else if (key_room->b_key_type == 0x03) {      // Both Contact & Contactless to be injected
      EMVData2DnloadBuf(IOCMD_KEY_LOADING, (BYTE *)&key_room->s_key_room, sizeof(KEY_ROOM));
      CTLData2DnloadBuf(CLCMD_KEY_LOADING, (BYTE *)&key_room->s_key_room, sizeof(KEY_ROOM));
    }
  }
  FreeMW(key_room);
  return aCurrNode;
}

//*****************************************************************************
//  Function        : SetAPNConfig
//  Description     : Load XML string and save params.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetAPNConfig()
{
  char user[32 + 1];
  char psw[32 + 1];
  char apn[32 + 1];
  char dispBuf[128];
  
  /*
  BYTE apn_username[32];
  BYTE apn_password[32];
  BYTE apn2_username[32];
  BYTE apn2_password[32];
  BYTE apnconfig[32];
  BYTE apnconfig2[32];
  */
  
  struct TERM_CFG term_cfg;

  if (!GetTermCfg(&term_cfg))
  {
    DispLineMW("failed to get term cfg", MW_LINE3, MW_CLREOL | MW_BIGFONT | MW_CENTER);
    while (GetCharMW() == 0)
      SleepMW();
    return;
  }
  
  ClearDispMW();
  memset(apn, 0, sizeof(apn));
  memcpy(apn, term_cfg.apnconfig, 32);
  sprintf(dispBuf, "apn1: %s", apn);
  DispLineMW(dispBuf, MW_LINE1, MW_CLREOL | MW_SPFONT);
  memset(user, 0, sizeof(user));
  memcpy(user, term_cfg.apn_username, 32);
  sprintf(dispBuf, "apn1 user: %s", user);
  DispLineMW(dispBuf, MW_LINE2, MW_CLREOL | MW_SPFONT);
  memset(psw, 0, sizeof(psw));
  memcpy(psw, term_cfg.apn_password, 32);
  sprintf(dispBuf, "apn1 psw: %s", psw);
  DispLineMW(dispBuf, MW_LINE3, MW_CLREOL | MW_SPFONT);
  
  memset(apn, 0, sizeof(apn));
  memcpy(apn, term_cfg.apnconfig2, 32);
  sprintf(dispBuf, "apn2: %s", apn);
  DispLineMW(dispBuf, MW_LINE4, MW_CLREOL | MW_SPFONT);
  memset(user, 0, sizeof(user));
  memcpy(user, term_cfg.apn2_username, 32);
  sprintf(dispBuf, "apn2 user: %s", user);
  DispLineMW(dispBuf, MW_LINE5, MW_CLREOL | MW_SPFONT);
  memset(psw, 0, sizeof(psw));
  memcpy(psw, term_cfg.apn2_password, 32);
  sprintf(dispBuf, "apn2 psw: %s", psw);
  DispLineMW(dispBuf, MW_LINE6, MW_CLREOL | MW_SPFONT);
  
  char choice = ToggleOption("select APN", KSimSel, TOGGLE_MODE_8LINE);
  
  ClearDispMW();
  if (choice == (char)-1)
  {
    sprintf(dispBuf, "user cancelled");
    DispLineMW(dispBuf, MW_LINE3, MW_CLREOL | MW_BIGFONT | MW_CENTER);
  }
  else
  {
    sprintf(dispBuf, "APN #%d is chosen", choice + 1);
    DispLineMW(dispBuf, MW_LINE3, MW_CLREOL | MW_BIGFONT | MW_CENTER);
    memset(apn, 0, sizeof(apn));
    memset(user, 0, sizeof(user));
    memset(psw, 0, sizeof(psw));
    if (choice == 0)
    {
      memcpy(apn, term_cfg.apnconfig, 32);
      memcpy(user, term_cfg.apn_username, 32);
      memcpy(psw, term_cfg.apn_password, 32);
    }
    else
    {
      memcpy(apn, term_cfg.apnconfig2, 32);
      memcpy(user, term_cfg.apn2_username, 32);
      memcpy(psw, term_cfg.apn2_password, 32);
    }
    SetGPRSInfo(user, psw, apn, 0);
  }

  while (GetCharMW() == 0)
    SleepMW();
}
//*****************************************************************************
//  Function        : XmlStringToCfg
//  Description     : Load XML string and save params.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN XmlStringToCfg(BYTE *aPtr)
{
  mxml_node_t *tree=NULL, *node=NULL, *next_node=NULL;
  //BYTE buf[256];
  //BYTE *string;
  BYTE start=1;
  BYTE state=ST_TERM_CLEAR;
  BOOLEAN ret=FALSE;

  gCardIdx = 0;
  
  // XML parsing
  tree = mxmlLoadString(NULL, aPtr, MXML_TEXT_CALLBACK);
  if (tree != NULL) {
    while (1) {
      if (start) {
        node = tree;
        node = mxmlFindElement(node, tree, "TerminalXML", NULL, NULL, MXML_DESCEND);
        start = 0;
      }
      if (node != NULL) {
        switch (state) {
          case ST_TERM_CLEAR:
            CloseSTISFiles();
            CreateSTISFiles();
            SetSTISMode(INIT_MODE);
            fCommitAllMW();
            state++;              // fall through
          case ST_TERM_CFG:
            // Search Term Cfg Table
            node = mxmlFindElement(node, tree, "Terminal", NULL, NULL, MXML_DESCEND);
            if (node != NULL) {
              if (node->type == MXML_ELEMENT) {
                //SprintfMW(buf, "T->name: %s", node->value.element.name);
                next_node = node->next;
              }
              //DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
              //SprintfMW(buf, "T->type: %d", node->type);
              //DispLineMW(buf, MW_LINE2, MW_SPFONT);
#if 0
              string = (BYTE *)mxmlGetText(node, NULL);
              if (string != NULL) {
                SprintfMW(buf, "T->text[%d]: %s", strlen(string), string);
              }
              else {
                SprintfMW(buf, "T->text: XXX");
              }
              DispLineMW(buf, MW_LINE3, MW_SPFONT);
              WaitKey(KBD_TIMEOUT);
#endif
              // parse sub-level of "option/choice" only
              node = ParseTermCfg(tree, node, next_node);
            }
            else {
              //node = tree = stree;        // search from start for next state
              start = 1;
              state++;
            }
            break;
          case ST_TERM_ACQ:
            // Search Acquirer Table
            node = mxmlFindElement(node, tree, "Acquirer", NULL, NULL, MXML_DESCEND);
            if (node != NULL) {
              if (node->type == MXML_ELEMENT) {
                //SprintfMW(buf, "T->name: %s", node->value.element.name);
                next_node = node->next;
              }
              //DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
              //SprintfMW(buf, "T->type: %d", node->type);
              //DispLineMW(buf, MW_LINE2, MW_SPFONT);
#if 0
              string = (BYTE *)mxmlGetText(node, NULL);
              if (string != NULL) {
                SprintfMW(buf, "A->text[%d]: %s", strlen(string), string);
              }
              else {
                SprintfMW(buf, "A->text: XXX");
              }
              DispLineMW(buf, MW_LINE3, MW_SPFONT);
              WaitKey(KBD_TIMEOUT);
#endif
              // parse sub-level of "option/choice" only
              node = ParseAcquirer(tree, node, next_node);
            }
            else {
              //node = tree = stree;        // search from start for next state
              start = 1;
              state++;
            }
            break;
          case ST_TERM_ISSUER:
            // Search Issuer Table
            node = mxmlFindElement(node, tree, "Issuer", NULL, NULL, MXML_DESCEND);
            if (node != NULL) {
              if (node->type == MXML_ELEMENT) {
                //SprintfMW(buf, "I->name: %s", node->value.element.name);
                //DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
                next_node = node->next;
              }
#if 0
              string = (BYTE *)mxmlGetText(node, NULL);
              if (string != NULL) {
                //SprintfMW(buf, "I->text[%d]: %s", strlen(string), string);
                SprintfMW(buf, "I->text[%d]: %s|%02x%02x", strlen(string), string, string[0],string[1]);
              }
              else {
                SprintfMW(buf, "I->text: XXX");
              }
              DispLineMW(buf, MW_LINE2, MW_SPFONT);
              WaitKey(KBD_TIMEOUT);
#endif
              // parse sub-level of "option/choice" only
              node = ParseIssuer(tree, node, next_node);
            }
            else {
              //DispLineMW("No \"IssuerTable\" Found", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
              //WaitKey(KBD_TIMEOUT);
              //node = tree = stree;        // search from start for next state
              start = 1;
              state++;
            }
            break;
          case ST_TERM_CARD:
            // Search Card Table
            node = mxmlFindElement(node, tree, "CardRange", NULL, NULL, MXML_DESCEND);
            if (node != NULL) {
              if (node->type == MXML_ELEMENT) {
                //SprintfMW(buf, "T->name: %s", node->value.element.name);
                next_node = node->next;
              }
              //DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
              //SprintfMW(buf, "T->type: %d", node->type);
              //DispLineMW(buf, MW_LINE2, MW_SPFONT);
#if 0
              string = (BYTE *)mxmlGetText(node, NULL);
              if (string != NULL) {
                SprintfMW(buf, "C->text[%d]: %s", strlen(string), string);
              }
              else {
                SprintfMW(buf, "C->text: XXX");
              }
              DispLineMW(buf, MW_LINE2, MW_SPFONT);
              WaitKey(KBD_TIMEOUT);
#endif
              // parse sub-level of "option/choice" only
              node = ParseCard(tree, node, next_node);
            }
            else {
              //node = tree = stree;        // search from start for next state
              start = 1;
              state++;
            }
            break;
          case ST_EMV_APP:
            // Search EMVL2 App
            node = mxmlFindElement(node, tree, "EMVLevel2App", NULL, NULL, MXML_DESCEND);
            if (node != NULL) {
              if (node->type == MXML_ELEMENT) {
                //SprintfMW(buf, "T->name: %s", node->value.element.name);
                next_node = node->next;
              }
              //DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
              //SprintfMW(buf, "T->type: %d", node->type);
              //DispLineMW(buf, MW_LINE2, MW_SPFONT);
#if 0
              string = (BYTE *)mxmlGetText(node, NULL);
              if (string != NULL) {
                SprintfMW(buf, "C->text[%d]: %s", strlen(string), string);
              }
              else {
                SprintfMW(buf, "C->text: XXX");
              }
              DispLineMW(buf, MW_LINE2, MW_SPFONT);
              WaitKey(KBD_TIMEOUT);
#endif
              // parse sub-level of "option/choice" only
              node = ParseEmvApp(tree, node, next_node);
            }
            else {
              //node = tree = stree;        // search from start for next state
              start = 1;
              state++;
            }
            break;
          case ST_CTL_APP:
            // Search EMVCTL App
            node = mxmlFindElement(node, tree, "EMVCtlApp", NULL, NULL, MXML_DESCEND);
            if (node != NULL) {
              if (node->type == MXML_ELEMENT) {
                //SprintfMW(buf, "T->name: %s", node->value.element.name);
                next_node = node->next;
              }
              //DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
              //SprintfMW(buf, "T->type: %d", node->type);
              //DispLineMW(buf, MW_LINE2, MW_SPFONT);
#if 0
              string = (BYTE *)mxmlGetText(node, NULL);
              if (string != NULL) {
                SprintfMW(buf, "C->text[%d]: %s", strlen(string), string);
              }
              else {
                SprintfMW(buf, "C->text: XXX");
              }
              DispLineMW(buf, MW_LINE2, MW_SPFONT);
              WaitKey(KBD_TIMEOUT);
#endif
              // parse sub-level of "option/choice" only
              node = ParseCtlApp(tree, node, next_node);
            }
            else {
              //node = tree = stree;        // search from start for next state
              start = 1;
              state++;
            }
            break;
          case ST_EMV_KEY:
            // Search EMVL2 Key
            node = mxmlFindElement(node, tree, "EMVLevel2Key", NULL, NULL, MXML_DESCEND);
            if (node != NULL) {
              if (node->type == MXML_ELEMENT) {
                //SprintfMW(buf, "T->name: %s", node->value.element.name);
                next_node = node->next;
              }
              //DispLineMW(buf, MW_LINE1, MW_CLRDISP|MW_SPFONT);
              //SprintfMW(buf, "T->type: %d", node->type);
              //DispLineMW(buf, MW_LINE2, MW_SPFONT);
#if 0
              string = (BYTE *)mxmlGetText(node, NULL);
              if (string != NULL) {
                SprintfMW(buf, "C->text[%d]: %s", strlen(string), string);
              }
              else {
                SprintfMW(buf, "C->text: XXX");
              }
              DispLineMW(buf, MW_LINE2, MW_SPFONT);
              WaitKey(KBD_TIMEOUT);
#endif
              // parse sub-level of "option/choice" only
              node = ParseEmvKey(tree, node, next_node);
            }
            else {
              //node = tree = stree;        // search from start for next state
              start = 1;
              state++;
            }
            break;
          case ST_END:
          default:
            node = NULL;
            break;
        }
      }
      else {
#if 0
        DispLineMW("No \"TermTable\" Found", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
        WaitKey(KBD_TIMEOUT);
#endif
        break;
      }
    }
  }
  else {
    DispLineMW("Load String Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
  }
  mxmlDelete(node);
  mxmlDelete(tree);
  if (GetAcqCount() && GetIssuerCount() && GetCardCount()) {
    SetAPNConfig();
    SetSTISMode(TRANS_MODE);
    DispClrBelowMW(MW_LINE3);
    DispLineMW("PARAM UPDATED", MW_LINE5, MW_CENTER|MW_SMFONT);
    WaitKey(300);
    ret = TRUE;
  }
    
  return ret;
}
//*****************************************************************************
//  Function        : STISLoadParam
//  Description     : Load XML file and save params.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN STISLoadParam(void)
{
  T_FILE file_info, *pInfo;
  int    fd, fsize, rlen;
  BYTE   fname[32], *ptr;
  DWORD  ctrl;

  strcpy(fname, "PARAM8");
  ctrl = K_SearchPub|K_SearchReset;
  while (1) {
    pInfo = os_file_search_data(ctrl, &file_info);
    if (pInfo == NULL)
      break;
    if (memcmp(fname, pInfo->s_name, 6) == 0)
      break;
    ctrl = K_SearchPub|K_SearchNext;
  }
  if (pInfo == NULL) {
    DispLineMW("PARAM8 not found!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
    return FALSE;
  }
  
  strcpy(fname, pInfo->s_owner);
  strcat(fname, pInfo->s_name);
  fd = os_file_open(fname, K_O_RDONLY);
  fsize = os_file_length(fd);
  if ((fd != -1) && (fsize > 0)) {
    ptr = (BYTE*) MallocMW(fsize);
    if (ptr == NULL) {
      os_file_close(fd);
      return FALSE;
    }
    rlen = os_file_read(fd, ptr, fsize);
    os_file_close(fd);
  }
  else
    return FALSE;

  if (rlen != fsize) {
    FreeMW(ptr);
    return FALSE;
  }
  
  EMVClrDnloadBuf();
  CTLClrDnloadBuf();
  if (XmlStringToCfg(ptr) == TRUE) {
    EMVDataUpdate();
    EMVInit();
    EMVKillDnloadBuf();
    CTLDataUpdate();
    CTLKillDnloadBuf();
    FreeMW(ptr);
    return TRUE;
  }
  EMVKillDnloadBuf();
  CTLKillDnloadBuf();
  FreeMW(ptr);
  return FALSE;
}
