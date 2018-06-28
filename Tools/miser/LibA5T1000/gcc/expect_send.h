#ifndef _EXPECT_SEND_H_
#define _EXPECT_SEND_H_

#include "common.h"
#include "basecall.h"

extern __pcs T_EXPECT_SEND * expect_send_alloc(T_EXPECT_SEND * a_s,DWORD a_n);
extern __pcs void expect_send_release(T_EXPECT_SEND ** a_s, DWORD a_n);

#endif
