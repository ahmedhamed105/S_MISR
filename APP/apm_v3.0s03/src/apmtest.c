//-----------------------------------------------------------------------------
//  File          : apmtest.c
//  Module        :
//  Description   : APM Test Functions.
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
#include "midware.h"
#include "logo.h"
#include "hardware.h"
#if (T800)
  #include "testicon.h"
#endif
#include "util.h"
#include "sysutil.h"
#include "kbdutil.h"
#include "rmenu.h"
#include "message.h"
#include "toggle.h"
#include "apmconfig.h"
#include "lptutil.h"
#include "comm.h"
#include "apmfunc.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
enum {
  TST_DISP,
  TST_SAM,
  TST_ICC,
  TST_MSR,
  TST_LPT,
  TST_PPP,
  MAX_TEST_ITEM,
};

enum {
  PPP_SETUP,
  PPP_STATUS,
  PPP_CONNECT,
  PPP_DISCONNECT,
  MAX_PPP_ITEM,
};

struct HOST_DAT {
  BYTE sbIP[4];
  BYTE sbPort[2];
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static struct HOST_DAT gHostDat = {
  {210,  3,  29, 89},
  {0x2E, 0xE0},    // 12000
};
BYTE gReplaceAllComm = FALSE;

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
//static struct RMENUDAT KTestMenu[MAX_TEST_ITEM+1] =
//{
//  { TST_DISP,     "LCD "      }, // extra space for current mark
//  { TST_SAM,      "SAM "      },
//  { TST_ICC,      "ICC "      },
//  { TST_MSR,      "MSR "      },
//  { TST_LPT,      "LPT "      },
//  { TST_PPP,      "PPP "      },
//  { -1,           ""          },
//};
//static struct RMENUDAT KGPRSMenu[MAX_PPP_ITEM+1] =
//{
//  { PPP_SETUP,      "SETUP "      }, // extra space for current mark
//  { PPP_STATUS,     "STATUS "     },
//  { PPP_CONNECT,    "CONNECT "    },
//  { PPP_DISCONNECT, "DIS-CONN " },
//  { -1,              ""          },
//};
//
//static const BYTE *KPPPStatusMsg[7] = {
//  "  Disconnected  ",
//  "    Calling     ",
//  " Authenticating ",
//  "   Negotiating  ",
//  "    Exiting     ",
//  "   Connected    ",
//  "  PPP Not Ready ",
//};
//
//static const BYTE KSelectCmd[20] = {0,0xa4,4,0,14,'1','P','A','Y','.','S','Y','S','.','D','D','F','0','1',0};


//*****************************************************************************
//  Function        : SetCommParam
//  Description     : Setup the communication parameters.
//  Input           : aParam;       // pointer to data.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetTestCommParam(struct COMMPARAM *aParam)
{
  // Config Host comm parameter.
  aParam->wLen = sizeof(struct COMMPARAM);
  aParam->bCommMode      = APM_COMM_TCPIP;
  aParam->bHostConnTime = 30;     // 30sec connection timeout
  aParam->bTimeoutVal    = 15;     // 15sec response timeout

  // Config PPP parameter
  aParam->sPPP.bKeepAlive  = TRUE; // Never disconnect
  aParam->sPPP.dDevice      = MW_PPP_DEVICE_GPRS;
  aParam->sPPP.dSpeed       = 230400;
  aParam->sPPP.dMode        = MW_PPP_MODE_NORMAL;
  memset(aParam->sPPP.scUserID, 0, sizeof(aParam->sPPP.scUserID));
  memset(aParam->sPPP.scPwd,    0, sizeof(aParam->sPPP.scPwd));
  aParam->sPPP.psLogin      = NULL;
  aParam->sPPP.dLoginPair   = 0;
  aParam->sPPP.psDialUp     = NULL;
  aParam->sPPP.dDialupPair  = 0;
  aParam->sPPP.psDialParam  = NULL;

  // Config TCP/IP parameter
  aParam->sTcp.bLen         = sizeof(struct TCPSREG);
  aParam->sTcp.bAdd2ByteLen = TRUE;
  memcpy(&aParam->sTcp.sTcpCfg.d_ip, gHostDat.sbIP, 4);
  aParam->sTcp.sTcpCfg.w_port = gHostDat.sbPort[0]*256+gHostDat.sbPort[1];
  aParam->sTcp.sTcpCfg.b_sslidx = 0;
  aParam->sTcp.sTcpCfg.b_option = MW_TCP_PPP_MODE;
}

//*****************************************************************************
//  Function        : GetMaxMalloc
//  Description     : Check Max available Memory for allocate.
//  Input           : N/A
//  Return          : mem in unit of Kbyte.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetMaxMalloc(void)
{
  DWORD max_mem = 12 * 1024 * 1024;   // max 12 Mbytes
  BYTE  *pMem;

  pMem = NULL;
  while ((pMem == NULL)&&(max_mem >= 1024)) {
    pMem = MallocMW(max_mem);
    if (pMem != NULL)
      break;
    max_mem -= 256;
  }
  if (pMem != NULL) {
    FreeMW(pMem);
    return max_mem;
  }
  return 0;
}
//*****************************************************************************
//  Function        : CalCheckDigit
//  Description     : Prompt user to enter number & calculate its check digit.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CalCheckDigit(void)
{
  BYTE tmp[27];

  while (TRUE) {
    DispLineMW("Enter Number:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    tmp[0]= 0;
    if (!GetKbd(NUMERIC_INPUT+MW_BIGFONT+MW_LINE5+RIGHT_JUST, IMAX(15), tmp))
      return;
    sprintf(tmp, "%c", chk_digit(&tmp[1], tmp[0]));
    DispLineMW(tmp, MW_LINE7, MW_RIGHT|MW_BIGFONT);
    AcceptBeep();
    if (WaitKey(KBD_TIMEOUT)==MWKEY_CANCL)
      break;
  }
}

