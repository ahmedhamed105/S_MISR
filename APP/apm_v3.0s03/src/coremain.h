//-----------------------------------------------------------------------------
//  File          : coremain.h
//  Module        :
//  Description   :
//  Author        : Auto Generate by coremain_h.bat
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
//  05 Apr  2012  Lewis       Initial Version.
//  31 May  2012  John        Fix bug when changing DHCP setting & saving batch
//                            Change GetKbd() to allow initial digit in AMOUNT_INPUT
//  20 Jun  2012  John        Add ECR PP300 command
//                            Update MenuSelect() & ShowGraphicT800() for color/mono
//                            Allow KBD backlight change immediately
//  10 Aug  2012  John        Fix bug in GetKbd()
//  21 Sep  2012  Kenneth     Fix bug in saving records
//  14 Dec  2012  John        Put reversal as first batch record
//  15 Mar  2013  John        Fix bug in SSL connection
//  10 Oct  2013  John        Fix bug in LAN connection (LAN cable checking & DHCP)
//  29 Nov  2013  John        Fix bug in saving offline txn with reversal follows
//  04 Dec  2013  John        Allow input 'B' in PABX code (KEY_ALPHA define in apm.h)
//                            Use auto speed with V.42 in ASYNC modem connection
//  13 Dec  2013  Kenneth     Fix bug in UpdateDisplay function in kbdutil.c
//  03 Apr  2014  John        Fix bug in setting external pinpad
//  11 Jun  2014  John        Fix bug in GetKbd (prefix with leading zero amount)
//  13 Aug  2014  John        Fix bug in STIS download over COM1 & COM2(AUX0)
//  26 Aug  2014  John        Fix bug in CheckHostRsp() if more than 99 card tables
//  05 Sep  2014  John        Add support of Wifi unit
//  26 Nov  2014  John        Fix bug in ConnectMdmLine() to dial secondary phone
//  31 Dec  2014  John        Fix bug in connecting to unavailable Wifi AP
//  08 Jan  2015  John        Add support to download parameters over Wifi
//  27 Feb  2015  John        Fix bug in Wifi status return (KConnStsMsg[])
//  26 Mar  2015  John        Fix bug in handling failure in SChnlSetupMW()
//  10 Apr  2015  John        Add fix IP setup for Wifi in WifiIPFunc()
//  08 Sep  2015  John        Add ECR support for LAN/Wifi interfaces
//  04 Mar  2016  John        Fix ECR bug for using PP300Dll's function
//  08 Jun  2016  John        Add ECR support for Bluetooth interfaces
//  29 Sep  2016  John        Add support of Web-TMS download
//  29 Sep  2016  John        APN customization from application through PackComm()
//  03 Oct  2016  John        Fix additional clear line bug in GetKbd function
//  03 Oct  2016  John        Add handling of '*' & '#' input in GetKbd function
//  31 Oct  2016  John        Fix bug in checking timeout in GetKbd function
//  08 Nov  2016  John        Add support of enabling SSL over Bluetooth channel 1 (ECR)
//                            => PARAM0: CA key for connecting ECR (i.e. Bluetooth MCP Ch1)
//                                       - CA Key Index used = SSL_CA_IDX_ECR (0xFD)
//                                       - Extra ECR SSL Mode (0 / 1 / 2) to set in APM Func 98 (available in T300-MPOS only)
//                            => PARAM1: CA key for connecting Host (i.e. Bluetooth MCP Ch2, or normal host connection)
//                                       - CA Key Index used = SSL_CA_IDX_HOST (0xFF)
//                            => PARAM2: Client Cert for connecting ECR / Host
//                                       - Client Cert Index used = SSL_CLN_CERT_IDX (0xFF)
//                            => PARAM3: Client Private Key for connecting ECR / Host
//                                       - Client Private Key Index used = SSL_CLN_PRV_KEY_IDX (0xFF)
//                            => PARAM8: XML encoded parameters to be used in APM Func 9
//                            => PARAM9: CA key for connecting Web-TMS (if not loaded, will use the default Spectra's one)
//                                       - CA Key Index used = SSL_CA_IDX_TMS (0xFC)
//  07 Dec  2016  John        Add PwrSleep() API and fix bug in ConnectTcpip()
//  21 Apr  2017  John        Add cert format checking from parameter files before loading
//                            Add WaitMcpNetRdy() for MCP channel and change to call os_power_sleep_ak_pwake() for sleep
//  15 May  2017  Wai         Bug fix for malloc corrupt issue in FTP download parameters
//  18 May  2017  John        Bug fix for user stack overflow in FTP download .gz file
//  25 Jul  2017  John        Modify hardware default off on startup
//                            Modify PwrSleep() API to accept timeout parameter
//  08 Jan  2018  John        Modify PwrSleep() to keep BT on in sleep mode
//  23 Mar  2018  John        Fix crash if PP300DLL is not loaded
//-----------------------------------------------------------------------------
#ifndef _INC_COREMAIN_H_
#define _INC_COREMAIN_H_
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <string.h>
#include "common.h"
#include "system.h"
#include "apm.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
struct APM_FILE_DATA
{
  BYTE bDefaultAppID;
};

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
extern struct APM_FILE_DATA gFileData;

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
extern const BYTE  KSoftName[];
extern const DWORD KExportFunc[];

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------
extern int VirginInit(void);
extern void GlobalsInit(void);
extern void GlobalsCleanUp(void);
extern int Coremain(DWORD aParam1, DWORD aParam2, DWORD aParam3);
extern void LoadFile(void);
extern void SaveFile(void);

//-----------------------------------------------------------------------------
//      Defines will be called by cstartup.c as well.
//			** Don't change the names!! **
//-----------------------------------------------------------------------------
#define MY_APPL_ID               APM_ID         // application ID
#define SOFT_VERSION             0x03           // Software Version
#define SOFT_SUBVERSION          0x03           // Software Sub Version
#define APPL_TYPE                0x00           // 0x00=Application 0x01=DLL
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif // _INC_COREMAIN_H_
