//-----------------------------------------------------------------------------
//  File          : key2dll.h
//  Module        : 
//  Description   : Declrartion & Defination for KEYDLL export functions.
//  Author        : Lewis
//  Notes         :
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  16 Sept 2008  Lewis       Port From CREON.
//-----------------------------------------------------------------------------
#ifndef _INC_KEYDLL_H_
#define _INC_KEYDLL_H_
#include "common.h"

//-----------------------------------------------------------------------------
//   Common Defines
//-----------------------------------------------------------------------------
#define KEYDLL_ID          0x7F

// Command ID
enum {
KDLL_KEYRESET        ,
KDLL_KEYSTATUS       ,
KDLL_KEYINJECT       ,
KDLL_KEYENCRYPT      ,
KDLL_KEYDECRYPT      ,
KDLL_KEY3ENCRYPT     ,
KDLL_KEY3DECRYPT     ,
KDLL_KEYGETX98PIN    ,
KDLL_KEYX99MAC       ,
KDLL_SETOPMODE       ,
KDLL_GETOPMODE       ,
KDLL_DUKPTRESET      ,
KDLL_DUKPTINJECT     ,
KDLL_DUKPTGETPIN     ,
KDLL_EXTPPADSET      ,
KDLL_ICCMKEYINJECT   ,
KDLL_DUKPTCALMAC     ,
KDLL_DOGENRSAKEY     ,
KDLL_GETRSAPUBKEY    ,
KDLL_HSMRSAENCRYPT   ,
KDLL_HSMRSADECRYPT   ,
KDLL_HSMRSASSLINJECT ,
KDLL_KEYSAVE         ,
KDLL_DSSGETKVC       ,
KDLL_DSSREGENKEY     ,
KDLL_DSS             ,
KDLL_AESKEYINJECT    ,
KDLL_AESENCRYPT      ,
KDLL_AESDECRYPT      ,
KDLL_HSMRSAKEYINJECT ,
KDLL_SREDENCRYPTALL  ,
KDLL_FUNC_COUNT
};


//-----------------------------------------------------------------------------
//   KEY related
//-----------------------------------------------------------------------------
//Key status, mode
#define KEY_STATUS_LOADED       0x01
#define KEY_STATUS_DECRYPT      0x02
#define KEY_STATUS_MASTER       0x04
#define KEY_STATUS_SINGLE       0x08
#define KEY_STATUS_DOUBLE       0x10
#define KEY_STATUS_TRIPLE       0x18
#define KEY_STATUS_MAC          0x20
#define KEY_STATUS_SRED_KEY     0x80    // for Master/Session Only
#define KEY_STATUS_DUKPT_BDK    0x80    // for DUKPT Only

//KVC mode
#define KVC_EXIST               0x01

//Key usage
#define USE_LEFT                0x40
#define USE_RIGHT               0x80
#define USE_THIRD               0xC0

//Encryption/Decryption mode
#define MODE_ECB                0
#define MODE_CBC                1

//Key Cmd Response code
#define RESP_OK                 0
#define RESP_KEY_CLR            1
#define RESP_KBD_FAILED         1
#define RESP_INVALID_TID        1
#define RESP_OUT_OF_SEQ         1
#define RESP_TMK_NOT_LOAD       1
#define RESP_KVC_ERR            1
#define RESP_WRONG_KEY_FUNC     1
#define RESP_FONT_TBL_FULL      1
#define RESP_LOCATION_ERROR     1
#define RESP_SINGLE_KEY_ONLY    2
#define RESP_EPROM_KEY_FAILED   2
#define RESP_PARITY_ERROR       2
#define RESP_LCD_ERROR          2
#define RESP_KEY_CANCEL         2
#define RESP_LENGTH_ERROR       2
#define RESP_KEY_EXIST          2
#define RESP_TIMEOUT            3
#define RESP_KEY_INDEX_ERROR    4
#define RESP_MODE_ERROR         5
#define RESP_AUTH_ERROR         6
#define RESP_DATA_LEN_ERR       7
#define RESP_KEY_OPER_ERR       8
#define RESP_SRED_INJ_ERR       9

// DUKPT Function response code
#define CMD_OK            0
#define KVC_ERR           1
#define CMD_CANCEL        2
#define NOT_INJECTED      3
#define INVALID_HOST      4
#define DUKPT_HALT        5

// ICC inject response code 
#define ICC_NOT_EXIST       10
#define ICC_OPEN_FAIL       11
#define ICC_READ_FILE_ERR   12
#define ICC_WRONG_PWD       13
#define ICC_FAIL            14
#define ICC_KEY_CANCEL      15

//Add. Param for GetPIN functions
#define PRM_4DGT_PIN            0x01
#define PRM_PIN_BYPASS          0x02

//Mode for DoGenRsaKey function
#define MODE_SSL_USE            0x01

//SRED definition
#define TAG_SRED                (WORD)0xFF60  //act as construction tag for SRed encrypted data
#define TAG_SRED_HOSTID         (WORD)0xFF61  //primitive tag for sred host id

//extern void KDLL_KeyReset(BYTE aKeyidx);  0xFF=>All 
#define KDLL_KeyReset(x)  lib_app_call(KEYDLL_ID, KDLL_KEYRESET, x, 0, 0)

//extern void KDLL_KeyStatus(BYTE *aStatusStr);
#define KDLL_KeyStatus(x)  lib_app_call(KEYDLL_ID, KDLL_KEYSTATUS, (DWORD)x, 0, 0)

//extern WORD KDLL_KeyInject(BYTE *aKeyStr);
#define KDLL_KeyInject(x)  (WORD) lib_app_call(KEYDLL_ID, KDLL_KEYINJECT, (DWORD)x, 0, 0)

//extern WORD KDLL_KeyEncrypt(BYTE aKeyidx, BYTE *aDataBuf, WORD aDataLen);
#define KDLL_KeyEncrypt(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_KEYENCRYPT, (DWORD)x, (DWORD)y, (WORD)z)

//extern WORD KDLL_KeyDecrypt(BYTE aKeyidx, BYTE *aDataBuf, WORD aDataLen);
#define KDLL_KeyDecrypt(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_KEYDECRYPT, (DWORD)x, (DWORD)y, (WORD)z)

//extern WORD KDLL_Key3Encrypt(BYTE aKeyidx, BYTE *aDataBuf, WORD aDataLen);
#define KDLL_Key3Encrypt(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_KEY3ENCRYPT, (DWORD)x, (DWORD)y, (WORD)z)

//extern WORD KDLL_Key3Decrypt(BYTE aKeyidx, BYTE *aDataBuf, WORD aDataLen);
#define KDLL_Key3Decrypt(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_KEY3DECRYPT, (DWORD)x, (DWORD)y, (WORD)z)

//extern WORD KDLL_GetX98PIN(BYTE *aPINBlock, BYTE *aPromptMsg, BYTE aKeyidx);
#define KDLL_GetX98PIN(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_KEYGETX98PIN, (DWORD)x, (DWORD)y, (DWORD)z)

// extern WORD KDLL_X99CalMAC(BYTE *aMacPtr, BYTE *aDataPtr, DWORD aLen);
#define KDLL_X99CalMAC(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_KEYX99MAC, (DWORD)x, (DWORD)y, (DWORD)z)

//extern void SetOpMode(BYTE aMode, BYTE *aInitData);
#define KDLL_SetOpMode(x, y)     lib_app_call(KEYDLL_ID, KDLL_SETOPMODE, (DWORD)x, (DWORD)y, (DWORD)0)

//extern void GetOpMode(BYTE *aMode);
#define KDLL_GetOpMode(x)        lib_app_call(KEYDLL_ID, KDLL_GETOPMODE, (DWORD)x, (DWORD)0, (DWORD)0)

//extern BOOLEAN DukptKeyReset(BYTE aKeyIdx);
#define KDLL_DukptKeyReset(x)      (BOOLEAN) lib_app_call(KEYDLL_ID, KDLL_DUKPTRESET, (DWORD)x, (DWORD)0, (DWORD)0)

//extern WORD DukptKeyInject(BYTE *aKeyStr);
#define KDLL_DukptKeyInject(x)     (WORD) lib_app_call(KEYDLL_ID, KDLL_DUKPTINJECT, (DWORD)x, (DWORD)0, (DWORD)0)

//extern WORD DukptGetPin(BYTE *aPINBlock, BYTE *aPromptMsg, BYTE aKeyIdx);
#define KDLL_DukptGetPin(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_DUKPTGETPIN, (DWORD)x, (DWORD)y, (DWORD)z)

//extern void ExtPPadSetup(DWORD aExtAid, DWORD aExtFuncId);
#define KDLL_ExtPPadSetup(x, y)    lib_app_call(KEYDLL_ID, KDLL_EXTPPADSET, (DWORD)x, (DWORD)y, (DWORD)0)

//extern WORD IccMKeyInject(BYTE aKeyidx, BYTE aType) 
#define KDLL_IccMKeyInject(x, y)   (WORD) lib_app_call(KEYDLL_ID, KDLL_ICCMKEYINJECT, (DWORD)x, (DWORD)y, (DWORD)0)

// extern WORD DukptCalMAC(BYTE *aMacPtr, BYTE *aDataPtr, DWORD aLen);
#define KDLL_DukptCalMAC(x, y, z)  (WORD) lib_app_call(KEYDLL_ID, KDLL_DUKPTCALMAC, (DWORD)x, (DWORD)y, (DWORD)z)

//extern int DoGenRsaKey(BYTE aPubKeyIdx, BYTE aPrvKeyIdx, DWORD aMode) 
#define KDLL_DoGenRsaKey(x, y, z)  (int) lib_app_call(KEYDLL_ID, KDLL_DOGENRSAKEY, (DWORD)x, (DWORD)y, (DWORD)z)

//extern int GetRsaPubKey(T_KEY *aPubKey) 
#define KDLL_GetRsaPubKey(x)       (int) lib_app_call(KEYDLL_ID, KDLL_GETRSAPUBKEY, (DWORD)x, (DWORD)0, (DWORD)0)

//extern int HsmRsaEncrypt(BYTE *aMsg, DWORD aSize)
#define KDLL_HsmRsaEncrypt(x, y)   (int) lib_app_call(KEYDLL_ID, KDLL_HSMRSAENCRYPT, (DWORD)x, (DWORD)y, (DWORD)0)

//extern int HsmRsaDecrypt(BYTE *aMsg, DWORD aSize);
#define KDLL_HsmRsaDecrypt(x, y)   (int) lib_app_call(KEYDLL_ID, KDLL_HSMRSADECRYPT, (DWORD)x, (DWORD)y, (DWORD)0)

//extern int HsmRsaSslInject(void);
#define KDLL_HsmRsaSslInject()     (int) lib_app_call(KEYDLL_ID, KDLL_HSMRSASSLINJECT, (DWORD)0, (DWORD)0, (DWORD)0)

//extern WORD KeySave(T_KEY_BLOCK *aKeyBlk)
#define KDLL_KeySave(x)            (WORD) lib_app_call(KEYDLL_ID, KDLL_KEYSAVE, (DWORD)x, (DWORD)0, (DWORD)0)

//extern BOOLEAN DssGetKvc(BYTE *aKvc);
#define KDLL_DssGetKvc(x)          (BOOLEAN) lib_app_call(KEYDLL_ID, KDLL_DSSGETKVC, (DWORD)x, (DWORD)0, (DWORD)0)

//extern BOOLEAN DssRegenKey(BYTE *aNewKvc);
#define KDLL_DssRegenKey(x)        (BOOLEAN) lib_app_call(KEYDLL_ID, KDLL_DSSREGENKEY, (DWORD)x, (DWORD)0, (DWORD)0)

//extern BOOLEAN Dss(BYTE *aIV, BYTE *aData, DWORD aModeLen);
#define KDLL_Dss(x, y, z)          (BOOLEAN) lib_app_call(KEYDLL_ID, KDLL_DSS, (DWORD)x, (DWORD)y, (DWORD)z)

//extern WORD AESKeyInject(BYTE aMode, WORD aKeyLen, BYTE *aKeyStr);
#define KDLL_AESKeyInject(x, y, z) (WORD) lib_app_call(KEYDLL_ID, KDLL_AESKEYINJECT, (DWORD)x, (DWORD)y, (DWORD)z)

//extern void AESEncrypt(BYTE *aDataBuf, WORD aDataLen);
#define KDLL_AESEncrypt(x, y)      lib_app_call(KEYDLL_ID, KDLL_AESENCRYPT, (DWORD)x, (DWORD)y, (DWORD)0)

//extern void AESDecrypt(BYTE *aDataBuf, WORD aDataLen);
#define KDLL_AESDecrypt(x, y)      lib_app_call(KEYDLL_ID, KDLL_AESDECRYPT, (DWORD)x, (DWORD)y, (DWORD)0)

//extern BOOLEAN HsmRsaKeyInject(T_RSA_PUBLIC_KEY *aPubKey, T_RSA_PRIVATE_KEY *aPrivateKey);
#define KDLL_HsmRsaKeyInject(x, y) (BOOLEAN) lib_app_call(KEYDLL_ID, KDLL_HSMRSAKEYINJECT, (DWORD)x, (DWORD)y, (DWORD)0)

//extern WORD SRedEncAll(DWORD aHostIdx, BYTE *aIoTlvList, int *aIoLen);
#define KDLL_SRedEncAll(x, y, z)   (WORD) lib_app_call(KEYDLL_ID, KDLL_SREDENCRYPTALL, (DWORD)x, (DWORD)y, (DWORD)z)

#endif // _INC_KEYDLL_H_
