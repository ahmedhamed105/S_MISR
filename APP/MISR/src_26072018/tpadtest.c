//-----------------------------------------------------------------------------
//  File          : TPadTest.c
//  Module        :
//  Description   : Include Touch Pad test routines.
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
//  13 Dec  2012  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "hwdef.h"
#include "sysutil.h"
#if (T1000|A5T1000|T300)
#include "apm.h"
#elif (T810)
#include "apm_lite.h"
#endif
#include "tpadtest.h"
#include "system.h"

// #if (TPAD_SUPPORT)    
#include "SPGLib.h"

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------
static void TouchDetected0(void);
static void TouchDetected1(void);
static void TouchDetected2(void);

#if(T1000|PR500)
static void SamUp(void);
static void SamDn(void);
static void SenUp(void);
static void SenDn(void);
#endif
static void DotUp(void);
static void DotDn(void);

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define TOUCHED(a)      (a&K_TpadTouched)
#define TOUCHED_DATA(a) (a&K_TpadNewData)

#define TOUCHED_X(a)  ((a&0x7FFF0000)>>16)
#define TOUCHED_Y(a)  (a&0x00007FFF)

#define TPAD_MAX_DOTSIZE    10
#define TPAD_MIN_DOTSIZE    1

#define RGB(r,g,b)      (((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>3) | ((r&g&b&4)<<3))
#define RGB_WHITE       RGB(255,255,255)
#define RGB_BACKGR      RGB(0xB0, 0xC1, 0xF0) 

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
const char *KTPadTest = { "   TouchPad Test   " };
const char *KSPadTest = { "  Signature Test   " };
#if (PR500)
const char *KSPadConf1 = { "SAM:(1:+)***(2:-)"};
const char *KSPadConf2 = { "SEN:(3:+)***(4:-)"};
const char *KSPadConf3 = { "DOT:(5:+)***(6:-)"};
#else
const char *KSPadConf = { "SAM:+***- SEN:+***- D:+**-"};
#endif

struct PADOBJ_INFO {
  WORD w_hstart;
  WORD w_vstart;
  WORD w_hend  ;
  WORD w_vend  ;
  BYTE b_red   ;
  BYTE b_green ;
  BYTE b_blue  ;
  FN   dw_func;
};
#if (PR500)
const struct PADOBJ_INFO KPadObj[] = { {  12,  12, 47, 47,0xFF,0xFF,0xFF, TouchDetected0},
                                       { 101,  45,137, 81,0xFF,0xFF,0xFF, TouchDetected1},
                                       { 192,  80,227,115,0xFF,0xFF,0xFF, TouchDetected2},
                                     };
#else
const struct PADOBJ_INFO KPadObj[] = { { 50, 80,194,239,0xFF,0x00,0x00, TouchDetected0},
                                       { 85,130,235,190,0x00,0xFF,0x00, TouchDetected1},
                                       {125, 80,270,240,0x00,0x00,0xFF, TouchDetected2}
                                     }; 
#endif
#define MAX_TPAD_OBJ  (sizeof(KPadObj)/sizeof(struct PADOBJ_INFO))

#if(T1000|PR500)
#if (PR500)
const struct PADOBJ_INFO KPadConf[] = { {  0,  0,  7,  7,0x00,0x00,0x00, SamUp},
                                        {231,  0,239,  7,0x00,0x00,0x00, SamDn},
                                        {  0, 59,  7, 67,0x00,0x00,0x00, SenUp},
                                        {231, 59,239, 67,0x00,0x00,0x00, SenDn},
                                        {  0,119,  7,127,0x00,0x00,0x00, DotUp},
                                        {231,119,239,127,0x00,0x00,0x00, DotDn},
                                      }; 
#else
const struct PADOBJ_INFO KPadConf[] = { { 48, 24, 60, 32,0x00,0x00,0x00, SamUp},
                                        { 96, 24,108, 32,0x00,0x00,0x00, SamDn},
                                        {168, 24,180, 32,0x00,0x00,0x00, SenUp},
                                        {216, 24,228, 32,0x00,0x00,0x00, SenDn},
                                        {264, 24,276, 32,0x00,0x00,0x00, DotUp},
                                        {300, 24,312, 32,0x00,0x00,0x00, DotDn},
                                      }; 
#endif
#else
const struct PADOBJ_INFO KPadConf[] = { {264, 24,276, 32,0x00,0x00,0x00, DotUp},
                                        {300, 24,312, 32,0x00,0x00,0x00, DotDn},
                                      }; 
#endif

#define MAX_TPAD_CONF (sizeof(struct PADOBJ_INFO)/sizeof(struct PADOBJ_INFO))
// #define MAX_TPAD_CONF (sizeof(KPadConf)/sizeof(struct PADOBJ_INFO))

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
static const struct PADOBJ_INFO *gCurrObj = NULL;
static DWORD  gDotSize=5;
static DWORD  gTPadObjId[MAX_TPAD_OBJ];
static DWORD  gTPadConfId[MAX_TPAD_CONF];
static DWORD  gLoss, gTouch;
struct POINT  gOld_pt={0xffffffff, 0xffffffff};
#if (PR500)
static DWORD  gEventKey = K_KeyNull;
enum
{
  K_SAMUP_KEY = MW_KEY1,
  K_SAMDN_KEY = MW_KEY2,
  K_SENUP_KEY = MW_KEY3,
  K_SENDN_KEY = MW_KEY4,
};
#endif

//*****************************************************************************
//  Function        : UpdTPadSample
//  Description     : Update TPad Sampling conf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
#if (T1000|PR500)
static DWORD UpdTPadSample(void)
{
  BYTE tmp[128];
  DWORD value = os_tpad_sampling(0);
  
  #if (PR500)
  sprintf(tmp, "%03d", value);
  DispLineMW(tmp, MW_LINE1+9, MW_SPFONT);
  #else
  sprintf(tmp, "\x1C+\x1D%03d\x1C-\x1D", value);
  DispLineMW(tmp, MW_LINE1+4, MW_SPFONT);
  #endif
  return value;
}
//*****************************************************************************
//  Function        : UpdTPadSensitivity
//  Description     : Update TPad Sensitivity conf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD UpdTPadSensitivity(void)
{
  BYTE tmp[128];
  DWORD value = os_tpad_sensitivity(0);
  
  #if (PR500)
  sprintf(tmp, "%03d", value);
  DispLineMW(tmp, MW_LINE2+9, MW_SPFONT);
  #else
  sprintf(tmp, "\x1C+\x1D%03d\x1C-\x1D", value);
  DispLineMW(tmp, MW_LINE1+14, MW_SPFONT);
  #endif
  return value;
}
#endif

//*****************************************************************************
//  Function        : UpdTPadDotSize
//  Description     : Update TPad Dot Size
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD UpdTPadDotSize(void)
{
  BYTE tmp[128];
  
  #if (PR500)
  sprintf(tmp, "%03d", gDotSize);
  DispLineMW(tmp, MW_LINE3+9, MW_SPFONT);
  #else
  sprintf(tmp, "\x1C+\x1D%02d\x1C-\x1D", gDotSize);
  DispLineMW(tmp, MW_LINE1+22, MW_SPFONT);
  #endif
  return gDotSize;
}

#if (T1000|PR500)
static void SamUpDn(BOOLEAN a_up)
{
  DWORD      value;

  value = os_tpad_sampling(0);
  if( a_up )
    value += (value < K_TpadSampleMax) ? 1 : 0;
  else
    value -= (value > K_TpadSampleMin) ? 1 : 0;
  os_tpad_sampling(value);

  UpdTPadSample();
}
static void SamUp(void)
{
#if (PR500)
  gEventKey = K_SAMUP_KEY;
#else
  SamUpDn(TRUE);
#endif
  os_tpad_alert_return();  // must call before leave.
}
static void SamDn(void)
{
#if (PR500)
  gEventKey = K_SAMDN_KEY;
#else
  SamUpDn(FALSE);
#endif
  os_tpad_alert_return();  // must call before leave.
}
static void SenUpDn(BOOLEAN a_up)
{
  DWORD      value;

  value = os_tpad_sensitivity(0);
  if( a_up )
    value += (value < K_TpadSensitivityMax) ? 1 : 0;
  else
    value -= (value > K_TpadSensitivityMin) ? 1 : 0;
  os_tpad_sensitivity(value);
  UpdTPadSensitivity();

}
static void SenUp(void)
{
#if (PR500)
  gEventKey = K_SENUP_KEY;
#else
  SenUpDn(TRUE);
#endif
  os_tpad_alert_return();  // must call before leave.
}
static void SenDn(void)
{
#if (PR500)
  gEventKey = K_SENDN_KEY;
#else
  SenUpDn(FALSE);
#endif
  os_tpad_alert_return();  // must call before leave.
}
#endif
static void DotUpDn(BOOLEAN a_up)
{
  if( a_up )
    gDotSize += (gDotSize < TPAD_MAX_DOTSIZE) ? 1 : 0;
  else
    gDotSize -= (gDotSize > TPAD_MIN_DOTSIZE) ? 1 : 0;
  GD_SetDotSize(gDotSize);
  UpdTPadDotSize();
}
static void DotUp(void)
{
  DotUpDn(TRUE);
  os_tpad_alert_return();  // must call before leave.
}
static void DotDn(void)
{
  DotUpDn(FALSE);
  os_tpad_alert_return();  // must call before leave.
}
//*****************************************************************************
//  Function        : DispPadObj
//  Description     : Display Pad Object.
//  Input           : aPadObj;      // pointer to struct PADOBJ_INFO
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
#if(T1000|PR500)
static void DispTPadConfObj(const struct PADOBJ_INFO *aPadObj)
{
  #if (PR500)
  T_LCDG *p_lcdg = (T_LCDG *)os_malloc(8+4);
  if( p_lcdg )
  {
    p_lcdg->b_hstart = aPadObj->w_hstart;
    p_lcdg->b_vstart = aPadObj->w_vstart;
    p_lcdg->b_hwidth = 8;
    p_lcdg->b_vwidth = 8;
    memset(p_lcdg->p_data, 0xff, 8);

    os_tpad_disp_putg((BYTE*)p_lcdg);
    os_free(p_lcdg);
  }
  #endif
}  

//*****************************************************************************
//  Function        : InitTPadConf
//  Description     : Init TPad Config items.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void InitTPadConf(void)
{
  T_TPAD_OBJ tpad_obj;
  DWORD      i;

  memset(gTPadConfId, 0, sizeof(gTPadConfId));
  for (i = 0; i < MAX_TPAD_CONF; i++) {
    memset(&tpad_obj, 0, sizeof(tpad_obj));
    tpad_obj.w_hstart     = KPadConf[i].w_hstart;
    tpad_obj.w_vstart     = KPadConf[i].w_vstart;
    tpad_obj.w_hend       = KPadConf[i].w_hend;
    tpad_obj.w_vend       = KPadConf[i].w_vend;
    tpad_obj.p_pen_detect = KPadConf[i].dw_func;
    tpad_obj.p_pen_loss   = NULL;
    gTPadConfId[i] = os_tpad_add_obj(&tpad_obj);
    DispTPadConfObj(&KPadConf[i]);
 }
}
#endif
//*****************************************************************************
//  Function        : CleanTPadConf
//  Description     : Cleanup TPad Config items.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void CleanTPadConf(void)
{
  DWORD i;

  for (i = 0; i < MAX_TPAD_CONF; i++) {
    os_tpad_del_obj(gTPadConfId[i]);
  }
}

//*****************************************************************************
//  Function        : IconPutS
//  Description     : Display string on the icon bar.
//  Input           : aStr;     // pointer to char string.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void IconPutS(const char *aStr, DWORD aColor)
{
  #if (PR500)
  DispLineMW(aStr, MW_LINE7, MW_SPFONT);
  #endif

  #ifdef os_disp_icon_putc
  os_disp_icon_textc(aColor);
  os_disp_icon_putc(K_ClrHome);
  os_disp_icon_putc(K_SelSpFont);
  while (*aStr != 0) {
    os_disp_icon_putc(*aStr++);
  }
  #endif //os_disp_icon_putc
}

////*****************************************************************************
////  Function        : ShowTouchPos
////  Description     : Show Touch Position.
////  Input           : N/A
////  Return          : N/A
////  Note            : N/A
////  Globals Changed : N/A
////*****************************************************************************
//static void ShowTouchPos(const struct PADOBJ_INFO *aPadObj)
//{
//  BYTE  tmp[128];
//  DWORD touch_pos = os_tpad_cpos();
//
//  if (TOUCHED_DATA(touch_pos)) {
//    sprintf(tmp, "  Touch:(%03d,%03d)  ", TOUCHED_X(touch_pos), TOUCHED_Y(touch_pos));
//    IconPutS(tmp, RGB(aPadObj->b_red, aPadObj->b_green, aPadObj->b_blue));
//  }
//}

static void TouchDetected0(void)
{
  BYTE  tmp[128];
  //ShowTouchPos(&KPadObj[0]);
  gCurrObj = &KPadObj[0];
  gTouch++;
  sprintf(tmp, "    T0:(%03d,%03d)    ", gLoss, gTouch);
  IconPutS(tmp, RGB(gCurrObj->b_red, gCurrObj->b_green, gCurrObj->b_blue));
  os_tpad_alert_return();  // must call before leave.
}
static void TouchDetected1(void)
{
  BYTE  tmp[128];
  //ShowTouchPos(&KPadObj[1]);
  gCurrObj = &KPadObj[1];
  gTouch++;
  sprintf(tmp, "    T1:(%03d,%03d)    ", gLoss, gTouch);
  IconPutS(tmp, RGB(gCurrObj->b_red, gCurrObj->b_green, gCurrObj->b_blue));
  os_tpad_alert_return();  // must call before leave.
}
static void TouchDetected2(void)
{
  BYTE  tmp[128];
  //ShowTouchPos(&KPadObj[2]);
  gCurrObj = &KPadObj[2];
  gTouch++;
  sprintf(tmp, "    T2:(%03d,%03d)    ", gLoss, gTouch);
  IconPutS(tmp, RGB(gCurrObj->b_red, gCurrObj->b_green, gCurrObj->b_blue));
  os_tpad_alert_return();  // must call before leave.
}

//*****************************************************************************
//  Function        : TouchLoss
//  Description     : Touch loss callback function for Touch Pad.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void TouchLoss(void)
{
  BYTE  tmp[128];
  //DWORD touch_pos = os_tpad_peek_cpos();

  gLoss++;
  gCurrObj = NULL;
  //sprintf(tmp, "    L:(%03d,%03d)    ", TOUCHED_X(touch_pos), TOUCHED_Y(touch_pos));
  sprintf(tmp, "    L:(%03d,%03d)    ", gLoss, gTouch);
  IconPutS(tmp, RGB(0x00,0x00,0x00));
  os_tpad_alert_return();  // must call before leave.
}

//*****************************************************************************
//  Function        : DispPadObj
//  Description     : Display Pad Object.
//  Input           : aPadObj;      // pointer to struct PADOBJ_INFO
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispPadObj(const struct PADOBJ_INFO *aPadObj)
{
  WORD   i, max_line;
  struct POINT pt1, pt2;

  pt1.dwPosX = aPadObj->w_hstart;
  pt1.dwPosY = aPadObj->w_vstart-K_DispVerticalOffset;
  pt2.dwPosX = aPadObj->w_hend;
  pt2.dwPosY = pt1.dwPosY;

  max_line = aPadObj->w_vend-aPadObj->w_vstart;
  os_disp_putc(K_PushCursor);
  GD_SetDotColor(aPadObj->b_red, aPadObj->b_green, aPadObj->b_blue);
  for (i = 0; i < max_line; i++) {
    GD_Line(&pt1, &pt2);
    pt1.dwPosY++;
    pt2.dwPosY++;
  }
  os_disp_putc(K_PopCursor);
}

//*****************************************************************************
//  Function        : InitTPadObj
//  Description     : Init TPad Objects.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void InitTPadObj(void)
{
  T_TPAD_OBJ tpad_obj;
  DWORD      i;

  memset(gTPadObjId, 0, sizeof(gTPadObjId));
  for (i = 0; i < MAX_TPAD_OBJ; i++) {
    memset(&tpad_obj, 0, sizeof(tpad_obj));
    tpad_obj.w_hstart     = KPadObj[i].w_hstart;
    tpad_obj.w_vstart     = KPadObj[i].w_vstart;
    tpad_obj.w_hend       = KPadObj[i].w_hend;
    tpad_obj.w_vend       = KPadObj[i].w_vend;
    tpad_obj.p_pen_detect = KPadObj[i].dw_func;
    tpad_obj.p_pen_loss   = TouchLoss;
    gTPadObjId[i] = os_tpad_add_obj(&tpad_obj);
    DispPadObj(&KPadObj[i]);
 }
}

//*****************************************************************************
//  Function        : CleanTPadObj
//  Description     : Cleanup TPad Objects.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void CleanTPadObj(void)
{
  DWORD i;

  for (i = 0; i < MAX_TPAD_OBJ; i++) {
    os_tpad_del_obj(gTPadObjId[i]);
  }
}


//*****************************************************************************
//  Function        : ShowTouchPoint
//  Description     : Show Touch on Lcd
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ShowTouchPoint(void)
{
  struct POINT pt;
  DWORD touch_pos;

  GD_SetDotSize(gDotSize);
  do
  {
    touch_pos = os_tpad_cpos();
    if (TOUCHED(touch_pos)&&TOUCHED_DATA(touch_pos)) {
      pt.dwPosX = TOUCHED_X(touch_pos);
      pt.dwPosY = TOUCHED_Y(touch_pos)-K_DispVerticalOffset;
      if (pt.dwPosY > K_DispVerticalOffset) {
        if( pt.dwPosX==0xffffffff && pt.dwPosY==0xffffffff )
          GD_Point(&pt);
        else if(gOld_pt.dwPosX!=pt.dwPosX || gOld_pt.dwPosY!=pt.dwPosY)
#if (PR500)
          GD_Line(&gOld_pt, &pt);
#else
//          GD_Point(&pt);
          GD_Line(&gOld_pt, &pt);
#endif
        gOld_pt.dwPosX = pt.dwPosX ;
        gOld_pt.dwPosY = pt.dwPosY ;
      }
    }
  }while(TOUCHED(touch_pos));
  gOld_pt.dwPosX = 0xffffffff ;
  gOld_pt.dwPosY = 0xffffffff ;
}


//*****************************************************************************
//  Function        : TPadTest
//  Description     : Test Touch Pad.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void TPadTest(void)
{
  #if (TPAD_SUPPORT)    
  #if(T1000|PR500)
  DWORD      sample, sensitive;
  #endif
  DWORD      keyin;
  BYTE       lcdbanner_sts;
  int        obj_cnt;
  BOOLEAN    add_obj;

  lcdbanner_sts = TRUE;
  #if !(PR500)
  os_config_write(K_CF_LcdBannerEnable, lcdbanner_sts);
  os_config_update();
  #endif
  IconPutS(KTPadTest, RGB(0,0,0));

  DispCtrlMW(MW_CLR_DISP);
  #if (PR500)
  DispLineMW(KSPadConf1, MW_LINE1, MW_SPFONT);
  DispLineMW(KSPadConf2, MW_LINE2, MW_SPFONT);
  DispLineMW(KSPadConf3, MW_LINE3, MW_SPFONT);
  ClearLCD();
  #else
  DispLineMW(KSPadConf, MW_LINE1, MW_SPFONT);
  #endif
  #if (T1000)
  sample    = UpdTPadSample();
  #endif
  #if(T1000|PR500)
  sensitive = UpdTPadSensitivity();
  #endif
  UpdTPadDotSize();
  obj_cnt = 1;
  add_obj = TRUE;
  gLoss = gTouch = 0;

  #if(T1000|PR500)  
  InitTPadConf();
  #endif
  while (TRUE) {
    if (add_obj) {
      InitTPadObj();
      add_obj = FALSE;
    }
    //if (gCurrObj != NULL) {
    //  ShowTouchPos(gCurrObj);
    //}
    #if (PR500) 
    if( gEventKey )
    {
      keyin = gEventKey;
      gEventKey = K_KeyNull;
    }  
    else
    #endif
    keyin = APM_WaitKey(0,0); 
    switch(keyin)
    {
      #if (PR500)
      case MWKEY_1: SamUpDn(TRUE);  break;
      case MWKEY_2: SamUpDn(FALSE); break;
      case MWKEY_3: SenUpDn(TRUE);  break;
      case MWKEY_4: SenUpDn(FALSE); break;
      #endif
      case MWKEY_5: DotUpDn(TRUE);  break;
      case MWKEY_6: DotUpDn(FALSE); break;
    }
    if ((keyin == MWKEY_ENTER)||(keyin==MWKEY_CANCL)) {
      CleanTPadObj();
      if (keyin==MWKEY_CANCL) {
        break;
      }
      DispCtrlMW(MW_CLR_DISP);
      #if (PR500)
      DispLineMW(KSPadConf1, MW_LINE1, MW_SPFONT);
      DispLineMW(KSPadConf2, MW_LINE2, MW_SPFONT);
      DispLineMW(KSPadConf3, MW_LINE3, MW_SPFONT);
      #else
      DispLineMW(KSPadConf, MW_LINE1, MW_SPFONT);
      #endif
      #if(T1000)
      sample    = UpdTPadSample();
      sensitive = UpdTPadSensitivity();
      #endif
      UpdTPadDotSize();
      obj_cnt++;
      if (obj_cnt > MAX_TPAD_OBJ) {
        obj_cnt = 1;
      }
      add_obj = TRUE;
      continue;
    }
    Delay10ms(1);
  }
  CleanTPadConf();
#if (PR500)
  redrawLogo();
#endif
  lcdbanner_sts = FALSE;
  #if(T1000)
  os_tpad_sensitivity(sensitive);
  os_tpad_sampling(sample);
  #endif
  #if !(PR500)
  os_config_write(K_CF_LcdBannerEnable, lcdbanner_sts);
  os_config_update();
  #endif
  #endif //(TPAD_SUPPORT)    
}
//*****************************************************************************
//  Function        : TPadTestEnable
//  Description     : Return TRUE if the test is enable for the terminal.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN TPadTestEnable(void)
{
// #define K_HdTPadInstalled       0x00400000
  #if (TPAD_SUPPORT)    
  // if (os_hd_config() & K_HdTPadinstalled) {
  if (os_hd_config() & 0x00400000) {
    return TRUE;
  }
  #endif
  return FALSE;
}

//*****************************************************************************
//  Function        : SPadTest
//  Description     : Test Signature Pad.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SPadTest(void)
{
  #if (TPAD_SUPPORT)    
  DWORD      keyin;
  #if(T1000|PR500)
  DWORD      sample, sensitive;
  #endif
  BYTE       lcdbanner_sts;
  #if (TMLPT_SUPPORT)
  int        ret;
  BYTE       tmp[128];
  #endif
  
  #if(T300|T810)
  //if T300|T810 no touch Pad
   if ((os_hd_config() & K_HdTPadInstalled) == FALSE) {
     DispLineMW("No Touch Pad", MW_LINE5, MW_CENTER|MW_CLRDISP|MW_BIGFONT);
     APM_WaitKey(9000, 0);
     return;
   }
  #endif

  lcdbanner_sts = TRUE;
  #if !(PR500)
  os_config_write(K_CF_LcdBannerEnable, lcdbanner_sts);
  os_config_update();
  #endif
  IconPutS(KSPadTest, RGB(0,0,0));

  os_disp_putc(K_PushCursor);
  DispCtrlMW(MW_CLR_DISP);
  #if (PR500)
  DispLineMW(KSPadConf1, MW_LINE1, MW_SPFONT);
  DispLineMW(KSPadConf2, MW_LINE2, MW_SPFONT);
  DispLineMW(KSPadConf3, MW_LINE3, MW_SPFONT);
  GD_SetDotColor(0xff, 0xff, 0xff);
  ClearLCD();
  #else
  DispLineMW(KSPadConf, MW_LINE1, MW_SPFONT);
  #endif
  #if(T1000)
  os_tpad_sensitivity(2);
  #endif
  GD_SetDotSize(gDotSize);
  #if(T1000)
  sample    = UpdTPadSample();
  sensitive = UpdTPadSensitivity();
  #endif
  UpdTPadDotSize();
  #if (PR500)
  GD_SetDotColor(0xff, 0xff, 0xff);
  os_tpad_disp_bl_control(0xFF);
  #else
  GD_SetDotColor(0, 0, 0);
  #endif
  #if(T1000|PR500)
  InitTPadConf();
  sensitive = os_tpad_sensitivity(0);
  sample    = os_tpad_sampling(0);
  #endif
  while (TRUE) {
    #if (PR500)
    if( gEventKey )
    {
      keyin = gEventKey;
      gEventKey = K_KeyNull;
    }  
    else
    #endif
    keyin = APM_WaitKey(0,0);
    switch(keyin)
    {
      #if (PR500)
      case MWKEY_1: SamUpDn(TRUE);  break;
      case MWKEY_2: SamUpDn(FALSE); break;
      case MWKEY_3: SenUpDn(TRUE);  break;
      case MWKEY_4: SenUpDn(FALSE); break;
      #endif
      case MWKEY_5: DotUpDn(TRUE);  break;
      case MWKEY_6: DotUpDn(FALSE); break;
    }
    #if (T300|T810)
    if ((keyin == MWKEY_POWER) || (keyin == MWKEY_FUNC3))
    #else
    if (keyin == MWKEY_POWER)
    #endif
      break;
    if (keyin == MWKEY_CLR) {
      DispCtrlMW(MW_CLR_DISP);
      #if (PR500)
      CleanTPadConf();
      DispLineMW(KSPadConf1, MW_LINE1, MW_SPFONT);
      DispLineMW(KSPadConf2, MW_LINE2, MW_SPFONT);
      DispLineMW(KSPadConf3, MW_LINE3, MW_SPFONT);
      ClearLCD();
      #else
      #if(T1000|PR500)
      InitTPadConf();
      #endif
      DispLineMW(KSPadConf, MW_LINE1, MW_SPFONT);
      #endif
      #if(T1000|PR500)
      UpdTPadSample();
      UpdTPadSensitivity();
      #endif
      UpdTPadDotSize();
    }
    #if (TMLPT_SUPPORT)
    #if (A5T1000|T1000|T300|T810)
    if (keyin == MWKEY_LEFT) {
    #else
    if (keyin == MWKEY_FUNC) {
    #endif
      //ret = PrintLcd2Lpt(0,0,320,216);
      ret = PrintLcd2LptCtr(0,24,320,192);
      if (ret < 0) {
        sprintf(tmp, "PrintLcd2Lpt:%d", ret);
        DispLineMW(tmp, MW_LINE9, MW_SPFONT|MW_CENTER|MW_REVERSE);
        APM_WaitKey(300,0);
      }
    }
    #endif
    ShowTouchPoint();
  }
  CleanTPadConf();
#if (PR500)
  redrawLogo();
  os_tpad_disp_bl_control(0x00);
#endif
  #if(T1000|PR500)
  os_tpad_sensitivity(sensitive);
  os_tpad_sampling(sample);
  #endif
  os_disp_putc(K_PopCursor);
  lcdbanner_sts = FALSE;
  #if !(PR500)
  os_config_write(K_CF_LcdBannerEnable, lcdbanner_sts);
  os_config_update();
  #endif
  #endif //(TPAD_SUPPORT)    
}
