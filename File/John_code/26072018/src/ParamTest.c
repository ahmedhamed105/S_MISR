//-----------------------------------------------------------------------------
//  File          : GLibTest.c
//  Module        :
//  Description   : Graphic Libary Test Program.
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
//  08 Jul  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "midware.h"
#if (T300|A5T1000|T1000|TIRO)
#include "apm.h"
#else
#include "apm_lite.h"
#endif
#include "sysutil.h"
#include "util.h"
#include "SPGLib.h"
#include "coremain.h"
#include "hardware.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define MAX_GD_WIDTH    320
#define MAX_GD_HEIGHT   216

#define MAX_GD_SIZE     (MAX_GD_WIDTH*MAX_GD_HEIGHT)

//-----------------------------------------------------------------------------
// Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static int gTmLptHandle = -1;
static int gBmpIdx = 0;
static int gBmpSize = 0;
static int gPrnIdx = 0;


//*****************************************************************************
//  Function        : DispDTG
//  Description     : Show data/time or training mode status on idle menu.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispDTG(void)
{
  struct DATETIME dtg;
  BYTE  tmp[MW_MAX_LINESIZE+1];

  TimerSetMW(gTimerHdl[TIMER_MAIN], 50);

  memset(tmp, 0, sizeof(tmp));
  ReadRTC(&dtg);
  ConvDateTime(tmp, &dtg, 1);
  memmove(&tmp[3], &tmp[4], 14);
  memmove(&tmp[6], &tmp[7], 10);

  tmp[16] = 0;
  os_disp_putc(K_PushCursor);
  os_disp_backc(RGB_WHITE);
  DispLineMW(tmp, MW_MAX_LINE, MW_CLREOL|MW_CENTER|MW_SPFONT);
  os_disp_putc(K_PopCursor);
}

//*****************************************************************************
//  Function        : DispParamFile
//  Description     : Display GIF data from param file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DispParamFile(BYTE aFileNo)
{
  T_FILE file_info, *pInfo;
  int    fd, fsize, rlen;
  BYTE   gif_file[64], *ptr;
  DWORD  ctrl;
 DWORD keyin;
  BYTE buf[64];
BYTE tmpbuf[64];
DWORD state;
DWORD tm_handle;

  if (aFileNo > 9)
    return FALSE;
  
  sprintf(gif_file, "PARAM%d", aFileNo);
  ctrl = K_SearchPub|K_SearchReset;
  while (1) {
    pInfo = os_file_search_data(ctrl, &file_info);
    if (pInfo == NULL)
      break;
    if (memcmp(gif_file, pInfo->s_name, 6) == 0)
      break;
    ctrl = K_SearchPub|K_SearchNext;
  }
  if (pInfo == NULL)
    return FALSE;
  
  strcpy(gif_file, pInfo->s_owner);
  strcat(gif_file, pInfo->s_name);
  fd = os_file_open(gif_file, K_O_RDONLY);
  fsize = os_file_length(fd);
  if ((fd != -1) && (fsize > 0)) {
    ptr = (BYTE*) MallocMW(fsize);
    if (ptr == NULL) {
      os_file_close(fd);
      return FALSE;
    }
    rlen = os_file_read(fd, ptr, fsize);
    os_file_close(fd);
  }
  else
    return FALSE;

  if (rlen != fsize) {
    FreeMW(ptr);
    return FALSE;
  }
  
  if (GIFLoadMem(ptr, fsize)) {
    GIFSetPos(0, 0);
    GIFPlay();

//	 os_rtc_get(buf);/* date and time */
//	 sprintf(tmpbuf, "Date %.4s/%.2s/%.2s %.2s:%.2s:%.2s",buf,&buf[4],&buf[6],&buf[8],&buf[10],&buf[12]);
  //   DispLineMW(tmpbuf, MW_LINE3, MW_REVERSE|MW_CENTER|MW_SPFONT);

	DispDTG();
     // if (APM_WaitKey(1000, 0) == MWKEY_ENTER){
    //      GIFStop(1);
    //      GIFUnLoad();
    //      FreeMW(ptr);
     //     return FALSE;
    //  }

   

 keyin = APM_WaitKey(1000, WAIT_ICC_INSERT | WAIT_MSR_READY);
  if (keyin == MWKEY_LEFT){
	  fuctiongo=1;
	    
  }
   #if (T300|A5T1000|T1000)
   if (keyin == MWKEY_UP ){
	  fuctiongo=2;    
  }
   #endif
     #if (T300|A5T1000|T1000)
   if (keyin == MWKEY_DN){
	  fuctiongo=3;    
  }
   #endif

   
//	 keyin = APM_WaitKey(1000, WAIT_ICC_INSERT | WAIT_MSR_READY);
    GIFStop(1);
    GIFUnLoad();
    FreeMW(ptr);
    return TRUE;
  }
  FreeMW(ptr);
  return FALSE;
}
//*****************************************************************************
//  Function        : ParamTest
//  Description     : Test Function for reading parameter file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD WaitPrnFinish(void)
{
    DWORD status;
    Delay10ms(50);
    status = StatMW(gTmLptHandle, MW_TMLPT_NORSTATUS, NULL);
    if (status & MW_TMLPT_FPRINTING) {
        do {
            SleepMW();
            status = StatMW(gTmLptHandle, MW_TMLPT_NORSTATUS, NULL);
            if ((status & 0xFF) != MW_TMLPT_SOK) { // Error
                break;
            }
        } while ((status & MW_TMLPT_FFINISH) == 0);
    }
    return status;
}
//*****************************************************************************
//  Function        : LptClose
//  Description     : Close the printer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
static void LptClose(void)
{
    WaitPrnFinish();
    CloseMW(gTmLptHandle);
    gTmLptHandle = -1;
}
//*****************************************************************************
//  Function        : LptOpen
//  Description     : Open and reset the printer.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
static DWORD LptOpen(void *aCfg)
{
    char filename[32];
    LptClose();
    strcpy(filename, DEV_LPT2);
    gTmLptHandle = OpenMW(filename, MW_WRONLY);
    if (gTmLptHandle >= 0) {
        if (IOCtlMW(gTmLptHandle, IO_TMLPT_CFG, aCfg) == -1)
            return FALSE;
        StatMW(gTmLptHandle, MW_TMLPT_RESET, NULL);
        return TRUE;
    }
    return FALSE;
}
//*****************************************************************************
//  Function        : LptPutC
//  Description     : Send the char to printer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
static DWORD LptPutC(DWORD aChar)
{
    DWORD status;
    
    // Loop until fatal error or char send to printer.
    while (1) {
        status = StatMW(gTmLptHandle, MW_TMLPT_NORSTATUS, NULL);
        switch (status & 0xFF) {
            case MW_TMLPT_SOK:
                break;
            case MW_TMLPT_STEMPHIGH:
                Delay10ms(5*100); // Wait 5 Second for printer cool down;
                StatMW(gTmLptHandle, MW_TMLPT_ERR_RESTART, NULL);
                break;
            case MW_TMLPT_SVOLTLOW:
                Delay10ms(5*100); // Wait 5 Second for power stable
                status = StatMW(gTmLptHandle, MW_TMLPT_NORSTATUS, NULL);
                if (status & MW_TMLPT_SVOLTLOW)  // real voltage low
                    return status;
                StatMW(gTmLptHandle, MW_TMLPT_ERR_RESTART, NULL);
                break;
            default :
                return status;
        }
        if ((status & 0xFF) == MW_TMLPT_SOK) {
            if (WriteMW(gTmLptHandle, &aChar, 1))
                break;
        }
    }
    return status;
}
//*****************************************************************************
//  Function        : LptPutN
//  Description     : Send the chars to printer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
static DWORD LptPutN(const BYTE *aDat, DWORD aLen)
{
    DWORD status = StatMW(gTmLptHandle, MW_TMLPT_NORSTATUS, NULL);
    while (aLen--) {
        status = LptPutC(*aDat);
        if ((status & 0xFF) != MW_TMLPT_SOK)
            break;
        aDat++;
    }
    return status;
}
//*****************************************************************************
//  Function        : PrintLogo
//  Description     : Print logo.
//  Input           : aWidth;     // in number of Byte
//                    aHeight;    // in number of Byte
//                    *aLogoPtr;  // pointer to constant data
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD PrintLogo(DWORD aWidth, DWORD aHeight, const BYTE *aLogoPtr)
{
    DWORD status, logobuf_size;
    BYTE cmd[4] = { 0x1B, 'K', 0x00, 0x00 };
    
    cmd[2] = (BYTE) (aWidth/8);
    cmd[3] = (BYTE) (aHeight>248?248:aHeight);
    
    status = LptPutN(cmd, 4);
    if ((status & 0xFF) != MW_TMLPT_SOK)
        return status;
    
    logobuf_size = aWidth/8 * cmd[3];
    status = LptPutN(aLogoPtr, logobuf_size);
    
    return status;
}
/*======================================================================*/
/* Convert a binary to packed BCD                                       */
/*======================================================================*/
static WORD pack_bin2bcd(WORD value, BYTE digit)
{
    WORD dec;
    BYTE ch,i;
    WORD result=0;
    
    for(i=0;i<digit-1;i++)
    {
        dec=1;
        for(ch=digit-i-1;ch;ch--)
            dec=dec*10;
        ch=0;
        while(value>=dec)
        {
            value-=dec;
            ch++;
            if(ch>9)
                return 0;
        }
        result<<=4;
        result|=ch;
    }
    result<<=4;
    result|=value;
    
    return result;
}
//*****************************************************************************
//  Function        : ReadBmp
//  Description     : Read bmp data to buffer
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD ReadBmp(BYTE *aBuf, BYTE *aBmp, DWORD aLen)
{
    DWORD len;
    
    len = aLen;
    if ((gBmpIdx + len) > gBmpSize)
        len = gBmpSize - gBmpIdx;
    
    memcpy(aBuf, &aBmp[gBmpIdx], len);
    gBmpIdx += len;
    return len;
}
//*****************************************************************************
//  Function        : SeekBmp
//  Description     : Seek bmp data according to offset
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SeekBmp(DWORD aOffset)
{
    gBmpIdx = aOffset;
}
//*****************************************************************************
//  Function        : WritePrn
//  Description     : Write prn data from buffer
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD WritePrn(BYTE *aPrn, BYTE *aBuf, DWORD aLen)
{
    memcpy(&aPrn[gPrnIdx], aBuf, aLen);
    gPrnIdx += aLen;
    return aLen;
}
//*****************************************************************************
//  Function        : ProcessLogoFile
//  Description     : Convert .bmp to printer data
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ProcessLogoFile(BYTE *aPrn, BYTE *aBmp, DWORD aLen)
{
    BYTE buf[128];
    DWORD filesize;
    DWORD bmp_offset;
    DWORD size;
    DWORD width;
    DWORD height;
    DWORD pitch,x,y;
    DWORD ptr;
    BYTE pixel[3];
    BYTE bw;
    BYTE R_th;
    BYTE G_th;
    BYTE B_th;
    BYTE header_only;
    BYTE half_dot;
    
    int j;
    WORD len,blen,k;
    BYTE ch;
    
    R_th = 127;
    G_th = 127;
    B_th = 127;
    
    header_only = 1;
    half_dot = 0xf9;
    
    gBmpIdx = gPrnIdx = 0;
    gBmpSize = aLen;
    do
    {
        bw = 0;
        //if (fReadMW(LogoTempFileHdr, buf, 14) != 14) {    /* Read file header */
        if (ReadBmp(buf, aBmp, 14) != 14) {    /* Read file header */
            break;
        }
        if (memcmp(buf, "BM", 2)) {
            break;
        }
        filesize = buf[5];
        filesize <<= 8;
        filesize += buf[4];
        filesize <<= 8;
        filesize+=buf[3];
        filesize <<= 8;
        filesize+=buf[2];
        bmp_offset = buf[13];
        bmp_offset <<= 8;
        bmp_offset += buf[12];
        bmp_offset <<= 8;
        bmp_offset += buf[11];
        bmp_offset <<= 8;
        bmp_offset += buf[10];
        
        //if (fReadMW(LogoTempFileHdr, buf, 40) != 40) {    /* Read bmp header */
        if (ReadBmp(buf, aBmp, 40) != 40) {    /* Read bmp header */
            break;
        }
        if (buf[14] == 1) {
            bw = 1;
        }
        size = buf[23];
        size <<= 8;
        size += buf[22];
        size <<= 8;
        size += buf[21];
        size <<= 8;
        size += buf[20];                /* Get content size */
        
        width = buf[7];
        width <<= 8;
        width += buf[6];
        width <<= 8;
        width += buf[5];
        width <<= 8;
        width += buf[4];                /* Get Image width */
        
        height = buf[11];
        height <<= 8;
        height += buf[10];
        height <<= 8;
        height += buf[9];
        height <<= 8;
        height += buf[8];               /* Get Image height */
        
        pitch=size/height;
        if (width != 0x180) {
            break;
        }
        ch = (BYTE) (width/256);
        WritePrn(aPrn, &ch, 1);
        ch = width%256;
        WritePrn(aPrn, &ch, 1);
        ch = (BYTE) (height/256);
        WritePrn(aPrn, &ch, 1);
        ch = height%256;
        WritePrn(aPrn, &ch, 1);
        /*------------------------------*/
        /* Generate output        */
        /*------------------------------*/
        
        j = 0;
        for (y=0;y<height;y++) {
            if ((y % 8) == 0) {
                len = 48*8;
                if ((height - y) < 8) {
                    len = (WORD)(height-y)*48;
                }
                if (header_only == 0) {
                    blen = pack_bin2bcd(len+2+3,4);
                    ch = blen/256;
                    WritePrn(aPrn, &ch, 1);
                    ch = blen%256;
                    WritePrn(aPrn, &ch, 1);
                    ch = 0x06;
                    WritePrn(aPrn, &ch, 1);
                    blen = pack_bin2bcd(len+2,4);
                    ch = blen/256;
                    WritePrn(aPrn, &ch, 1);
                    ch = blen%256;
                    WritePrn(aPrn, &ch, 1);
                    ch = j++;
                    WritePrn(aPrn, &ch, 1);
                    ch = half_dot;
                    WritePrn(aPrn, &ch, 1);
                }
            }
            ptr=(height-1-y)*pitch+bmp_offset;
            SeekBmp(ptr);
            if (bw) {
                for (x=0;x<width;x+=8) {
                    ReadBmp(pixel, aBmp, 1);
                    ch = 0;
                    for (k=1;k!=0x100;k<<=1) {
                        ch <<= 1;
                        if ((pixel[0] & k) == 0)
                            ch |= 1;
                    }
                    WritePrn(aPrn, &ch, 1);
                }
            } else {
                ch=0;
                for (x=0;x<width;x++) {
                    ch>>=1;
                    ReadBmp(pixel, aBmp, 3);
                    if ((pixel[0]>R_th)&&(pixel[1]>G_th)&&(pixel[2]>B_th))
                        ch|=0;
                    else
                        ch|=0x80;
                    if ((x%8)==7) {
                        WritePrn(aPrn, &ch, 1);
                    }
                }
            }
        }
        break;
    } while(0);
}
//*****************************************************************************
//  Function        : LptParamFile
//  Description     : Print BMP data from param file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN LptParamFile(BYTE aFileNo)
{
    T_FILE file_info, *pInfo;
    int fd, fsize, rlen;
    DWORD ctrl;
    DWORD status;
    BYTE prncfg[8] = { 4, 0, 0, 0, 2, 0, 0, 0 };       // printer contrast = 2
    BYTE bmp_file[64], *ptr, *prn;
    WORD width, height;
    
    if ((aFileNo < 5) && (aFileNo > 9))
        return FALSE;
    
    sprintf(bmp_file, "PARAM%d", aFileNo);
    ctrl = K_SearchPub|K_SearchReset;
    while (1) {
        pInfo = os_file_search_data(ctrl, &file_info);
        if (pInfo == NULL)
            break;
        if (memcmp(bmp_file, pInfo->s_name, 6) == 0)
            break;
        ctrl = K_SearchPub|K_SearchNext;
    }
    if (pInfo == NULL)
        return FALSE;
    
    strcpy(bmp_file, pInfo->s_owner);
    strcat(bmp_file, pInfo->s_name);
    fd = os_file_open(bmp_file, K_O_RDONLY);
    fsize = os_file_length(fd);
    if ((fd != -1) && (fsize > 0)) {
        ptr = (BYTE*) MallocMW(fsize);
        if (ptr == NULL) {
            os_file_close(fd);
            return FALSE;
        }
        rlen = os_file_read(fd, ptr, fsize);
        os_file_close(fd);
    }
    else
        return FALSE;
    
    if (rlen != fsize) {
        FreeMW(ptr);
        return FALSE;
    }
    // convert printer data from bitmap file
    prn = (BYTE*) MallocMW(fsize);
    if (prn == NULL) {
        FreeMW(ptr);
        return FALSE;
    }
    ProcessLogoFile(prn, ptr, rlen);
    
    if (!LptOpen(prncfg)) {
        DispLineMW("Lpt Open Err!", MW_LINE5, MW_CENTER|MW_BIGFONT);
        APM_WaitKey(9000,0);
        FreeMW(ptr);
        FreeMW(prn);
        return FALSE;
    }
    
    DispLineMW("PRINTING ...", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    width = prn[0]*256 + prn[1];
    height = prn[2]*256 + prn[3];
    status = PrintLogo(width, height, &prn[4]);
    if ((status & 0xFF) != MW_TMLPT_SOK) {
        DispLineMW("Lpt Err!", MW_LINE5, MW_CENTER|MW_BIGFONT);
        APM_WaitKey(9000,0);
    }
    LptClose();
    FreeMW(ptr);
    FreeMW(prn);
    return TRUE;
}
