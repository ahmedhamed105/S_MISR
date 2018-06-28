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
#include "util.h"
#include "midware.h"
#include "apm.h"
#include "sysutil.h"
#include "corevar.h"
#include "input.h"
#include "tranutil.h"
#include "brdrtest.h"

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
BYTE gProdName[MAX_PROD_NAME][MAX_PROD_STR_LEN+1];    // product names
BYTE gProdCnt;                                        // product count

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
//*****************************************************************************
//  Function        : BRdrGetString
//  Description     : Test barcode reader.
//  Input           : N/A
//  Return          : BRD_RES_OK      - Data Read;
//                    BRD_RES_NO_DEV  - No Device;
//                    BRD_RES_EN_ERR  - Enable Error;
//                    BRD_RES_FAIL    - Fail;
//                    BRD_RES_TO      - Timeout;
//                    BRD_RES_CANCL   - Cancelled
//  Note            : JJJ
//  Globals Changed : N/A
//*****************************************************************************
BYTE BRdrGetString(BYTE *aStr, DWORD *aLen)
{
#if (A5T1000|T1000)
  int fd;
  char dev_name[24];
  DWORD keyin, old_status, status, timeout, len;
  BYTE tmp[2048];
  BYTE endl=0;
  BYTE ret=BRD_RES_NO_DEV;

#ifdef K_HdBarcode
  if ((os_hd_config() & K_HdBarcode) == K_HdNoBarcode)
    return ret;
#endif
#ifdef K_XHdBarcode
  if ((os_hd_config_extend() & K_XHdBarcode) == K_XHdNoBarcode)
    return ret;
#endif
  
  strcpy(dev_name, DEV_BRDR);
  fd = OpenMW(dev_name, MW_RDONLY);
  if (fd < 0)
    return ret;
  
  while (!endl) {
    keyin = 0;
    DispLineMW("Barcode Scanning", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
    status = StatMW(fd,0,NULL);
    if (status != MW_BRDR_STAT_SCANNING) {
      DispLineMW("Cancel To Stop", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
      timeout = -1;
      if (!IOCtlMW(fd, IO_BRDR_SCAN, &timeout)) {
        //DispLineMW("Scan Fail!", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
        //Delay10ms(300);
        ret = BRD_RES_FAIL;
        break;
      }
    }
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
              //sprintf(tmp, "Abort Err : %02d", keyin++);
              //DispLineMW(tmp, MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
              if (GetCharMW()==MWKEY_CANCL)
                break;
              Delay10ms(20);
            }
          }
          ret = BRD_RES_CANCL;
          endl = 1;
          break;
        }
        if (status == MW_BRDR_STAT_DATARDY) {
          memset(tmp, 0, sizeof(tmp));
          if (len = ReadMW(fd, tmp, sizeof(tmp))) {
            if (len > *aLen)
              len = *aLen;
            memcpy(aStr, tmp, len);
            *aLen = len;
            IOCtlMW(fd, IO_BRDR_ABORT, NULL);
            //DispLineMW("Enter To Scan", MW_LINE1, MW_CENTER|MW_REVERSE|MW_BIGFONT);
            //APM_WaitKey(9000, 0);
            //old_status = -1;
            Short1Beep();
            ret = BRD_RES_OK;
            endl = 1;
            break;
          }
        }
      }
    }
    else {
      //DispLineMW("BRdr Open Fail!", MW_MAX_LINE, MW_CENTER|MW_REVERSE|MW_SPFONT);
      //APM_WaitKey(9000, 0);
      ret = BRD_RES_EN_ERR;
      break;
    }
  }
  CloseMW(fd);
  return ret;
#endif
}
//*****************************************************************************
//  Function        : BRdrGetAmount
//  Description     : Test barcode reader.
//  Input           : N/A
//  Return          : N/A
//  Note            : JJJ
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN BRdrGetAmount(void)
{
  DWORD len;
  BYTE product[MAX_PROD_STR_LEN+1];
  BYTE buf[48], tmp[12+1];
  BYTE lead0len;
  BYTE ret=FALSE;
  BYTE i, endl=0, scan=1;
  
  memset(gProdName, 0, sizeof(gProdName));
  gProdCnt = 0;
  while (!endl) {
    if (scan) {
      memset(product, 0, sizeof(product));
      len = sizeof(product)-1;
      ret = BRdrGetString(product, &len);
      if (ret == BRD_RES_OK) {
        // barcode scan successfully
        memcpy(&gProdName[gProdCnt][0], product, len);
        gProdCnt++;
        INPUT.dd_amount += 1000;                              // dummy amount ($10) for each product
      }
      else if (ret != BRD_RES_CANCL) {
        SprintfMW(buf, "BRdr Err: %d", ret);
        DispLineMW(buf, MW_LINE5, MW_CLRDISP|MW_SPFONT|MW_CENTER);
        APM_WaitKey(9000, 0);
        memset(gProdName, 0, sizeof(gProdName));
        gProdCnt = 0;
        INPUT.dd_amount = 0;
        break;
      }
    }
    scan = 0;
    // barcode scanning stopped
    ClearDispMW();
    for (i=0; i<gProdCnt; i++) {
      memset(buf, 0, sizeof(buf));
      SprintfMW(buf, "%d-%s", i+1, &gProdName[i][0]);
      DispLineMW(buf, MW_LINE1+(MW_LINE2-MW_LINE1)*i, MW_SPFONT|MW_LEFT);
    }
    if (INPUT.dd_amount) {
      memset(buf, 0, sizeof(buf));
      memset(tmp, 0, sizeof(tmp));
      dbin2bcd(tmp, INPUT.dd_amount);
      split(buf, tmp, 10);
      lead0len = (BYTE)skpb(buf, '0', 20);                  /* leading zero count */
      SprintfMW(buf, "Total[%d]: $%s\x05", gProdCnt, &buf[lead0len]);
    }
    else
      SprintfMW(buf, "Total[%d]: $0\x05", gProdCnt);
    DispLineMW(buf, MW_LINE8, MW_REVERSE|MW_SPFONT|MW_LEFT);
    DispLineMW("[UP/DN]NextItem   [OK]Pay", MW_MAX_LINE, MW_CLREOL|MW_SPFONT|MW_LEFT);
    switch (APM_WaitKey(9000, 0)) {
      case MWKEY_ENTER:
        Short2Beep();
        ret = TRUE;
        endl = 1;
        break;
      case MWKEY_UP:
      case MWKEY_DN:
        if (gProdCnt >= MAX_PROD_NAME)
          LongBeep();
        else
          scan = 1;
        break;
      case MWKEY_CANCL:
        memset(gProdName, 0, sizeof(gProdName));
        gProdCnt = 0;
        INPUT.dd_amount = 0;
        endl = 1;
        break;
      default:
        LongBeep();
        break;
    }
  }
  return ret;
}
