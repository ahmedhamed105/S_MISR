#ifndef _ATMEL_SAMA5D2_ALL_H_
#define _ATMEL_SAMA5D2_ALL_H_

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#endif

#undef   CONFIG_SOC_SAMA5D21
#undef   CONFIG_SOC_SAMA5D22
#undef   CONFIG_SOC_SAMA5D23
#undef   CONFIG_SOC_SAMA5D24
#undef   CONFIG_SOC_SAMA5D25
#undef   CONFIG_SOC_SAMA5D26
#undef   CONFIG_SOC_SAMA5D27
#undef   CONFIG_SOC_SAMA5D28

#define  CONFIG_SOC_SAMA5D28   1

#if defined(__cplusplus)
#define __I   volatile         /* read-only permission */
#else
#define __I   volatile const   /* read-only permission */
#endif
#define __O   volatile         /* write-only permission */
#define __IO  volatile         /* read/write permission */

#if defined(CONFIG_SOC_SAMA5D21)
  #include "sama5d2/sama5d21.h"
#elif defined(CONFIG_SOC_SAMA5D22)
  #include "sama5d2/sama5d22.h"
#elif defined(CONFIG_SOC_SAMA5D23)
  #include "sama5d2/sama5d23.h"
#elif defined(CONFIG_SOC_SAMA5D24)
  #include "sama5d2/sama5d24.h"
#elif defined(CONFIG_SOC_SAMA5D26)
  #include "sama5d2/sama5d26.h"
#elif defined(CONFIG_SOC_SAMA5D27)
  #include "sama5d2/sama5d27.h"
#elif defined(CONFIG_SOC_SAMA5D28)
  #include "sama5d2/sama5d28.h"
#else
  #error Library does not support the specified device.
#endif

#include "sama5d2/chip_pins.h"
#include "sama5d2/chip_misc.h"
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#include "sama5d2/compiler.h"
#endif

#if ( defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__) )
#include "arm.inc"
#endif

#endif
