//-----------------------------------------------------------------------------
//  File          : apmconfig.c
//  Module        :
//  Description   : APM Configuration Functions.
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include "hwdef.h"
#include "midware.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "rmenu.h"
#include "message.h"
#include "toggle.h"
#include "comm.h"
#include "stis.h"
#include "termdata.h"
#include "inittran.h"
#include "hardware.h"
#include "apmconfig.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
//enum {
//  STIS_SETUP=0,
//  STIS_INIT,
//  STIS_SHOW,
//  STIS_PRINT,
//  MAX_STIS_ITEM,
//};


//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
//static struct RMENUDAT KStisMenu[MAX_STIS_ITEM+1] =
//{
//  { STIS_SETUP,   "Setup "     }, // extra space for current mark
//  { STIS_INIT,    "Init "      },
//  { STIS_SHOW,    "Show "      },
//  { STIS_PRINT,   "Print "     },
//  { -1,           ""           },
//};
//*****************************************************************************
//  Function        : LcdSetup
//  Description     : Adjust Lcd's contrast.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void LcdSetup(void)
{
  #define MAX_CONTRAST      63
  #define MAX_BRIGHTNESS     7
  #define MAX_TIMEOUT      255
  BYTE  buf[128];
  DWORD value, max_value;
  DWORD keyin;

  #if (PR500|PR608D|R700|T700|TIRO|PR608)
  // Contrast
  DispLineMW("LCD Contrast", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SPFONT); 
  GetSysCfgMW(MW_SYSCFG_LCDCONTRAST, buf);
  value     = buf[0];
  max_value = MAX_CONTRAST;
  keyin = 0;
  do {
    if ((keyin == MWKEY_ENTER) && (value < max_value))
      value++;
    if ((keyin == MWKEY_CLR) && (value > 0))
      value--;

    if ((keyin == MWKEY_ENTER) || (keyin == MWKEY_CLR) || (keyin == 0)) {
      os_disp_cont(value);
      sprintf(buf, "Level: %03d", value);
      DispLineMW(buf, MW_LINE4, MW_BIGFONT);
      DispLineMW("+:<Enter> -:<Clear>", MW_LINE7, MW_CENTER|MW_SPFONT);
    }
    keyin = 0x00;
    SleepMW();
  } while ((keyin=GetCharMW()) != MWKEY_CANCL);
  PutSysCfgMW(MW_SYSCFG_LCDCONTRAST, &value);
  UpdSysCfgMW();
  #endif

  #if (T300|T810|A5T1000|T1000|T800)
  // Brightness
  DispLineMW("LCD Brightness", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SPFONT); 
  GetSysCfgMW(MW_SYSCFG_LCDB, buf);
  value     = buf[0];
  max_value = MAX_BRIGHTNESS;
  keyin = 0;
  do {
    if ((keyin == MWKEY_ENTER) && (value < max_value))
      value++;
    if ((keyin == MWKEY_CLR) && (value > 0))
      value--;

    if ((keyin == MWKEY_ENTER) || (keyin == MWKEY_CLR) || (keyin == 0)) {
      os_disp_brightness(value);
      sprintf(buf, "Level: %03d", value);
      DispLineMW(buf, MW_LINE4, MW_BIGFONT);
      DispLineMW("+:<Enter> -:<Clear>", MW_LINE7, MW_CENTER|MW_SPFONT);
    }
    keyin = 0x00;
    SleepMW();
  } while ((keyin=GetCharMW()) != MWKEY_CANCL);

  PutSysCfgMW(MW_SYSCFG_LCDB, &value);
  UpdSysCfgMW();


  // Timeout
  DispLineMW("LCD Timeout", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SPFONT); 
  GetSysCfgMW(MW_SYSCFG_LCDTIMEOUT, buf);
  value     = buf[0];
  max_value = MAX_TIMEOUT;
  keyin = 0;
  do {
    if (keyin == MWKEY_ENTER) {
      if (value < max_value)
        value++;
      else if (value == max_value)
        value = 0;
    }
    else if (keyin == MWKEY_CLR) {
      if (value > 0)
        value--;
      else if (value == 0)
        value = MAX_TIMEOUT;
    }

    if ((keyin == MWKEY_ENTER) || (keyin == MWKEY_CLR) || (keyin == 0)) {
      os_disp_bl_control(value);
      sprintf(buf, "Timeout: %03d", value);
      DispLineMW(buf, MW_LINE4, MW_BIGFONT);
      DispLineMW("+:<Enter> -:<Clear>", MW_LINE7, MW_CENTER|MW_SPFONT);
    }
    keyin = 0x00;
    SleepMW();
  } while ((keyin=GetCharMW()) != MWKEY_CANCL);

  PutSysCfgMW(MW_SYSCFG_LCDTIMEOUT, &value);
  UpdSysCfgMW();
  #endif //(T300|T810|A5T1000|T1000|T800)
}
#if (T300|T810|A5T1000|T1000|T800)    
//*****************************************************************************
//  Function        : KbdBacklight
//  Description     : Toggle Keyboard Backlight control
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void KbdBacklight(void)
{
  BYTE kbdbl, cur_val, buf[4];
  DWORD value;

  GetSysCfgMW(MW_SYSCFG_KBDBL, &kbdbl);
  DispLineMW(KKbdBacklight, MW_LINE3, MW_CLRDISP|MW_BIGFONT);

  cur_val = ToggleOption(NULL, KEnable, kbdbl|TOGGLE_MODE_4LINE);
  if ((cur_val != kbdbl) && (cur_val != 0xFF))  {
    kbdbl = cur_val;
    PutSysCfgMW(MW_SYSCFG_KBDBL, &kbdbl);
    UpdSysCfgMW();
    // set LCD backlight once to let KBD backlight change
    GetSysCfgMW(MW_SYSCFG_LCDTIMEOUT, buf);
    value = buf[0];
    os_disp_bl_control(value);
  }
}
#endif // (T800)
//*****************************************************************************
//  Function        : DispTermIP
//  Description     : Display terminal ip, netmask, gateway ip.
//  Input           : aIP;      // pointer to ip info.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispTermIP(BYTE *aIP)
{
  DispClrBelowMW(MW_LINE2);
  DispLineMW("IP Address", MW_LINE2, MW_SMFONT);
  ShowIp(aIP, MW_LINE3, FALSE);
  DispLineMW("Netmask", MW_LINE4, MW_SMFONT);
  ShowIp(&aIP[4], MW_LINE5, FALSE);
  DispLineMW("Gateway IP", MW_LINE6, MW_SMFONT);
  ShowIp(&aIP[8], MW_LINE7, FALSE);
}
//*****************************************************************************
//  Function        : ConfigTermIP
//  Description     : Setup ip, netmask, gateway ip
//  Input           : aIP;      // pointer to ip info.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ConfigTermIP(BYTE *aIP)
{
  DispLineMW(KIpHdr, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispTermIP(aIP);

  if (!EditIp(aIP, MW_LINE3))
    return FALSE;

  if (!EditIp(&aIP[4], MW_LINE5))
    return FALSE;

  if (!EditIp(&aIP[8], MW_LINE7))
    return FALSE;

  DispLineMW(KEnterSave, MW_LINE8, MW_REVERSE|MW_CLREOL|MW_CENTER|MW_SPFONT);

  if (WaitKey(KBD_TIMEOUT) != MWKEY_ENTER)
    return FALSE;

  return TRUE;
}
//*****************************************************************************
//  Function        : ConfigTermIP2
//  Description     : Setup ip, netmask, gateway ip
//  Input           : aIP;      // pointer to ip info.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ConfigTermIP2(BYTE *aIP)
{
  DispLineMW(KIpHdr, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispTermIP(aIP);

  if (!EditIp(aIP, MW_LINE3))
    return FALSE;

  if (!EditIp(&aIP[4], MW_LINE5))
    return FALSE;

  if (!EditIp(&aIP[8], MW_LINE7))
    return FALSE;

  DispClrBelowMW(MW_LINE4);
  DispLineMW("DNS1 IP", MW_LINE4, MW_SMFONT);
  if (!EditIp(&aIP[12], MW_LINE5))
    return FALSE;

  DispLineMW("DNS2 IP", MW_LINE6, MW_SMFONT);
  if (!EditIp(&aIP[16], MW_LINE7))
    return FALSE;
  
  DispLineMW(KEnterSave, MW_LINE8, MW_REVERSE|MW_CLREOL|MW_CENTER|MW_SPFONT);

  if (WaitKey(KBD_TIMEOUT) != MWKEY_ENTER)
    return FALSE;

  return TRUE;
}
//*****************************************************************************
//  Function        : ToggleDHCP
//  Description     : Toggle DHCP setting
//  Input           : aDHCP;            // current DHCP setting
//                    aIP;              // current IP config
//  Return          : TRUE/FALSE;       // TRUE => state changed.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ToggleDHCP(BYTE *aDHCP, BYTE *aIP)
{
  BOOLEAN ret;
  BYTE  cur_val;

  ret = FALSE;

  DispLineMW(KDHCPHdr, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispTermIP(aIP);

  cur_val = ToggleOption(NULL, KEnable, *aDHCP|TOGGLE_MODE_8LINE);
  if ((cur_val != *aDHCP) && (cur_val != 0xFF))  {
    *aDHCP = cur_val;
    ret = TRUE;
  }
  return ret;
}
#if (CDC_SUPPORT)           // Condition Compile for Codec Support
//*****************************************************************************
//  Function        : SetCodecVol
//  Description     : Adjust Codec Volume.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetCodecVol(void)
{
  DWORD volume;
  BYTE  tmp[256];

  volume = StatMW(gDevHandle[APM_SDEV_CDC], MW_CODEC_STATUS, NULL);
  volume >>= 8;
  DispLineMW("ADJUST VOLUME", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
  DispLineMW("ENTER - UP", MW_LINE5, MW_CENTER|MW_BIGFONT);
  DispLineMW("CLEAR - DOWN", MW_LINE7, MW_CENTER|MW_BIGFONT);

  while (TRUE) {
    sprintf(tmp, "-%02d-", volume);
    DispLineMW(tmp, MW_LINE4, MW_CENTER|MW_SMFONT);
    switch (YesNo()) {
      case 1:
        if (volume > 0)
          volume--;
        break;
      case 2:
        if (volume < 0x0F)
          volume++;
        break;
      default:
        StatMW(gDevHandle[APM_SDEV_CDC], MW_CODEC_VOL|volume, NULL);  // Set Volume
        return;
    }
  }
}
#endif
