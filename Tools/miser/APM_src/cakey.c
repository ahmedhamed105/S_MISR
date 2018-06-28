//-----------------------------------------------------------------------------
//  File          : cakey.c
//  Module        :
//  Description   : Inject CA key routine.
//  Author        : Lewis
//  Notes         : N/A
//
//  Naming conventions
//  ~~~~~~~~~~~~~~~~~~
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  15 May  2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "basecall.h"
#include "midware.h"
#include "apm.h"
#include "hwdef.h"
#include "stis.h"
#include "cakey.h"
#include "GetCert.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "key2dll.h"
#include "ftptran.h"
#include "termdata.h"
#include "toggle.h"

//-----------------------------------------------------------------------------
// Constant
//-----------------------------------------------------------------------------
static const struct MW_KEY KVsftpdKey[1] = {
  /* 4 vsftpd.cer  */
  {
    .d_exponent = 0x00010001,      /* Exponent   */
    .d_keysize = 256,              /* Key Length */
    .s_key = {                     /* Key        */
      0x68C7D7B3, 0x6E5DF734, 0x79A7595F, 0xBD0012CD, 
      0x6E4CD71B, 0x9182CD4A, 0x0AF122E7, 0xE17238DF, 
      0x2BCDACC7, 0x40A378ED, 0xB18DD170, 0x633F05AF, 
      0x97BF8A4B, 0x19244423, 0x3846E159, 0x942F4297, 
      0x3BF4F098, 0xC6865CB7, 0xA77093CD, 0x9E582D5F, 
      0xE9E6A250, 0x3DB4483B, 0x059C326F, 0x0F938920, 
      0xA9E39214, 0x73A713FA, 0x62043484, 0x808A1D0C, 
      0xEB6832C4, 0x3702D65D, 0xF7C2C1F8, 0x53CED530, 
      0xE31BE901, 0x4D955A98, 0xAFDDC47F, 0xDAE7C93C, 
      0x9AE15363, 0xB380154E, 0x5AC25E88, 0x5AFF77B1, 
      0xE33CD558, 0x839CF674, 0xCF154435, 0x089B929A, 
      0x1970F013, 0xE54D8497, 0xB0EB8974, 0x6E2FF2FC, 
      0x2F459869, 0x2C0E2870, 0xBEF278AB, 0xD2603C24, 
      0x5D8D7BF2, 0xA09AB0E9, 0xC560EA63, 0xF7498EEC, 
      0x4F111F7A, 0x4F6CEE58, 0x1F48C420, 0xD2DAAA2A, 
      0x94D76200, 0x82EBFCC3, 0x4AC15234, 0x0524A43E
    },
    .d_key_idx = SSL_CA_IDX_TMS                 /* Key Index  */
  }

};


//*****************************************************************************
//  Function        : InjectCAKey
//  Description     : Inject Spectra key to base.
//  Input           : N/A
//  Return          : TRUE/FALSE
//  Note            : //08-11-16 JC ++
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN InjectCAKey(void)
{
#if (MCP_SUPPORT)
  struct TERM_DATA term_data;
#endif
  struct ACQUIRER_TBL acq_tbl;
  struct MW_KEY *cakey;
  T_CLIENT_CERT *ClnCert;
  T_RSA_PRIVATE_KEY *ClnPriKey;
  T_KEY_BLOCK key_block;
  BOOLEAN ret=TRUE;
  DWORD i;
  BYTE ecr_ssl=0;
  
  cakey = (struct MW_KEY *) MallocMW(sizeof(struct MW_KEY));
  ClnCert = (T_CLIENT_CERT *) MallocMW(sizeof(T_CLIENT_CERT));
  ClnPriKey = (T_RSA_PRIVATE_KEY *) MallocMW(sizeof(T_RSA_PRIVATE_KEY));
  
#if (MCP_SUPPORT)
  // ECR SSL mode
  if (os_hd_config() & K_HdBt) {
    if (GetTermData(&term_data)) {
      if ((term_data.b_ecr_port&0x3F) == TOG_BT)
        ecr_ssl = term_data.b_ecr_ssl;      // only enable SSL over MCP channel 1
    }
  }
#endif
    
  /****** LOAD CA KEY ******/
  // CA key for TMS
  memset(cakey, 0, sizeof(struct MW_KEY));
  if(!GetKey(cakey, CA_TMS)) {
    memcpy(cakey, KVsftpdKey, sizeof(struct MW_KEY));
    if (InjectCAKeyMW(cakey) == 0) {
      DispLineMW("Key Injection Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      ret = FALSE;
    }
  }
  else {
    cakey->d_key_idx = SSL_CA_IDX_TMS;
    if (InjectCAKeyMW(cakey) == 0) {
      DispLineMW("Key Param Inj Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      ret = FALSE;
    }
  }
  // CA key for ECR
  memset(cakey, 0, sizeof(struct MW_KEY));
  if(GetKey(cakey, CA_ECR)) {
    cakey->d_key_idx = SSL_CA_IDX_ECR;
    if (InjectCAKeyMW(cakey) == 0) {
      DispLineMW("Key Param Inj Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      ret = FALSE;
    }
  }
  else if (ecr_ssl) {
    DispLineMW("PARAM0 not found!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
  }
  // CA key for Host
  memset(cakey, 0, sizeof(struct MW_KEY));
  if(GetKey(cakey, CA_HOST)) {
    cakey->d_key_idx = SSL_CA_IDX_HOST;
    if (InjectCAKeyMW(cakey) == 0) {
      DispLineMW("Key Param Inj Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      ret = FALSE;
    }
  }
  else {
    for (i=0; i < GetAcqCount(); i++) {
      if (GetAcqTbl(i, &acq_tbl)) {
        if ((acq_tbl.b_ssl_key_idx&0x7F) == 0x01) {
          // at least one of the acquirer needs CA Key Idx 01
          DispLineMW("PARAM1 not found!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          WaitKey(KBD_TIMEOUT);
          break;
        }
      }
    }
  }
    
  /****** LOAD CLIENT CERT ******/
  if(GetClientCert(ClnCert)) {
    ClnCert->d_cert_idx = SSL_CLN_CERT_IDX;           // set cert index
    if (bs_clientcert_inject(ClnCert) == 0) {
      DispLineMW("Cln Cert Injection Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      ret = FALSE;
    }
  }
  else if (ecr_ssl >= 2) {
    DispLineMW("PARAM2 not found!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
  }
  
  /****** LOAD CLIENT PRIVATE KEY ******/
  if(GetClientPriKey(ClnPriKey)) {
    ClnPriKey->d_key_idx = SSL_CLN_PRV_KEY_IDX;       // set key index
    key_block.w_kb_len    = K_KBRsaPrivLen;
    key_block.w_key_idx   = ClnPriKey->d_key_idx;
    key_block.w_key_version = 1;
    key_block.b_algorithm = K_KBAlgorithmRsaPrivate;
    key_block.b_usage     = K_KBSslSignature;
    memcpy(&(key_block.s_rsa_pri_kb.s_privkey), ClnPriKey, sizeof(T_RSA_PRIVATE_KEY));
    
    // convert private key from plaintext into encrypted form
    if (KDLL_KeySave(&key_block) != RESP_OK) {
      DispLineMW("PriKey Save Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      ret = FALSE;
    }
    else {
      if (bs_privatekey_inject(&key_block) == 0) {
        DispLineMW("PriKey Injection Fail!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
        WaitKey(KBD_TIMEOUT);
        ret = FALSE;
      }
    }
  }
  else if (ecr_ssl >= 2) {
    DispLineMW("PARAM3 not found!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
  }
  
  FreeMW(cakey);
  FreeMW(ClnCert);
  FreeMW(ClnPriKey);
  return ret;
}

