//-----------------------------------------------------------------------------
//  File          : main.c
//  Module        :
//  Description   : Main Entry.
//  Author        : Auto Generate by main_c.bat
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
//  DD MMM  YYYY  main_c      Initial Version.
//-----------------------------------------------------------------------------
#include "system.h"
#include "coremain.h"

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
// Application Header
const T_APP_HDR KAppHdr = {
  (DWORD) "spectratech.com",
  (DWORD) KExportFunc,            // Export Function Table
  (DWORD) NULL,                   // Reserved
  (DWORD) KSoftName,              // Application Name
  (BYTE)  SOFT_VERSION,           // Version
  (BYTE)  SOFT_SUBVERSION,        // Sub Version
  (BYTE)  APPL_TYPE,              // Application Type 0 => Appl, 1 => Appl DLL
  (BYTE)  MY_APPL_ID,             // Application ID
  (DWORD) 0x12345678,             // Magic Number
};

//*****************************************************************************
//  Function        : MainApplication
//  Description     : Entry point for application.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int main(DWORD aParam1, DWORD aParam2, DWORD aParam3)
{
  int ret_val = 0;
  T_APP_INFO sappl_info;

  // Check Midware Exist
  if (!os_app_info(0x0F, &sappl_info)) {
    // Midware not exist
    os_disp_puts("\f\x12\nMidware Not Exist!");
    while (os_kbd_getkey() != K_KeyCancel) os_sleep();
    os_reset();
  }

  // Virgin Init
  if (os_app_get_virgin(MY_APPL_ID)) {
    VirginInit();
    os_app_clear_virgin(MY_APPL_ID);
  }

  // Initialize Globals Variables
  GlobalsInit();

  // Start application coremain
  if (KAppHdr.b_type == 0x00) {         // is application
    ret_val = Coremain(aParam1, aParam2, aParam3);
  }

  // House Keep
  GlobalsCleanUp();

  // Return to Caller.
  os_appcall_return(ret_val);

  return ret_val;
}
