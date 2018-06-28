//-----------------------------------------------------------------------------
//  File          : ECR.h
//  Module        :
//  Description   : Declrartion & Defination for ECR.C
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |        Struct define : Leading T                                         |
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
//  24 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_ECR_H_
#define _INC_ECR_H_
#include "common.h"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
// ECR Command Class
#define ECR_CLA_PP300       0x00
#define ECR_CLA_EDC         0x01


// Tags defines follow 8583 0 => 7F
#define ECRTAG_PAN          0xDF02    // packed BCD
#define ECRTAG_TXNAMT       0xDF04    // packed BCD
#define ECRTAG_TRACE        0xDF0B    // packed BCD
#define ECRTAG_TXNTIME      0xDF0C    // packed BCD HHMMSS
#define ECRTAG_TXNDATE      0xDF0D    // packed BCD MMDD
#define ECRTAG_EXPDATE      0xDF0E    // packed BCD YYMM
#define ECRTAG_RRN          0xDF25    // ASCII
#define ECRTAG_AUTH_RESP    0xDF26    // ASCII
#define ECRTAG_RESP_CODE    0xDF27    // ASCII
#define ECRTAG_TERMID       0xDF29    // ASCII
#define ECRTAG_MERCHID      0xDF2A    // ASCII
#define ECRTAG_ADDAMT       0xDF36    // packed BCD
#define ECRTAG_BATCHNO      0xDF3C    // packed BCD


// Tags User define
#define ECRTAG_ECRREF           0xDF81     // ASCII[16];
#define ECRTAG_CARDHOLDERNAME   0xDF82     // ASCII[26];
#define ECRTAG_HOSTIDX          0xDF83     // binary
#define ECRTAG_HOSTNAME         0xDF84     // ASCII[10]
#define ECRTAG_HOSTTOT          0xDF85     //

// ECR EDC command
#define ECR_EDC_SALE        0x00
#define ECR_EDC_OFFLSALE    0x01
#define ECR_EDC_REFUND      0x02
#define ECR_EDC_VOID        0x03
#define ECR_EDC_GETTXN      0x04
#define ECR_EDC_LASTTRACE   0x05
#define ECR_EDC_HOSTNAME    0x06
#define ECR_EDC_HOSTTOTAL   0x07
#define ECR_EDC_AUTH        0x08
#define ECR_EDC_SETTLE      0x09
#define MAX_ECREDC_CMD      0x0A

extern DDWORD EcrGetAmount(void);
extern DWORD EcrGetRef(BYTE *aEcrRef);
extern void EcrResp(void);
extern void EcrCmdProcess(void);

#endif // _INC_ECR_H_

