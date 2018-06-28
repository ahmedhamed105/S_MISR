//-----------------------------------------------------------------------------
//  File          : mcpconnect.c
//  Module        :
//  Description   : MCP Connect.
//  Author        : Lewis
//  Notes         : N/A
//
//  Naming conventions
//  ~~~~~~~~~~~~~~~~~~
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  01 Dec  2007  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "basecall.h"
#include "midware.h"
#include "apm.h"
#include "hwdef.h"
#include "sysutil.h"
#include "mcp.h"

#if (MCP_SUPPORT)
//-----------------------------------------------------------------------------
// Define
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant
//-----------------------------------------------------------------------------

//*****************************************************************************
//  Function        : GetMCPStatus
//  Description     : Return current MCP status
//  Input           : N/A
//  Return          : MCP status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetMCPStatus(void)
{
  return bs_mcp_get_phase();
}
#endif
