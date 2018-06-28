/*
************************************
*       Module : common.h          *
*       Name   : TF                *
*       Date   : 16-06-2015        *
************************************
        Common typedefs and declarations
*/

#ifndef _COMMON_H_
#define _COMMON_H_

typedef unsigned char       BOOLEAN;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int        DWORD;          /* 32 bit data type */
typedef unsigned long long  DDWORD;          /* 64 bit data type */
typedef const BYTE *        CSTR;

typedef void (*FN00)(void);
typedef void (*FN01)(DWORD a_p1);
typedef void (*FN02)(DWORD a_p1, DWORD a_p2);
typedef void (*FN03)(DWORD a_p1, DWORD a_p2, DWORD a_p3);
typedef DWORD (*FN10)(void);
typedef DWORD (*FN11)(DWORD a_p1);
typedef DWORD (*FN12)(DWORD a_p1,DWORD a_p2);
typedef DWORD (*FN13)(DWORD a_p1,DWORD a_p2,DWORD a_p3);


typedef struct {
  BYTE    b_len;
  BYTE    b_comm_mode;
  BYTE    b_comm_speed;
  BYTE    b_poll_addr;
  union {
    WORD  w_buf_size;
    BYTE  s_gap[2];
  };
  union {
    BYTE   *p_buf_addr;
    BYTE   s_extra[4];
  };
} T_COMM_CMD;

typedef void (*FN)(void);               /* function variable type */
typedef struct chain {
  struct chain *p_next;
  FN            p_fn;
} T_CHAIN;                              /* function link list type */


#ifndef NULL
#define NULL            (void *)0
#endif

#define TRUE              1
#define FALSE             0

#define true              1
#define false             0

#define ON                1
#define OFF               0

#define on                1
#define off               0

#define swapw(x)        ((x[1]<<8) + x[0])
#define swapl(x)        ((x[3]<<24) + (x[2]<<16) + (x[1]<<8) + x[0])
#define conv_bw(x)      ((x[0]<<8) + x[1])
#define conv_bl(x)      ((x[0]<<24) + (x[1]<<16) + (x[2]<<8) + x[3])
#define conv_lw(x)      ((x[1]<<8) + x[0])
#define conv_ll(x)      ((x[3]<<24) + (x[2]<<16) + (x[1]<<8) + x[0])

#define min(a,b)  ( (a) < (b) ? (a) : (b) )
#define max(a,b)  ( (a) > (b) ? (a) : (b) )

#ifndef __IAR_SYSTEMS_ICC__
#define __pcs
#else
  #if (__IAR_SYSTEMS_ICC__ >= 6)
    #define __pcs
  #endif
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif
#endif  //_COMMON_H_
