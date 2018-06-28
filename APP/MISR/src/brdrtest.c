//-----------------------------------------------------------------------------
//  File          : brdrtest.c
//  Module        :
//  Description   : Include Barcode reader test.
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
//  05 Mar  2013  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include "midware.h"
#include "apm.h"
#include "sysutil.h"

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------



//*****************************************************************************
//  Function        : BRdrTest
//  Description     : Test barcode reader.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void BRdrTest(void)
{
#if (A5T1000|T1000)
  int fd;
  char dev_name[24];
  DWORD keyin, old_status, status, timeout, len;
  BYTE tmp[2048];
  BYTE endl=0;

#ifdef K_HdBarcode
  if ((os_hd_config() & K_HdBarcode) == K_HdNoBarcode)
    return;
#endif
#ifdef K_XHdBarcode
  if ((os_hd_config_extend() & K_XHdBarcode) == K_XHdNoBarcode)
    return;
#endif
  
  while (!endl) {
    keyin = 0;
    strcpy(dev_name, DEV_BRDR);
    fd = OpenMW(dev_name, MW_RDONLY);
    if (fd >= 0) {
      DispLineMW("Barcode Scanning", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
      status = StatMW(fd,0,NULL);
      if (status != MW_BRDR_STAT_SCANNING) {
        DispLineMW("Cancel To Stop", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
        timeout = -1;
        if (!IOCtlMW(fd, IO_BRDR_SCAN, &timeout)) {
          DispLineMW("Scan Fail!", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
          Delay10ms(300);
        }
      }
      //DispLineMW("Enter To Scan", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
      if (IOCtlMW(fd, IO_BRDR_ENABLE, NULL)) {
        old_status = -1;
        while (1) {
          SleepMW();
          status = StatMW(fd,0,NULL);
          if (old_status != status) {
            sprintf(tmp, "Status:%08X", status);
            old_status = status;
            DispLineMW(tmp, MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
          }
  
          keyin = GetCharMW();
          if (keyin==MWKEY_CANCL) {
            if (status==MW_BRDR_STAT_SCANNING) {
              keyin = 1;
              while (!IOCtlMW(fd, IO_BRDR_ABORT, NULL)) {
                sprintf(tmp, "Abort Err : %02d", keyin++);
                DispLineMW(tmp, MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
                if (GetCharMW()==MWKEY_CANCL)
                  break;
                Delay10ms(100);
              }
            }
            endl = 1;
            break;
          }
          if (keyin==MWKEY_ENTER) {
            if (status != MW_BRDR_STAT_SCANNING) {
              DispLineMW("Cancel To Stop", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
              timeout = -1;
              if (!IOCtlMW(fd, IO_BRDR_SCAN, &timeout)) {
                DispLineMW("Scan Fail!", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
                Delay10ms(300);
              }
            }
          }
          if (status == MW_BRDR_STAT_DATARDY) {
            if (len = ReadMW(fd, tmp, sizeof(tmp))) {
              tmp[len > 180 ? 180 : len] = 0;
              DispClrBelowMW(MW_LINE1);
              DispPutNCMW(tmp, strlen(tmp));
              sprintf(tmp, "Len: %04d bytes", len);
              DispLineMW(tmp, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
              Short2Beep();
              IOCtlMW(fd, IO_BRDR_ABORT, NULL);
              //DispLineMW("Enter To Scan", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
              APM_WaitKey(9000, 0);
              old_status = -1;
              break;
            }
          }
        }
      }
      else {
        DispLineMW("BRdr Open Fail!", MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
        APM_WaitKey(9000, 0);
        endl = 1;
      }
      CloseMW(fd);
    }
  }
#endif
}
