/*
-----------------------------------------------------------------------------
-      ATMEL Microcontroller Software Support  -   ROUSSET -
-----------------------------------------------------------------------------
 The software is delivered "AS IS" without warranty or condition of any
 kind  DEFINE   either express  DEFINE   implied or statutory. This includes without
 limitation any warranty or condition with respect to merchantability or
 fitness for any particular purpose  DEFINE   or against the infringements of
 intellectual property rights of others.
-----------------------------------------------------------------------------
- File Name            : arm.inc
- Object               : ARM Core Definition File.
- Translator           : ARM Software Development Toolkit V2.11a
-
- 1.0 10/03/98 JCZ     : Creation
- 2.0 21/10/98 JCZ     : Clean up.
-----------------------------------------------------------------------------
*/
/*
-------------------------------
- ARM Core Mode and Status Bits
-------------------------------
*/
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))

ARM_MODE_USER  	DEFINE  0x10
ARM_MODE_FIQ  	DEFINE  0x11
ARM_MODE_IRQ  	DEFINE  0x12
ARM_MODE_SVC  	DEFINE  0x13
ARM_MODE_ABT  	DEFINE  0x17
ARM_MODE_UND    DEFINE  0x1B
ARM_MODE_SYS  	DEFINE  0x1F
ARM_MODE_BITS 	DEFINE	0x1F

I_BIT  DEFINE  0x80
F_BIT  DEFINE  0x40
T_BIT  DEFINE  0x20

#endif
