//-----------------------------------------------------------------------------
//  File          : KeyDllTest.c
//  Module        :
//  Description   : Include Testing routine for KEYDLL Export Func.
//  Author        : Lewis
//  Notes         : N/A
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
//  22 Oct  2008  Lewis       Initial Version.
//  14 Jan  2009  Lewis       Add external PP300 support.
//-----------------------------------------------------------------------------
#include <string.h>
#include "key2dll.h"
#include "util.h"
#include "kbdutil.h"
#include "toggle.h"
#include "keydlltest.h"

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
#define MAX_KEY_INSERT   7
static char *KKeyInsCmd[MAX_KEY_INSERT] = {
  // Encrypt, Single, No KVC, Dest=1 , use Master Decrypt (3_)
  "\x08\x01\x00\x00\x87\x62\xD9\x1E\x23\x82\x82\x8D",                  
  // Encrypt, Single, 3 byte KVC, Dest = 1, use Master Decrypt(4_)
  "\x09\x07\x00\x00\x4E\x64\x86\x2A\xEB\xAE\x22\x9E\xE2\xF2\x43",      
  // Decrypt, Triple, No KVC, Dest=2 , use Master Decrypt (5_6_7)
  "\x1A\x02\x00\x00\x99\x31\xC3\x5E\x6A\xF4\x8B\xA2\x02\xD4\x9B\xF0\x6B\x1F\x59\xBC\x70\x40\x6C\xB9\x6B\x50\x84\xF8",  
  // Encrypt, Double, No KVC, Dest=3 , use Key2 Decrypt (7_8)
  "\x10\x03\x02\x00\xAF\x6F\x86\x08\xD8\xB6\x9D\x7C\x9F\xAD\x24\xB1\xAD\x26\x90\xEB",
  //Decryption Key, Double Key Len, No KVC Use Key2 decrypt (A_B)
  "\x12\x04\x02\x00\x44\x25\x26\x49\x8F\xD1\xB3\x55\xDB\x4A\x9C\x05\x55\x64\xA4\x4A",
  //Encryption Key, Triple Key Len, No KVC, Use Key2 Decrypt (C_D_E)
  "\x18\x05\x02\x00\xC5\xE8\xA7\x48\xD4\x60\x79\x79\x57\xA0\x46\xE5\x94\xF9\x13\x67\xEA\x9C\x00\xE0\x81\xE6\x29\xA3", 
  //Decryption key, Triple Key Len, No KVC, Use Key2 Decrypt (F_1_2)
  "\x1A\x06\x02\x00\x98\xA2\x93\x93\x8B\x31\xC6\xC0\x93\x6D\x9A\x37\x3C\xAC\xF2\x9B\x48\x46\xCA\x58\xFA\x42\x59\x41", 
};

static BYTE *KDukptPinResult[3] = {
  "\x1B\x9C\x18\x45\xEB\x99\x3A\x7A",
  "\x10\xA0\x1C\x8D\x02\xC6\x91\x07",
  "\x18\xDC\x07\xB9\x47\x97\xB4\x66"
};
//*****************************************************************************
//  Function        : ShowKeyStatus
//  Description     : Show Current Key status from KEYDLL
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ShowKeyStatus(void)
{
  BYTE tmp[40];
  int i;

  // Show Key Status
  KDLL_KeyStatus(tmp);
  DispLineMW("Key Status", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  for (i = 0; i < 40; i++) {
    if ((i%10)==0)
      printf("\n");
    printf("%02X", tmp[i]);
  }
  WaitKey(KBD_TIMEOUT);
}
//*****************************************************************************
//  Function        : KeyDllTest
//  Description     : Test Function for KEYDLL.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void KeyDllTest(void)
{
  BYTE tmp[256], mac[16];
  int i;
  WORD ret_val;

  DispLineMW("Use PP300 ?", MW_LINE3, MW_CLRDISP|MW_LEFT|MW_BIGFONT);
  if (ToggleYesNo()== 2)
    KDLL_ExtPPadSetup(MIDWARE_ID, SCHNLGETPINMW);  // Use External pinpad to get PIN
  else
    KDLL_ExtPPadSetup(0, 0);                       // Disable

  DispLineMW("Key Reset", MW_LINE8, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  KDLL_KeyReset(0xFF);
  ShowKeyStatus();

  DispLineMW("Key Injection", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  for (i = 0; i < MAX_KEY_INSERT; i++) {
    memcpy(tmp, KKeyInsCmd[i], sizeof(tmp));
    ret_val = KDLL_KeyInject(tmp);
    if (ret_val != RESP_OK) {
      DispLineMW("Injection Failed!", MW_LINE5, MW_SPFONT);
      sprintf(tmp, "Idx:%d Ret:%d", i, ret_val);
      DispLineMW(tmp, MW_LINE6, MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      return;
    }
  }
  ShowKeyStatus();

  DispLineMW("Encrypt/Decrypt Test", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  ret_val = RESP_OK;
  while (1) {
    #if (T300==0)&&(T810==0)&&(T1000==0)&&(A5T1000==0) // Test Case Not Apply for T1000
    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07", 8);
    ret_val = KDLL_KeyDecrypt(USE_LEFT|0x06, tmp, 8);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\x0E\x32\x5E\x11\x3D\xFC\xBA\x07", 8)!=0) {
      ret_val = 0x61;
      break;
    }
    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07", 8);
    ret_val = KDLL_KeyDecrypt(USE_THIRD|0x06, tmp, 8);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\x60\xCA\x64\x37\xD6\x2E\x50\xE8", 8)!=0) {
      ret_val = 0x63;
      break;
    }
    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07", 8);
    ret_val = KDLL_KeyEncrypt(USE_LEFT|0x05, tmp, 8);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\xAF\x96\xFE\xAC\xD8\x03\xF8\x81", 8)!=0) {
      ret_val = 0x51;
      break;
    }
    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07", 8);
    ret_val = KDLL_KeyEncrypt(USE_THIRD|0x05, tmp, 8);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\x14\x6D\x33\x64\xF7\xF0\x2B\x3D", 8)!=0) {
      ret_val = 0x53;
      break;
    }
    #endif //(T300==0)&&(T810==0)&&(T1000==0)&&(A5T1000==0)

    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
    ret_val = KDLL_Key3Decrypt(0x04, tmp, 16);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\xDC\x0A\x98\x94\xC4\xE5\xB4\x26\x36\xFC\xBA\x6B\x07\xC8\x7A\xD1", 16)!=0) {
      ret_val = 0x84;
      break;
    }

    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
    ret_val = KDLL_Key3Decrypt(0x06, tmp, 16);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\x48\x93\x8C\xA3\x63\x28\x1A\x90\x70\x39\x11\xA2\xA8\x0E\x5E\xEC", 16)!=0) {
      ret_val = 0x86;
      break;
    }

    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
    ret_val = KDLL_Key3Encrypt(0x03, tmp, 16);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\x96\x06\x03\x2E\xDB\xF0\xBA\x21\x4D\x5E\x12\xCB\x04\x6B\x45\x67", 16)!=0) {
      ret_val = 0x83;
      break;
    }

    memcpy(tmp, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F", 16);
    ret_val = KDLL_Key3Encrypt(0x05, tmp, 16);
    if (ret_val != RESP_OK)
      break;
    if (memcmp(tmp, "\x53\x32\x6E\x81\x2B\x86\x40\x5A\xE1\x8B\x61\x5C\x72\xE4\x9F\xA2", 16)!=0) {
      ret_val = 0x85;
      break;
    }

    DispClrBelowMW(MW_LINE3);
    DispLineMW("Test Success!", MW_LINE5, MW_CENTER|MW_SPFONT);
    ret_val = RESP_OK;
    WaitKey(KBD_TIMEOUT);
    break;
  }
  if (ret_val != RESP_OK) {
    DispClrBelowMW(MW_LINE3);
    sprintf(tmp, "Test Failed! %X", ret_val);
    DispLineMW(tmp, MW_LINE5, MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
    return;
  }

  #if (T300==0)&&(T810==0)&&(T1000==0)&&(A5T1000==0)
  // !!Current Keydll do not support inject MAC key.
  DispLineMW("MAC Test", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  mac[0] = 0x01;
  memcpy(tmp, "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA", 8);
  ret_val = RESP_OK;
  if ((KDLL_X99CalMAC(mac, tmp, 8) != RESP_OK) ||
      (memcmp(mac, "\x08\x46\x33\x28\x75\x3D\xC5\x70", 8) != 0) ) {
    DispLineMW("Test Failed!", MW_LINE5, MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
    return;
  }
  DispClrBelowMW(MW_LINE3);
  DispLineMW("Test Success!", MW_LINE5, MW_CENTER|MW_SPFONT);
  WaitKey(KBD_TIMEOUT);
  #endif


  DispLineMW("GetX98PIN Test", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  memcpy(mac, "\x01\x02\x03\x04\x05\x06\x07\x08\xFF", 9);
  DispGotoMW(MW_LINE3, MW_SPFONT);
  strcpy(tmp, "Ent PIN(123456):");
  ret_val = RESP_OK;
  if ((KDLL_GetX98PIN(mac, tmp, 0x01) != RESP_OK) ||
      (memcmp(mac, "\xF0\x0A\x4F\x80\x9B\x22\xB6\x44", 8) != 0) ) {
    DispClrBelowMW(MW_LINE3);
    DispLineMW("Test Failed!", MW_LINE5, MW_CENTER|MW_SPFONT);
    WaitKey(KBD_TIMEOUT);
    return;
  }
  DispClrBelowMW(MW_LINE3);
  DispLineMW("Test Success!", MW_LINE5, MW_CENTER|MW_SPFONT);
  WaitKey(KBD_TIMEOUT);

  DispLineMW("DUKPTPIN Test", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  memcpy(tmp, "\x10\x01\x6A\xC2\x92\xFA\xA1\x31\x5B\x4D\x85\x8A\xB3\xA3\xD7\xD5\x93\x3A\xFF\xFF\x98\x76\x54\x32\x10\xE0\x00\x00", 28);
  KDLL_DukptKeyInject(tmp);

  for (i = 0; i < 3; i++) {
    memcpy(mac, "\x40\x12\x34\x56\x78\x90\x9F", 7);
    strcpy(tmp, "Ent PIN(1234):");
    ret_val = RESP_OK;
    DispGotoMW(MW_LINE3, MW_SPFONT);
    if ((KDLL_DukptGetPin(mac, tmp, 1) != RESP_OK) ||
        (memcmp(mac, KDukptPinResult[i], 8) != 0) ) {
      DispClrBelowMW(MW_LINE3);
      DispLineMW("Test Failed!", MW_LINE5, MW_CENTER|MW_SPFONT);
      WaitKey(KBD_TIMEOUT);
      return;
    }
  }
  DispClrBelowMW(MW_LINE3);
  DispLineMW("Test Success!", MW_LINE5, MW_CENTER|MW_SPFONT);
  WaitKey(KBD_TIMEOUT);
}
