/*-----------------------------------------------------------------------------
 *  File          : keytrans.c
 *  Module        : T1000
 *  Description   : Include routines for sale/offline transaction.
 *  Author        : John
 *  Notes         :
 *
 *  Date          Who         Action
 *  ------------  ----------- -------------------------------------------------
 *  19 Mar  2012  John        Port from CREON/PR608.
 *-----------------------------------------------------------------------------
 */
#include <string.h>
#include <stdio.h>
#include "constant.h"
#include "corevar.h"
#include "sysutil.h"
#include "tranutil.h"
#include "message.h"
#include "keytrans.h"
#include "key2dll.h"
#include "midware.h"
#include "util.h"
#include "input.h"


static BYTE dhost_idx=0;
static BYTE keysize;
static BYTE keylen;

/******************************************************************************
 *  Function        : key_ready
 *  Description     : Check if key loaded in key bank
 *  Input           : idx;            // key index
 *  Return          : TRUE;           // key loaded
 *                    FALSE;          // key not loaded
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN key_ready(BYTE idx)
{
  BYTE keysts[40];

  KDLL_KeyStatus(keysts);
  if (keysts[idx] & KEY_STATUS_LOADED)
    return TRUE;
  else
    return FALSE;
}
/******************************************************************************
 *  Function        : ResetKey
 *  Description     : Reset terminal key
 *  Input           : idx;            // key index
 *  Return          : TRUE;           // key reset ok
 *                    FALSE;          // key reset failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void ResetKey(BYTE idx)
{
  DispLineMW("Resetting Keys..", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  KDLL_KeyReset(idx);
  Short2Beep();
}
/******************************************************************************
 *  Function        : LoadTmk
 *  Description     : Load terminal master key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN LoadTmk(void)
{
  const BYTE KTMK[] = "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10\x11\x22\x33\x44\x55\x66\x77\x88";
  BYTE *keycfg;
  WORD ret;
  BYTE buf[32];
  DWORD keyin;
  BYTE * keycontent;
  
  ClearDispMW();
  DispLineMW("Choose TMK Key Length:", MW_LINE1, MW_LEFT|MW_SPFONT);
  DispLineMW("1.SINGLE", MW_LINE2, MW_LEFT|MW_SPFONT);
  DispLineMW("2.DOUBLE", MW_LINE3, MW_LEFT|MW_SPFONT);
  DispLineMW("3.TRIPLE", MW_LINE4, MW_LEFT|MW_SPFONT);

  while (1) {
    keyin = GetCharMW();
    
    switch (keyin) {
      case MWKEY_1:
        keylen = 8;
        keysize = KEYMODE_SINGLE;
        break;
        
      case MWKEY_2:
        keylen = 16;
        keysize = KEYMODE_DOUBLE;
        break;
        
      case MWKEY_3:
        keylen = 24;
        keysize = KEYMODE_TRIPLE;
        break;
    }
    if ((keyin == MWKEY_1) || (keyin == MWKEY_2) || (keyin == MWKEY_3))
      break;
    else if (keyin == MWKEY_CANCL)
      return FALSE;
  }
  
  keycontent = MallocMW(keylen);
  memcpy(keycontent, KTMK, keylen);
  keycfg = MallocMW(4+keylen);

  // injecting the TMK
  keycfg[0] = KEYMODE_MKEY | keysize;
  keycfg[1] = MKEY_IDX;
  memcpy(&keycfg[2], "\xFF\x00", 2);
  memcpy(&keycfg[4], keycontent, keylen);
  
  ret = KDLL_KeyInject(keycfg);
  if (ret) {
    DispLineMW("TMKI Failed!", MW_LINE5, MW_CENTER|MW_BIGFONT);
    SprintfMW(buf, "Ret = %d", ret);
    DispLineMW(buf, MW_LINE7, MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(200, 0);
    return FALSE;
  }
  FreeMW(keycontent);
  FreeMW(keycfg);
  Short2Beep();
  DispLineMW("TMKI OK", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  APM_WaitKey(9000, 0);
  return TRUE;
}
/******************************************************************************
 *  Function        : LoadKeys
 *  Description     : Load data encrytion/decryption keys
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void LoadKeys(void)
{
  // plaintext encrypt key: 0000111122223333
  const BYTE KEncrypt1[] = "\x96\xC6\x16\x8A\x2E\x9F\x15\xEB";
  // plaintext encrypt key: 0000111122223333 4444555566667777
  const BYTE KEncrypt2[] = "\x26\xA7\xD1\x19\x3D\x01\x14\xF6\x46\x52\x57\x3C\x22\xD8\x35\xE2";
  // plaintext encrypt key: 0000111122223333 4444555566667777 88889999AAAABBBB
  const BYTE KEncrypt3[] = "\xA6\xA4\xD1\x6B\xAE\xE6\x8A\x8C\x65\x15\x44\xEF\xB9\x63\x4F\x9D\xB6\xBF\xBC\xE4\x61\xF1\xDD\xF6";
  // plaintext decrypt key: CCCCDDDDEEEEFFFF
  const BYTE KDecrypt1[] = "\x24\x22\xEA\x54\x1A\x8A\x1E\x35";
  // plaintest decrypt key: CCCCDDDDEEEEFFFF 0011223344556677
  const BYTE KDecrypt2[] = "\x9C\x15\x9E\x60\xC4\x4F\x8D\x6F\x31\xA7\x36\x4C\xAC\x91\xCA\x39";
  // plaintest decrypt key: CCCCDDDDEEEEFFFF 0011223344556677 8899AABBCCDDEEFF
  const BYTE KDecrypt3[] = "\x73\x36\x2D\xC6\x79\xD1\xA1\x3C\x0F\x16\xDE\xA6\xD4\x4E\x34\xC9\x6A\xFF\xB7\xAC\x9B\x82\xEE\x76";
  BYTE * keycfg;
  BYTE * keycontent;

  ClearDispMW();
  
  keycontent = MallocMW(keylen);
  if (keysize == KEYMODE_SINGLE)
    memcpy(keycontent, KEncrypt1, keylen);
  if (keysize == KEYMODE_DOUBLE)
    memcpy(keycontent, KEncrypt2, keylen);
  if (keysize == KEYMODE_TRIPLE)
    memcpy(keycontent, KEncrypt3, keylen);
  keycfg = MallocMW(4+keylen);
  
  // injecting the encryption key
  keycfg[0] = KEYMODE_ENC | keysize;
  keycfg[1] = EKEY_IDX;
  keycfg[2] = MKEY_IDX;
  keycfg[3] = 0x00;
  memcpy(&keycfg[4], keycontent, keylen);
  
  if (KDLL_KeyInject(keycfg)) {
    DispLineMW("Inject Failed!", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  else {
    DispLineMW("Inject EKEY OK", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  Short2Beep();
  APM_WaitKey(9000, 0);
  
  if (keysize == KEYMODE_SINGLE)
    memcpy(keycontent, KDecrypt1, keylen);
  if (keysize == KEYMODE_DOUBLE)
    memcpy(keycontent, KDecrypt2, keylen);
  if (keysize == KEYMODE_TRIPLE)
    memcpy(keycontent, KDecrypt3, keylen);
  
  // injecting the decryption key
  keycfg[0] = KEYMODE_DEC | keysize;   // double key length
  keycfg[1] = DKEY_IDX;
  keycfg[2] = MKEY_IDX;
  keycfg[3] = 0x00;
  memcpy(&keycfg[4], keycontent, keylen);
  
  if (KDLL_KeyInject(keycfg)) {
    DispLineMW("Inject Failed!", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  else {
    DispLineMW("Inject DKEY OK", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  FreeMW(keycontent);
  FreeMW(keycfg);
  Short2Beep();
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : LoadMacKey
 *  Description     : Load data encrytion/decryption keys
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void LoadMacKey(void)
{
  // plaintext mac key: 0011223344556677
  const BYTE KMac1[] = "\xCA\xDB\x67\x82\xEE\x2B\x48\x23";
  // plaintext mac key: 0011223344556677 8899AABBCCDDEEFF
  const BYTE KMac2[] = "\x31\xA7\x36\x4C\xAC\x91\xCA\x39\xC0\x48\x9F\x69\xBE\xC5\x4F\xA2";
  // plaintext mac key: 0011223344556677 8899AABBCCDDEEFF 0123456789ABCDEF
  const BYTE KMac3[] = "\x0F\x16\xDE\xA6\xD4\x4E\x34\xC9\x6A\xFF\xB7\xAC\x9B\x82\xEE\x76\xF6\x53\xF7\x0A\x75\xFE\x67\x8A";
  BYTE * keycfg;
  BYTE * keycontent;

  ClearDispMW();
  
  keycontent = MallocMW(keylen);
  if (keysize == KEYMODE_SINGLE)
    memcpy(keycontent, KMac1, keylen);
  if (keysize == KEYMODE_DOUBLE)
    memcpy(keycontent, KMac2, keylen);
  if (keysize == KEYMODE_TRIPLE)
    memcpy(keycontent, KMac3, keylen);
  keycfg = MallocMW(4+keylen);
  
  // injecting the encryption key
  keycfg[0] = KEY_STATUS_MAC | keysize;
  keycfg[1] = MACKEY_IDX;
  keycfg[2] = MKEY_IDX;
  keycfg[3] = 0x00;
  memcpy(&keycfg[4], keycontent, keylen);
  
  if (KDLL_KeyInject(keycfg)) {
    DispLineMW("Inject Failed!", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  else {
    DispLineMW("Inject MACKEY OK", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  FreeMW(keycontent);
  FreeMW(keycfg);
  Short2Beep();
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : EncData
 *  Description     : Encrypt data for testing
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void EncData(void)
{
  BYTE kbdbuf[49], buf[128];
  WORD ret;
  
  // plaintext data
  const BYTE data[] = "12345678ABCDEFGHstuvwxyz";
  
  ClearDispMW();
  memcpy(&kbdbuf[1], data, keylen);
  kbdbuf[0] = keylen;
  if (keylen == 8)
    ret = KDLL_KeyEncrypt(EKEY_IDX, &kbdbuf[1], kbdbuf[0]);
  else
    ret = KDLL_Key3Encrypt(EKEY_IDX, &kbdbuf[1], kbdbuf[0]);
  if (ret) {
    SprintfMW(buf, "Enc Err: %d", ret);
    DispLineMW(buf, MW_LINE5, MW_BIGFONT);
  }
  else {
    DispLineMW("Encrypted:", MW_LINE5, MW_SMFONT);
    split(buf, &kbdbuf[1], kbdbuf[0]);
    DispGotoMW(MW_LINE6, MW_SMFONT);
    DispPutNCMW(buf, kbdbuf[0]*2);
    DispCtrlMW(K_SelBigFont);
  }
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : DecData
 *  Description     : Decrypt data for testing
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void DecData(void)
{
  BYTE kbdbuf[49], buf[128];
  WORD ret;
  // plaintext data
  const BYTE data[] = "12345678ABCDEFGHstuvwxyz";
  
  ClearDispMW();
  memcpy(&kbdbuf[1], data, keylen);
  kbdbuf[0] = keylen;
  if (keylen == 8)
    ret = KDLL_KeyDecrypt(DKEY_IDX, &kbdbuf[1], kbdbuf[0]);
  else
    ret = KDLL_Key3Decrypt(DKEY_IDX, &kbdbuf[1], kbdbuf[0]);
  if (ret) {
    SprintfMW(buf, "Dec Err: %d", ret);
    DispLineMW(buf, MW_LINE5, MW_BIGFONT);
  }
  else {
    DispLineMW("Decrypted:", MW_LINE5, MW_SMFONT);
    split(buf, &kbdbuf[1], kbdbuf[0]);
    DispGotoMW(MW_LINE6, MW_SMFONT);
    DispPutNCMW(buf, kbdbuf[0]*2);
    DispCtrlMW(K_SelBigFont);
  }
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : MacData
 *  Description     : Cal MAC for data for testing
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void MacData(void)
{
  BYTE mac[16], buf[128];
  BYTE mode=0;                // refer to os_key_cbc_mac() in OSM
  WORD ret;
  
  // plaintext data
  BYTE data[] = "12345678ABCDEFGHstuvwxyz";
  
  ClearDispMW();
  memset(buf, 0, sizeof(buf));              // first 8 bytes - IV
  memcpy(&buf[8], data, strlen(data));
  mac[0] = MACKEY_IDX;
  if (keylen == 8)
    mode = 0;
  else if (keylen == 16)
    mode = 1;
  else if (keylen == 24)
    mode = 3;
  ret = KDLL_X99CalMAC(mac, buf, strlen(data)|(mode << 24));
  if (ret) {
    SprintfMW(buf, "MAC Err: %d", ret);
    DispLineMW(buf, MW_LINE5, MW_BIGFONT);
  }
  else {
    DispLineMW("MAC:", MW_LINE5, MW_SMFONT);
    split(buf, mac, 8);
    DispGotoMW(MW_LINE6, MW_SMFONT);
    DispPutNCMW(buf, 16);
    DispCtrlMW(K_SelBigFont);
  }
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : SetMode
 *  Description     : Set ECB/CBC mode for encryption/decryption
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void SetMode(void)
{
  BYTE key;
  BYTE kbdbuf[18];
  
  memset(kbdbuf, 0, sizeof(kbdbuf));
  DispLineMW("ENC/DEC Mode:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
  DispLineMW("[ENTER] - ECB", MW_LINE3, MW_BIGFONT);
  DispLineMW("[CLEAR] - CBC", MW_LINE5, MW_BIGFONT);
  switch (key=APM_WaitKey(9000, 0)) {
    case MWKEY_ENTER:
      KDLL_SetOpMode(MODE_ECB, kbdbuf);
      Short2Beep();
      break;
    case MWKEY_CLR:
      DispLineMW("Init Vector:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      if (!APM_GetKbd(HEX_INPUT+ECHO+MW_LINE3+RIGHT_JUST, IMIN(16)+IMAX(16), kbdbuf))
        return;
      compress(&kbdbuf[1], &kbdbuf[1], 8);
      KDLL_SetOpMode(MODE_CBC, &kbdbuf[1]);
      Short2Beep();
      break;
    default:
      break;
  }
}
/******************************************************************************
 *  Function        : GetPIN
 *  Description     : Get PIN from user
 *  Input           : N/A
 *  Return          : TRUE;           // get pin ok
 *                    FALSE;          // get pin failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN GetPIN(void)
{
  BYTE pinblk[11];
  BYTE msg[] = "\f\x11\nENTER PIN";
  //BYTE buf[32];
  
  memset(gGTS.s_input.sb_pin, ' ', 8);

  if (!key_ready(EKEY_IDX)) {
    DispLineMW("NO PIN KEY", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000,0);
    return FALSE;
  }

#if 1
  // Online PIN
  memcpy(pinblk, gGTS.s_input.sb_pan, 10);
  pinblk[10] = 0; //can pass PRM_4DGT_PIN and/or PRM_PIN_BYPASS
  if (KDLL_GetX98PIN(pinblk, msg, EKEY_IDX))
    return FALSE;
  else
    memcpy(gGTS.s_input.sb_pin, pinblk, 8);
#else
  // GetPIN demo
  memcpy(pinblk, "\x40\x12\x34\x56\x78\x90\x9F\xFF\xFF\xFF\x00", 11); //X9.24
  if (KDLL_GetX98PIN(pinblk, msg, EKEY_IDX))
    return FALSE;
  DispClrBelowMW(MW_LINE6);
  split(buf, pinblk, 8);
  DispGotoMW(MW_LINE7, MW_SPFONT);
  DispPutNCMW(buf, 16);
  DispCtrlMW(K_SelBigFont);
  APM_WaitKey(9000, 0);
#endif

  return TRUE;
}
/******************************************************************************
 *  Function        : SetDhostIdx
 *  Description     : Reset terminal DUKPT init key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN SetDhostIdx(void)
{
  BYTE kbdbuf[4];
  
  kbdbuf[0] = 1;
  kbdbuf[1] = '0' + dhost_idx;
  while (1) {
    DispLineMW("DHost Idx(0-4):", MW_LINE5, MW_CLRDISP|MW_BIGFONT);
    if (!APM_GetKbd(NUMERIC_INPUT+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMIN(1)+IMAX(1), kbdbuf))
      return FALSE;
    if (kbdbuf[1] < '5')
      break;
    else
      LongBeep();
  }
  dhost_idx = kbdbuf[1] - '0';
  return TRUE;
}
/******************************************************************************
 *  Function        : ResetDukptIkey
 *  Description     : Reset terminal DUKPT init key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN ResetDukptIkey(void)
{
  DispLineMW("Resetting Keys..", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  if (!KDLL_DukptKeyReset(dhost_idx)) {
    DispLineMW("DUKI-P Rst Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  if (!KDLL_DukptKeyReset(dhost_idx+1)) {
    DispLineMW("DUKI-M Rst Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  Short2Beep();
  return TRUE;
}
/******************************************************************************
 *  Function        : LoadDukptIkey
 *  Description     : Load terminal DUKPT init key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN LoadDukptIkey(void)
{
  BYTE keycfg[32];

  // injecting the initial key (for PIN)
  keycfg[0] = KEYMODE_DOUBLE;
  keycfg[1] = dhost_idx;
  // initial key (16 bytes)     6AC292FAA1315B4D 858AB3A3D7D5933A
  memcpy(&keycfg[2], "\x6A\xC2\x92\xFA\xA1\x31\x5B\x4D\x85\x8A\xB3\xA3\xD7\xD5\x93\x3A", 16);
  // KSN (10 bytes)             FFFF9876543210E00000  
  memcpy(&keycfg[18], "\xFF\xFF\x98\x76\x54\x32\x10\xE0\x00\x00", 10);
  
  DispLineMW("Loading IKeyP..", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  if (KDLL_DukptKeyInject(keycfg)) {
    DispLineMW("DUKI-P Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  
  // injecting the initial key (for MAC)
  keycfg[0] = KEYMODE_DOUBLE + KEY_STATUS_MAC;
  keycfg[1] = dhost_idx+1;
  // initial key (16 bytes)     6AC292FAA1315B4D 858AB3A3D7D5933A
  memcpy(&keycfg[2], "\x6A\xC2\x92\xFA\xA1\x31\x5B\x4D\x85\x8A\xB3\xA3\xD7\xD5\x93\x3A", 16);
  // KSN (10 bytes)             FFFF9876543210E00000  
  memcpy(&keycfg[18], "\xFF\xFF\x98\x76\x54\x32\x10\xE0\x00\x00", 10);
  
  DispLineMW("Loading IKeyM..", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  if (KDLL_DukptKeyInject(keycfg)) {
    DispLineMW("DUKI-M Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  Short2Beep();
  return TRUE;
}
/******************************************************************************
 *  Function        : GetDukptPin
 *  Description     : Get DUKPT encrypted pin block
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN GetDukptPin(void)
{
  const BYTE msg[]="\f\x11==== DUKPT =====\nENTER PIN:\x0";
  BYTE pinblk[32];
  BYTE buf[32];

  // format the PAN
  memcpy(pinblk, "\x40\x12\x34\x56\x78\x90\x9F\xFF\xFF\xFF", 10); //X9.24
  
  if (KDLL_DukptGetPin(pinblk, msg, dhost_idx)) {
    DispLineMW("DGetPIN Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  DispClrBelowMW(MW_LINE6);
  split(buf, pinblk, 8);            // PIN block (8B)
  DispGotoMW(MW_LINE7, MW_SPFONT);
  DispPutNCMW(buf, 16);
  split(buf, &pinblk[8], 10);       // KSN (10B)
  DispGotoMW(MW_LINE8, MW_SPFONT);
  DispPutNCMW(buf, 20);
  DispCtrlMW(K_SelBigFont);
  APM_WaitKey(9000, 0);
  return TRUE;
}
/******************************************************************************
 *  Function        : GetDukptMac
 *  Description     : Get DUKPT MAC data
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN GetDukptMac(void)
{
  BYTE mac[8];
  BYTE msg[16], buf[128];
  WORD ret;

  // format the PAN
  memcpy(msg, "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF", 16);
  mac[0] = dhost_idx+1;
  ret = KDLL_DukptCalMAC(mac, msg, sizeof(msg));
  if (ret) {
    SprintfMW(buf, "DGetMAC Failed[%d]!", ret);
    DispLineMW(buf, MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  ClearDispMW();
  split(buf, mac, 8);               // MAC data (8B)
  DispGotoMW(MW_LINE7, MW_SPFONT);
  DispPutNCMW(buf, 16);
  split(buf, &mac[8], 10);          // KSN (10B)
  DispGotoMW(MW_LINE8, MW_SPFONT);
  DispPutNCMW(buf, 20);
  DispCtrlMW(K_SelBigFont);
  APM_WaitKey(9000, 0);
  return TRUE;
}
/******************************************************************************
 *  Function        : IccTmkInject
 *  Description     : Get TMK from ICC
 *  Input           : N/A
 *  Return          : N/A           
 *                    N/A         
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void IccTmkInject(void)
{
  BYTE key;
  
  DispLineMW("ICC TMK INJECT:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
  DispLineMW("[ENTER] - Normal", MW_LINE3, MW_BIGFONT);
  DispLineMW("[CLEAR] - DUKPT", MW_LINE5, MW_BIGFONT);
  switch (key=APM_WaitKey(9000, 0)) {
    case MWKEY_ENTER:
      KDLL_IccMKeyInject(MKEY_IDX, 0);    // Terminal Master Key
      break;
    case MWKEY_CLR:
      KDLL_IccMKeyInject(MKEY_IDX, 1);    // DUKPT Initial Key
      break;
    default:
      break;
  }
  WaitICCRemove();
}
