//-----------------------------------------------------------------------------
//  File          : camtest.c
//  Module        :
//  Description   : Include Camera test.
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
//  19 Sept 2016  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#if (T300|A5T1000)
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "apm.h"
#include "hwdef.h"
#include "sysutil.h"
#include "SPGLib.h"
#include "bar2dll.h"
#include "camtest.h"
#include "hardware.h"


//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
//#define DM_TIMEOUT          2               // Try 2 sec to decode Data Matrix
#define DM_TIMEOUT          0               // Skip decode Data Matrix

typedef struct {
  WORD w_hstart;            // horizontal start position
  WORD w_vstart;            // vertical start position  
  WORD w_hwidth;            // horizontal data width    
  WORD w_vwidth;            // vertical data width      
  WORD data[K_DispMemorySize];    // graphic data array       
} T_LCDBUF;

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
static BOOLEAN gSensorOpened=FALSE;


//*****************************************************************************
//  Function        : ShowLayer
//  Description     : Display input buffer on Overlay.
//  Input           : aView;        // lcd buffer
//                    aLayId;       // Overlay ID.
//                    aExtra;       // pointer to extra parameters.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN ShowLayer(T_LCDBUF *aView, DWORD aLayId, BYTE *aExtra)
{
  DWORD control;

  control = aLayId | ((aView!=NULL)?K_DispOverlayEn:0);
  if (aView && aView->w_vwidth==K_DispHeightInPixel) {
    control |= K_DispOverlayFullScr;
  }
  os_disp_overlay(aView, control, aExtra);
  return TRUE;
}

//*****************************************************************************
//  Function        : SensorOpen
//  Description     : Open image sensor for barcode and print overlay1 area.
//  Input           : aMode;        // 
//                    aPreview;     // preview area.
//                    aFrameCfg;    // Frame config from system.
//                    aOneDHeight;  // Height of 1D barcode area indicator.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SensorOpen(DWORD aMode, T_LCDG *aPreview, T_IMAGE_FRAME *aFrameCfg, DWORD aOneDHeight)
{
#define COLOR_565(RED, GREEN, BLUE)           (((((RED) >> 3) & 0x1f)<<11) | ((((GREEN) >> 2) & 0x3f)<<5) | (((BLUE) >> 3) & 0x1f))
#define COLOR_4444(ALPHA, RED, GREEN, BLUE)   (((((ALPHA)>> 4) & 0xf)<<12) | ((((RED) >> 4) & 0xf)<<8) | ((((GREEN) >> 4) & 0xf)<<4) | (((BLUE) >> 4) & 0xf))
  int ret;
  BYTE  dispbuf[128];
  BYTE  *pOverLay;
  WORD  trans_grey  = COLOR_4444(80, 255, 0, 0);
  WORD  transparent = COLOR_4444(0, 0, 0, 0);
  DWORD control = 0;
  DWORD retry=3;

  while (retry--) {
    if ((ret = os_image_sensor_open(aMode, aPreview, aFrameCfg)) < 0) {
      if (retry > 0) {
        Delay10ms(10);
        continue;
      }
      sprintf(dispbuf, "sensor_open:[%d]", ret);
      DispLineMW(dispbuf, MW_LINE9, MW_CENTER|MW_REVERSE|MW_SPFONT);
      APM_WaitKey(KBD_TIMEOUT, 0);
      return FALSE;
    }
  }
  
  if (aPreview == NULL) {
    os_disp_overlay(NULL, K_DispOverlay1, NULL); // off
  }
  else {
    // draw overlay pattern
    // Add Indication overlay here
    pOverLay = MallocMW(sizeof(T_LCDG) + K_DispMemorySize);
    if (pOverLay != NULL) {
      memcpy(pOverLay, aPreview, sizeof(T_LCDG));
      if (aMode & SENSOR_MODE_FULL_SCREEN) {
        ((T_LCDG *)pOverLay)->w_hstart = 0;
        ((T_LCDG *)pOverLay)->w_vstart = 0;
        ((T_LCDG *)pOverLay)->w_hwidth = K_DispWidthInPixel;
        ((T_LCDG *)pOverLay)->w_vwidth = K_DispHeightInPixel;
        control = K_DispOverlayFullScr;
      }
      WORD *ptr = ((T_LCDG *)pOverLay)->p_data;
      DWORD diff     = ((T_LCDG *)pOverLay)->w_vwidth*aOneDHeight/aFrameCfg->height;
      DWORD upper    = ((T_LCDG *)pOverLay)->w_vwidth/2 - diff;
      DWORD lower    = upper + 2 * diff;
      int i, j;
      aOneDHeight/=2;
      for (i = 0; i < ((T_LCDG *)pOverLay)->w_hwidth; i++) {
        for (j = 0; j < ((T_LCDG *)pOverLay)->w_vwidth; j++) {
          if (((j>upper)&&(j<lower)))
            *ptr = trans_grey;
          else
            *ptr = transparent;
          ptr++;
        }
      }
      control |= K_DispOverlay1|K_DispOverlayARGB4444;
      ShowLayer((T_LCDBUF *)pOverLay, control, NULL);
    }
    FreeMW(pOverLay);
  }
  gSensorOpened = TRUE;
  return TRUE;
}

//*****************************************************************************
//  Function        : SensorClose
//  Description     : Close image sensor and off overlay1.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SensorClose(void)
{
  ShowLayer(NULL, K_DispOverlay1, NULL);  // off
  os_image_sensor_close();
  gSensorOpened = FALSE;
  return TRUE;
}

#ifdef os_image_sensor_set_mode
//*****************************************************************************
//  Function        : SensorSwMode
//  Description     : Switch mode of image sensor
//  Input           : aMode;        // 
//                    aPreview;     // preview area.
//                    aFrameCfg;    // Frame config from system.
//                    aOneDHeight;  // Height of 1D barcode area indicator.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN SensorSwMode(DWORD aMode, T_LCDG *aPreview, T_IMAGE_FRAME *aFrameCfg, DWORD aOneDHeight)
{
#define COLOR_565(RED, GREEN, BLUE)           (((((RED) >> 3) & 0x1f)<<11) | ((((GREEN) >> 2) & 0x3f)<<5) | (((BLUE) >> 3) & 0x1f))
#define COLOR_4444(ALPHA, RED, GREEN, BLUE)   (((((ALPHA)>> 4) & 0xf)<<12) | ((((RED) >> 4) & 0xf)<<8) | ((((GREEN) >> 4) & 0xf)<<4) | (((BLUE) >> 4) & 0xf))
  int ret;
  BYTE  dispbuf[128];
  BYTE  *pOverLay;
  WORD  trans_grey  = COLOR_4444(80, 255, 0, 0);
  WORD  transparent = COLOR_4444(0, 0, 0, 0);
  DWORD control = 0;

  if ((ret = os_image_sensor_set_mode(aMode, aPreview, aFrameCfg)) < 0) {
    sprintf(dispbuf, "sensor_setmode:[%d]", ret);
    DispLineMW(dispbuf, MW_LINE9, MW_CENTER|MW_REVERSE|MW_SPFONT);
    APM_WaitKey(KBD_TIMEOUT, 0);
    return FALSE;
  }
  
  if (aPreview == NULL) {
    os_disp_overlay(NULL, K_DispOverlay1, NULL); // off
  }
  else {
    // draw overlay pattern
    // Add Indication overlay here
    pOverLay = MallocMW(sizeof(T_LCDG) + K_DispMemorySize);
    if (pOverLay != NULL) {
      memcpy(pOverLay, aPreview, sizeof(T_LCDG));
      if (aMode & SENSOR_MODE_FULL_SCREEN) {
        ((T_LCDG *)pOverLay)->w_hstart = 0;
        ((T_LCDG *)pOverLay)->w_vstart = 0;
        ((T_LCDG *)pOverLay)->w_hwidth = K_DispWidthInPixel;
        ((T_LCDG *)pOverLay)->w_vwidth = K_DispHeightInPixel;
        control = K_DispOverlayFullScr;
      }
      WORD *ptr = ((T_LCDG *)pOverLay)->p_data;
      DWORD diff     = ((T_LCDG *)pOverLay)->w_vwidth*aOneDHeight/aFrameCfg->height;
      DWORD upper    = ((T_LCDG *)pOverLay)->w_vwidth/2 - diff;
      DWORD lower    = upper + 2 * diff;
      int i, j;
      aOneDHeight/=2;
      for (i = 0; i < ((T_LCDG *)pOverLay)->w_hwidth; i++) {
        for (j = 0; j < ((T_LCDG *)pOverLay)->w_vwidth; j++) {
          if (((j>upper)&&(j<lower)))
            *ptr = trans_grey;
          else
            *ptr = transparent;
          ptr++;
        }
      }
      control |= K_DispOverlay1|K_DispOverlayARGB4444;
      ShowLayer((T_LCDBUF *)pOverLay, control, NULL);
    }
    FreeMW(pOverLay);
  }
  return TRUE;
}
#endif
//*****************************************************************************
//  Function        : LcdOnLayerEnable
//  Description     : Enable or disable LcdOnLayer function.
//  Input           : aEnable;  // TRUE/FALSE;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
#define K_TRANS_COLOR   0xFFFF
#define K_TEXT_COLOR    0x07E0
static void LcdOnLayerEnable(BOOLEAN aEnable)
{
  if (aEnable) {
    os_disp_putc(K_PushCursor);
    os_disp_backc(K_TRANS_COLOR);
    //os_disp_textc(K_TEXT_COLOR);
    os_disp_textc(RGB(0x00,0x00,0xFF));
  }
  else {
    os_disp_putc(K_PopCursor);
  }
}

//*****************************************************************************
//  Function        : LcdOnLayerShow
//  Description     : Pop Lcd buffer to Overlay
//  Input           : aId;      // overlay id
//                    aShow;    // TRUE/FALSE => show / no show
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void LcdOnLayerShow(DWORD aId, BOOLEAN aShow)
{
  BYTE extra[OVERLAY_EXTRA_RGB_CR_KEY_HB+1];
  T_LCDBUF lcd, *pLcdBuf = NULL;

  if (aShow) {
    memset(&lcd, 0, sizeof(lcd));
    lcd.w_hwidth = 320;
    lcd.w_vwidth = 216;
    os_disp_getg(&lcd);

    extra[OVERLAY_EXTRA_PARAM_LEN]     = OVERLAY_EXTRA_RGB_CR_KEY_HB;
    extra[OVERLAY_EXTRA_RGB_CR_KEY_LB] = K_TRANS_COLOR & 0xff;   // Set Transparent color
    extra[OVERLAY_EXTRA_RGB_CR_KEY_HB] = (K_TRANS_COLOR >> 8) & 0xff;
    pLcdBuf = &lcd;
  }

  ShowLayer(pLcdBuf, aId|K_DispOverlayRGB565|K_DispOverlayCRKeyExist, extra);
}

BOOLEAN Y800toLcdLeft(T_LCDBUF *aLcd, T_IMAGE_FRAME *aCfg, BYTE *aY800, DWORD aOffset) 
{
  int  x, y, xoff, yoff;
  WORD *ptr = aLcd->data;

  aLcd->w_hwidth = aLcd->w_hwidth > aCfg->width ? aCfg->width : aLcd->w_hwidth;
  aLcd->w_vwidth = aLcd->w_vwidth > aCfg->height ? aCfg->height : aLcd->w_vwidth;
  xoff = yoff = 0;
  xoff = aOffset;
  for (x = aLcd->w_hwidth - 1; x >= 0; x--) {
    for (y = 0; y < aLcd->w_vwidth; y++) {
      WORD grey = (aY800[(y+yoff)*aCfg->width+x+xoff]/8)&0x1F;
      *ptr++ = grey << 11 | grey << 6 | grey | 0x20;
    }
  }
  
  return TRUE;
}

void ShowBarcode(BYTE *aDat, T_IMAGE_FRAME *aFrameCfg)
{
  T_LCDBUF *plcd = (T_LCDBUF *) MallocMW(sizeof(T_LCDBUF));
  memset(plcd, 0, sizeof(T_LCDBUF));
  plcd->w_hwidth = 320;
  plcd->w_vwidth = 12;
  plcd->w_hstart = 0;
  plcd->w_vstart = 160;
  Y800toLcdLeft(plcd, aFrameCfg, aDat, 0);
  os_disp_putg(plcd);

  plcd->w_hwidth = 320;
  plcd->w_vwidth = 12;
  plcd->w_hstart = 0;
  plcd->w_vstart = 180;
  Y800toLcdLeft(plcd, aFrameCfg, aDat, 320);
  os_disp_putg(plcd);

  plcd->w_hwidth = 320;
  plcd->w_vwidth = 12;
  plcd->w_hstart = 0;
  plcd->w_vstart = 200;
  Y800toLcdLeft(plcd, aFrameCfg, aDat, 640);
  os_disp_putg(plcd);

  FreeMW(plcd);
}
//*****************************************************************************
//  Function        : CamTest
//  Description     : Test Camera.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
enum {
  MODE_W960_D			 ,             // 960x720 with preview & decode
  MODE_W640_D			 ,             // 640x480 with preview & decode
  MODE_W320_D			 ,             // 320x240 with preview & decode
  MODE_W320_SD			 ,             // 320x240 with preview & decode small
  MODE_F960_D			 ,             // 960x720 with FullScreen & decode
  MODE_F640_D			 ,             // 640x480 with FullScreen & decode
  MODE_F320_D			 ,             // 320x240 with FullScreen & decode
  MODE_MAX               ,
};
void CamTest2D(void)
{
  T_IMAGE_FRAME framecfg; // output structure for driver to return image frame properties
  T_LCDG previewlcd;

  T_LCDG *preview=NULL;
  BYTE *pY800Dat = NULL;
  T_BAR_DAT  *pbar_dat  = NULL;
  T_BAROUT_DAT *pbar_out = NULL;
  #define DECODEDSTR_SIZE 256
  #define SYMTYPE_SIZE    128
  char *pdecoded_str = NULL;
  char *psym_type    = NULL;

  DWORD keyin, mode, curr_mode, last_mode, i;
  BYTE  dispbuf[512];
  BOOLEAN img_led = 0;
  DWORD oned_height = 0;
  //DWORD cap_start, cap_tick, dec_start, dec_tick;
  int zb, zx, twod, ret;

  if (!IsAppIdExistMW(BARDLL_ID)) {
    DispLineMW("BARDLL NOT LOADED!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(KBD_TIMEOUT, 0);
    return;
  }

  // Ready Default Preview config
  memset(&previewlcd, 0, sizeof(previewlcd));
  previewlcd.w_hwidth = 160; //144;  // must key the ratio
  previewlcd.w_vwidth = 120; //108;
  previewlcd.w_hstart = 0;
  previewlcd.w_vstart = 0;
  preview = &previewlcd;

  // Setup Current disp mode
  curr_mode = MODE_F320_D; 
  last_mode = MODE_MAX;
  mode      = SENSOR_MODE_320;

  DispLineMW("CamTest", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  DispLineMW("Wait For CAM Ready", MW_LINE5, MW_CENTER|MW_SPFONT);

  LcdOnLayerEnable(TRUE);

  pY800Dat = MallocMW(960*720);   // allocate max frame size; 960*720
  pbar_dat = (T_BAR_DAT  *)MallocMW(sizeof(T_BAR_DAT));
  pbar_out = (T_BAROUT_DAT *)MallocMW(sizeof(T_BAROUT_DAT));
  pdecoded_str = (char *)MallocMW(DECODEDSTR_SIZE);
  psym_type    = (char *)MallocMW(SYMTYPE_SIZE);

  zb = zx = twod = keyin = 0;
  while (keyin != MWKEY_CANCL) {
    #ifdef os_disp_bl_control
    os_disp_bl_control(256); // on now
    #endif
    switch (keyin) {
      case MWKEY_CLR:
        // toggle led on/off
        img_led = img_led == 0 ? 1 : 0;
        os_image_sensor_led_open(img_led);
        break;
      case MWKEY_ASTERISK:
        if (curr_mode == MODE_F320_D)
          curr_mode = MODE_F640_D;
        else
          curr_mode = MODE_F320_D;
        break;
      default:
        break;
    }
    if (last_mode != curr_mode) {
      switch (curr_mode) {
        case MODE_W960_D:
          mode    = SENSOR_MODE_960;
          preview = &previewlcd;
          DispLineMW("960 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, FALSE);   // disable layer 2
          break;
        case MODE_W640_D:
          mode    = SENSOR_MODE_640;
          preview = &previewlcd;
          DispLineMW("640 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, FALSE);   // disable layer 2
          break;
        case MODE_W320_D:
          mode    = SENSOR_MODE_320;
          preview = &previewlcd;
          DispLineMW("320 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, FALSE);   // disable layer 2
          break;
        case MODE_F960_D:
          mode    = SENSOR_MODE_FULL_SCREEN|SENSOR_MODE_960;
          preview = &previewlcd;
          DispLineMW("960 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          break;
        case MODE_F640_D:
          mode    = SENSOR_MODE_FULL_SCREEN|SENSOR_MODE_640;
          preview = &previewlcd;
          DispLineMW("640 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          break;
        case MODE_F320_D:
          mode    = SENSOR_MODE_FULL_SCREEN|SENSOR_MODE_320;
          preview = &previewlcd;
          DispLineMW("320 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          break;
      }
#ifdef os_image_sensor_set_mode
      if (gSensorOpened) {
        if (!SensorSwMode(mode, preview, &framecfg, oned_height)) {
          DispLineMW("SenSw failed!", MW_LINE9, MW_CLREOL|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          APM_WaitKey(KBD_TIMEOUT, 0);
          break;
        }
      }
      else {
        SensorClose();
        if (!SensorOpen(mode, preview, &framecfg, oned_height)) {
          DispLineMW("SenOpen failed!", MW_LINE9, MW_CLREOL|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          APM_WaitKey(KBD_TIMEOUT, 0);
          break;
        }
      }
#else
      SensorClose();
      if (!SensorOpen(mode, preview, &framecfg, oned_height)) {
        return;
      }
#endif
      last_mode = curr_mode;
    }

    // Get Frame if not preview only
    //cap_start = TickGet();
    for (i = 0; i < 2; i++) {  // retry max twice
      ret = os_image_sensor_get_frame(pY800Dat);
      if (ret == -58) {  // according to KK -58 => thread not ready => delay & retry once.
        Delay10ms(1);
        continue;
      }
      else 
        break;
    }
    if (ret < 0) {
      sprintf(dispbuf, "get_frame:%d", ret);
      DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
      if (curr_mode > MODE_W320_SD) {
        LcdOnLayerShow(K_DispOverlay2, TRUE);
      }
      keyin=APM_WaitKey(KBD_TIMEOUT, 0);
      DispClrBelowMW(MW_LINE7);
      continue;
    }
    while ((ret=os_image_sensor_get_frame_complete())==0) {
      if ((keyin=GetCharMW())!=0) break;
      os_sleep();
    }
    //cap_tick = TickRun10ms(cap_start);

    // Decode
    DispClrBelowMW(MW_LINE8);
    if ((pbar_dat != NULL) && (pbar_out != NULL) && (pdecoded_str != NULL) && (psym_type != NULL) ) 
    {
      memset(pdecoded_str, 0, DECODEDSTR_SIZE);
      memset(psym_type,    0, SYMTYPE_SIZE);

      // Try decode 1D barcode 1st
      pbar_dat->type   = framecfg.type;
      pbar_dat->width  = framecfg.width;
      pbar_dat->height = framecfg.height;
      pbar_dat->dat    = pY800Dat;

      // Enable follow line to try full image on 1D barcode. otherwise only mid 40 lines
      //BAR_1DTryHard(TRUE);  // Try full image

      // output parameters
      pbar_out->dat          = pdecoded_str;
      pbar_out->max_dat_len  = DECODEDSTR_SIZE;
      pbar_out->type_str     = psym_type;
      pbar_out->max_type_len = SYMTYPE_SIZE;

#if 0
      //dec_start = TickGet();
      zb = BAR_Decode1D(pbar_out, pbar_dat);
      //dec_tick  = TickRun10ms(dec_start);
      //if (zb <= 0) {
      //  enh_mode = TRUE;
      //  dec_start = TickGet();
      //  zb = BAR_Decode1DEnh(pbar_out, pbar_dat);
      //  //zb = BAR_Decode1D(pbar_out, pbar_dat);
      //  dec_tick  = TickRun10ms(dec_start);
      //}

      //sprintf(dispbuf, "ZB%d[%d][%d-%d]", framecfg.width, zb, cap_tick, dec_tick);
      //DispLine(dispbuf, SU_LINE6, SU_CLREOL|SU_SPFONT);
      if (zb > 0) {
        //sprintf(dispbuf, "%s", enh_mode?"Enhanced":"Normal");
        //DispLine(dispbuf, SU_LINE6, SU_RIGHT|SU_SPFONT);
        //sprintf(dispbuf, "[%s:%s]", psym_type, pdecoded_str);
        sprintf(dispbuf, "[%s]", pdecoded_str);
        DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
      }

      //dec_start = TickGet();
      //zx = onedreader_decode(pbar_out->dat, pbar_dat->dat, pbar_dat->width, pbar_dat->height, pbar_out->max_dat_len);
      zx = BAR_ZXDecode1D(pbar_out, pbar_dat);
      //dec_tick  = TickRun10ms(dec_start);
      //if (zx <= 0) {
      //  enh_mode = TRUE;
      //  dec_start = TickGet();
      //  zb = BAR_ZXDecode1DEnh(pbar_out, pbar_dat);
      //  //zb = BAR_ZXDecode1D(pbar_out, pbar_dat);
      //  dec_tick  = TickRun10ms(dec_start);
      //}
      //sprintf(dispbuf, "ZX%d[%d]", framecfg.width, zx);
      //DispLineMW(dispbuf, MW_LINE8, MW_CLREOL|MW_SPFONT);
      if (zx > 0) {
        //sprintf(dispbuf, "%s", enh_mode?"Enhanced":"Normal");
        //DispLineMW(dispbuf, MW_LINE8, MW_RIGHT|MW_SPFONT);
        //sprintf(dispbuf, "[%s:%s]", psym_type, pdecoded_str);
        sprintf(dispbuf, "[%s]", pdecoded_str);
        DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
      }
#endif
       // Temperary Disable 2D
       if ((zb<=0)&&(zx<=0)) {
         pbar_dat->height = framecfg.height;
         pbar_dat->dat    = pY800Dat;
         //dec_start = TickGet();
         twod = BAR_Decode2D(pbar_out, pbar_dat, DM_TIMEOUT);
         //ret = BAR_Decode2D(pbar_out, pbar_dat, 0);    // not support data matrix
         //dec_tick  = TickRun10ms(dec_start);
         //sprintf(dispbuf, "2D%d[%d-%d]", framecfg.width, cap_tick, dec_tick);
         //sprintf(dispbuf, "2D%d", framecfg.width);
         //DispLineMW(dispbuf, MW_LINE7, MW_SPFONT);
         if (twod > 0) {
           //sprintf(dispbuf, "[%s:%s]", psym_type, pdecoded_str);
           sprintf(dispbuf, "[%s]", pdecoded_str);
           DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
         }
       } 
    }
    if ((zb > 0) || (zx > 0) || (twod > 0)) {
      Short2Beep();
      break;
    }

    // extract Image and display
    if (curr_mode <= MODE_W320_SD) {
      //T_LCDBUF *plcd = (T_LCDBUF *) MallocMW(sizeof(T_LCDBUF));
      //memset(plcd, 0, sizeof(T_LCDBUF));
      //plcd->w_hwidth = 160;
      //plcd->w_vwidth = 120;
      //plcd->w_hstart = 160;
      //plcd->w_vstart = 0;
      //Y800toLcd(plcd, &framecfg, pY800Dat);
      //os_disp_putg(plcd);
      //FreeMW(plcd);
    }
    else {   // update layer2 for full screen mode only
      LcdOnLayerShow(K_DispOverlay2, TRUE);
    }

    keyin = GetCharMW();
  }  // while

  if (pbar_dat  != NULL)    FreeMW(pbar_dat);
  if (pbar_out != NULL)     FreeMW(pbar_out);
  if (pdecoded_str != NULL) FreeMW(pdecoded_str);
  if (psym_type!= NULL)     FreeMW(psym_type);
  if (pY800Dat != NULL)     FreeMW(pY800Dat);

  // Close image sensor
  SensorClose();
  if ((zb > 0) || (zx > 0) || (twod > 0)) {
    DispLineMW("Scan Success!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    APM_WaitKey(500, 0);
  }
  LcdOnLayerShow(K_DispOverlay2, FALSE);
  LcdOnLayerEnable(FALSE);
}
void CamTestAll(void)
{
  T_IMAGE_FRAME framecfg; // output structure for driver to return image frame properties
  T_LCDG previewlcd;

  T_LCDG *preview=NULL;
  BYTE *pY800Dat = NULL;
  T_BAR_DAT  *pbar_dat  = NULL;
  T_BAROUT_DAT *pbar_out = NULL;
  #define DECODEDSTR_SIZE 256
  #define SYMTYPE_SIZE    128
  char *pdecoded_str = NULL;
  char *psym_type    = NULL;

  DWORD keyin, timeout, mode, curr_mode, last_mode, i;
  BYTE  dispbuf[512];
  BOOLEAN img_led = 0;
  DWORD oned_height = 5;
  //DWORD cap_start, cap_tick, dec_start, dec_tick;
  int zb, zx, twod, ret;
  DWORD free_run;

#ifdef K_XHdImageSensor
  if ((os_hd_config_extend() & K_XHdImageSensor) == K_XHdNoImageSensor)
    return;
#elif K_HdCamera
  if ((os_hd_config() & K_HdCamera) == K_HdNoCamera)
    return;
#endif
    
  if (!IsAppIdExistMW(BARDLL_ID)) {
    DispLineMW("BARDLL NOT LOADED!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(KBD_TIMEOUT, 0);
    return;
  }

  // Ready Default Preview config
  memset(&previewlcd, 0, sizeof(previewlcd));
  previewlcd.w_hwidth = 160; //144;  // must key the ratio
  previewlcd.w_vwidth = 120; //108;
  previewlcd.w_hstart = 0;
  previewlcd.w_vstart = 0;
  preview = &previewlcd;

  // Setup Current disp mode
  //curr_mode = MODE_F640_D; 
  curr_mode = MODE_F320_D; 
  last_mode = MODE_MAX;
  mode      = SENSOR_MODE_640;

  DispLineMW("CamTest", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_SPFONT);
  DispLineMW("Wait For CAM Ready", MW_LINE5, MW_CENTER|MW_SPFONT);

  LcdOnLayerEnable(TRUE);

  pY800Dat = MallocMW(960*720);   // allocate max frame size; 960*720
  pbar_dat = (T_BAR_DAT  *)MallocMW(sizeof(T_BAR_DAT));
  pbar_out = (T_BAROUT_DAT *)MallocMW(sizeof(T_BAROUT_DAT));
  pdecoded_str = (char *)MallocMW(DECODEDSTR_SIZE);
  psym_type    = (char *)MallocMW(SYMTYPE_SIZE);

  zb = zx = twod = keyin = 0;
  timeout = 1*TIME_1SEC;
  while (keyin != MWKEY_CANCL) {
    #ifdef os_disp_bl_control
    os_disp_bl_control(256); // on now
    #endif
    switch (keyin) {
      case MWKEY_CLR:
        // toggle led on/off
        img_led = img_led == 0 ? 1 : 0;
        os_image_sensor_led_open(img_led);
        break;
      case MWKEY_ASTERISK:
        if (curr_mode == MODE_F320_D)
          curr_mode = MODE_F640_D;
        else
          curr_mode = MODE_F320_D;
        break;
      default:
        break;
    }
    if (last_mode != curr_mode) {
      switch (curr_mode) {
        case MODE_W960_D:
          mode    = SENSOR_MODE_960;
          preview = &previewlcd;
          DispLineMW("960 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, FALSE);   // disable layer 2
          break;
        case MODE_W640_D:
          mode    = SENSOR_MODE_640;
          preview = &previewlcd;
          DispLineMW("640 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, FALSE);   // disable layer 2
          break;
        case MODE_W320_D:
          mode    = SENSOR_MODE_320;
          preview = &previewlcd;
          DispLineMW("320 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, FALSE);   // disable layer 2
          break;
        case MODE_F960_D:
          mode    = SENSOR_MODE_FULL_SCREEN|SENSOR_MODE_960;
          preview = &previewlcd;
          DispLineMW("960 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          break;
        case MODE_F640_D:
          mode    = SENSOR_MODE_FULL_SCREEN|SENSOR_MODE_640;
          preview = &previewlcd;
          DispLineMW("640 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          break;
        case MODE_F320_D:
          mode    = SENSOR_MODE_FULL_SCREEN|SENSOR_MODE_320;
          preview = &previewlcd;
          DispLineMW("320 Decode", MW_LINE9, MW_CLRDISP|MW_CENTER|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          break;
      }
#ifdef os_image_sensor_set_mode
      if (gSensorOpened) {
        if (!SensorSwMode(mode, preview, &framecfg, oned_height)) {
          DispLineMW("SenSw failed!", MW_LINE9, MW_CLREOL|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          APM_WaitKey(KBD_TIMEOUT, 0);
          break;
        }
      }
      else {
        SensorClose();
        if (!SensorOpen(mode, preview, &framecfg, oned_height)) {
          DispLineMW("SenOpen failed!", MW_LINE9, MW_CLREOL|MW_SPFONT);
          LcdOnLayerShow(K_DispOverlay2, TRUE);
          APM_WaitKey(KBD_TIMEOUT, 0);
          break;
        }
      }
#else
      SensorClose();
      if (!SensorOpen(mode, preview, &framecfg, oned_height)) {
        return;
      }
#endif
      last_mode = curr_mode;
    }

    // Get Frame if not preview only
    //cap_start = TickGet();
    for (i = 0; i < 2; i++) {  // retry max twice
      ret = os_image_sensor_get_frame(pY800Dat);
      if (ret == -58) {  // according to KK -58 => thread not ready => delay & retry once.
        Delay10ms(1);
        continue;
      }
      else 
        break;
    }
    if (ret < 0) {
      sprintf(dispbuf, "get_frame:%d", ret);
      DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
      if (curr_mode > MODE_W320_SD) {
        LcdOnLayerShow(K_DispOverlay2, TRUE);
      }
      keyin=APM_WaitKey(KBD_TIMEOUT, 0);
      DispClrBelowMW(MW_LINE7);
      continue;
    }
    while ((ret=os_image_sensor_get_frame_complete())==0) {
      if ((keyin=GetCharMW())!=0) break;
      os_sleep();
    }
    //cap_tick = TickRun10ms(cap_start);

    // Decode
    DispClrBelowMW(MW_LINE8);
    if ((pbar_dat != NULL) && (pbar_out != NULL) && (pdecoded_str != NULL) && (psym_type != NULL) ) 
    {
      memset(pdecoded_str, 0, DECODEDSTR_SIZE);
      memset(psym_type,    0, SYMTYPE_SIZE);

      // Try decode 1D barcode 1st
      pbar_dat->type   = framecfg.type;
      pbar_dat->width  = framecfg.width;
      pbar_dat->height = framecfg.height;
      pbar_dat->dat    = pY800Dat;

      // Enable follow line to try full image on 1D barcode. otherwise only mid 40 lines
      //BAR_1DTryHard(TRUE);  // Try full image

      // output parameters
      pbar_out->dat          = pdecoded_str;
      pbar_out->max_dat_len  = DECODEDSTR_SIZE;
      pbar_out->type_str     = psym_type;
      pbar_out->max_type_len = SYMTYPE_SIZE;

      //dec_start = TickGet();
      zb = BAR_Decode1D(pbar_out, pbar_dat);
      //dec_tick  = TickRun10ms(dec_start);
      //if (zb <= 0) {
      //  enh_mode = TRUE;
      //  dec_start = TickGet();
      //  zb = BAR_Decode1DEnh(pbar_out, pbar_dat);
      //  //zb = BAR_Decode1D(pbar_out, pbar_dat);
      //  dec_tick  = TickRun10ms(dec_start);
      //}

      //sprintf(dispbuf, "ZB%d[%d][%d-%d]", framecfg.width, zb, cap_tick, dec_tick);
      //DispLine(dispbuf, SU_LINE6, SU_CLREOL|SU_SPFONT);
      if (zb > 0) {
        //sprintf(dispbuf, "%s", enh_mode?"Enhanced":"Normal");
        //DispLine(dispbuf, SU_LINE6, SU_RIGHT|SU_SPFONT);
        //sprintf(dispbuf, "[%s:%s]", psym_type, pdecoded_str);
        sprintf(dispbuf, "[%s]", pdecoded_str);
        DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
      }

      //dec_start = TickGet();
      //zx = onedreader_decode(pbar_out->dat, pbar_dat->dat, pbar_dat->width, pbar_dat->height, pbar_out->max_dat_len);
      zx = BAR_ZXDecode1D(pbar_out, pbar_dat);
      //dec_tick  = TickRun10ms(dec_start);
      //if (zx <= 0) {
      //  enh_mode = TRUE;
      //  dec_start = TickGet();
      //  zb = BAR_ZXDecode1DEnh(pbar_out, pbar_dat);
      //  //zb = BAR_ZXDecode1D(pbar_out, pbar_dat);
      //  dec_tick  = TickRun10ms(dec_start);
      //}
      //sprintf(dispbuf, "ZX%d[%d]", framecfg.width, zx);
      //DispLineMW(dispbuf, MW_LINE8, MW_CLREOL|MW_SPFONT);
      if (zx > 0) {
        //sprintf(dispbuf, "%s", enh_mode?"Enhanced":"Normal");
        //DispLineMW(dispbuf, MW_LINE8, MW_RIGHT|MW_SPFONT);
        //sprintf(dispbuf, "[%s:%s]", psym_type, pdecoded_str);
        sprintf(dispbuf, "[%s]", pdecoded_str);
        DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
      }

       // Temperary Disable 2D
       if ((zb<=0)&&(zx<=0)) {
         pbar_dat->height = framecfg.height;
         pbar_dat->dat    = pY800Dat;
         //dec_start = TickGet();
         twod = BAR_Decode2D(pbar_out, pbar_dat, DM_TIMEOUT);
         //ret = BAR_Decode2D(pbar_out, pbar_dat, 0);    // not support data matrix
         //dec_tick  = TickRun10ms(dec_start);
         //sprintf(dispbuf, "2D%d[%d-%d]", framecfg.width, cap_tick, dec_tick);
         //sprintf(dispbuf, "2D%d", framecfg.width);
         //DispLineMW(dispbuf, MW_LINE7, MW_SPFONT);
         if (twod > 0) {
           //sprintf(dispbuf, "[%s:%s]", psym_type, pdecoded_str);
           sprintf(dispbuf, "[%s]", pdecoded_str);
           DispLineMW(dispbuf, MW_LINE9, MW_CLREOL|MW_SPFONT);
         }
       } 
    }
    if ((zb > 0) || (zx > 0) || (twod > 0)) {
      Short2Beep();
    }

    // extract Image and display
    if (curr_mode <= MODE_W320_SD) {
      //T_LCDBUF *plcd = (T_LCDBUF *) MallocMW(sizeof(T_LCDBUF));
      //memset(plcd, 0, sizeof(T_LCDBUF));
      //plcd->w_hwidth = 160;
      //plcd->w_vwidth = 120;
      //plcd->w_hstart = 160;
      //plcd->w_vstart = 0;
      //Y800toLcd(plcd, &framecfg, pY800Dat);
      //os_disp_putg(plcd);
      //FreeMW(plcd);
    }
    else {   // update layer2 for full screen mode only
      LcdOnLayerShow(K_DispOverlay2, TRUE);
    }

    if ((zb > 0) || (zx > 0) || (twod > 0)) {
//      LcdOnLayerShow(K_DispOverlay2, FALSE);
//      LcdOnLayerEnable(FALSE);
//      DispLineMW("Scan Success!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
//      APM_WaitKey(500, 0);
//      //DispClrBelowMW(MW_LINE1);
//      LcdOnLayerShow(K_DispOverlay2, TRUE);
//      LcdOnLayerEnable(TRUE);
      timeout = 1*TIME_1SEC;
    }
    else 
      timeout = 0;

#if 1
    free_run = FreeRunMW();
    while (FreeRunMW() < free_run + timeout) {
      if ((keyin = GetCharMW()) != 0)
        break;
      SleepMW();
    }
#else
    keyin = GetCharMW();
#endif
  }  // while

  if (pbar_dat  != NULL)    FreeMW(pbar_dat);
  if (pbar_out != NULL)     FreeMW(pbar_out);
  if (pdecoded_str != NULL) FreeMW(pdecoded_str);
  if (psym_type!= NULL)     FreeMW(psym_type);
  if (pY800Dat != NULL)     FreeMW(pY800Dat);

  // Close image sensor
  SensorClose();
  LcdOnLayerShow(K_DispOverlay2, FALSE);
  LcdOnLayerEnable(FALSE);
}
#endif
