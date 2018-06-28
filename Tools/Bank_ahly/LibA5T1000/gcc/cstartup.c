//-----------------------------------------------------------------------------
//  File          : cstartup.c                                                     
//  Module        :                                                            
//  Description   : Main Entry.                                                
//  Author        : 
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
//#include <stdio.h>
#include "system.h"
#include "coremain.h"

//-----------------------------------------------------------------------------
//      Prototypes                                                             
//-----------------------------------------------------------------------------
//From Link Scripts
extern unsigned long _start_of_app;
extern unsigned long _end_of_app;
extern unsigned long _start_of_data;
extern unsigned long _end_of_data;
extern unsigned long _start_of_bss;
extern unsigned long _end_of_bss;
extern unsigned long _end_of_sram;
extern unsigned long _end_of_flash;

//From main.c
extern const T_APP_HDR KAppHdr;
extern int main(unsigned long aFuncNo, unsigned long aParam2, unsigned long aParam3);

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
const unsigned long KAppHeader[12] __attribute__ ((section ("sect_apphdr"))) =
{
  (unsigned long)&KAppHdr,          // app table ptr
  (unsigned long)&_start_of_app,    // crc check start at 0x50
  (unsigned long)&_end_of_flash,    // crc check end address
  (unsigned long)0,                 // crc will be overrided
  (unsigned long)&_start_of_data,   // SFB(DATA_I)
  (unsigned long)&_end_of_bss,      // SFE(DATA_N), Maybe 
  (unsigned long)(((unsigned long)MY_APPL_ID) | ((unsigned long)SOFT_VERSION<<8) | ((unsigned long)SOFT_SUBVERSION<<16) | ((unsigned long)APPL_TYPE<<24)),
  (unsigned long)0,                 // will be filled with YYYYMMDD                
  (unsigned long)0,                 // will be filled with HHMMSSNN
  (unsigned long)0,                 // fill with TMS checksum
  (unsigned long)0,                 // fill display checksum
  (unsigned long)0x80000000,        // crc of YYYYMMDDHHMMSSNN, TMS checksum & display checksum, sign data
};

const unsigned char KKek[8] __attribute__ ((section ("sect_kek"))) = "Spectra\x00";

//*****************************************************************************
//  Function        : SegmentInit
//  Description     : Initial Clean Up.
//  Input           : N/A                                                      
//  Return          : N/A                                                      
//  Note            : N/A                                                      
//  Globals Changed : N/A                                                      
//*****************************************************************************
void SegmentInit(void)
{
  unsigned long *src;
  unsigned long *dest;
  unsigned long *end_addr;

  //Debug
  //printf("\f%X\n%X\n%X\n%X\n%X\n%X\n%X\n%X"
  //        , &_start_of_app
  //        , &_end_of_app
  //        , &_start_of_data
  //        , &_end_of_data
  //        , &_start_of_bss
  //        , &_end_of_bss
  //        , &_end_of_sram
  //        , &_end_of_flash
  //      );
  //while (os_kbd_getkey () == 0) os_sleep();
 
  //------------------------------
  // Check data area size         
  //------------------------------
  if ((unsigned long)&_end_of_bss > (unsigned long)&_end_of_sram) {
    os_disp_puts("\f""Data oversized.");
    os_disp_puts("\nPls Power Off");
    while (os_kbd_getkey () == 0) os_sleep();
  }

  //------------------------------
  // Copy preset data             
  //------------------------------
  src      = &_end_of_app;
  dest     = &_start_of_data;
  end_addr = &_end_of_data;
  while (dest < end_addr) {
    *dest = *src;
    src++;
    dest++;
  }

  //------------------------------
  // Clear uninitialized data     
  //------------------------------
  dest = &_start_of_bss;
  end_addr = &_end_of_bss;
  while (dest < end_addr) {
    *dest = 0;
    dest++;
  }
}

//*****************************************************************************
//  Function        : cstartup
//  Description     : Entry point for application.                             
//  Input           : N/A                                                      
//  Return          : N/A                                                      
//  Note            : N/A                                                      
//  Globals Changed : N/A                                                      
//*****************************************************************************
int cstartup (unsigned long aFuncNo, unsigned long aParam2, unsigned long aParam3) __attribute__ ((section ("sect_startup")));
int cstartup (unsigned long aFuncNo, unsigned long aParam2, unsigned long aParam3)
{
  SegmentInit();
  return main(aFuncNo, aParam2, aParam3);
}

