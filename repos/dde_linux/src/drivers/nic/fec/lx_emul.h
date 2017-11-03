/*
 * \brief  Freescale ethernet driver Linux emulation environment
 * \author Stefan Kalkowski
 * \date   2017-11-01
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

#ifndef _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_
#define _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_

#include <base/fixed_stdint.h>
#include <stdarg.h>

#include <lx_emul/extern_c_begin.h>

typedef unsigned long atomic_long_t;
typedef int clockid_t;

enum {
	HZ        = 100UL,
	PAGE_SIZE = 4096UL
};

#include <lx_emul/compiler.h>
#include <lx_emul/printf.h>
#include <lx_emul/types.h>
#include <lx_emul/irq.h>
#include <lx_emul/kernel.h>
#include <lx_emul/jiffies.h>
#include <lx_emul/time.h>
#include <lx_emul/timer.h>
#include <lx_emul/mutex.h>


typedef __s64 time64_t;

struct timespec64 {
	time64_t tv_sec;  /* seconds */
	long     tv_nsec; /* nanoseconds */
};

typedef enum {
	/*PHY_INTERFACE_MODE_NA,
	PHY_INTERFACE_MODE_MII,
	PHY_INTERFACE_MODE_GMII,
	PHY_INTERFACE_MODE_SGMII,
	PHY_INTERFACE_MODE_TBI,
	PHY_INTERFACE_MODE_REVMII,
	PHY_INTERFACE_MODE_RMII,
	PHY_INTERFACE_MODE_RGMII,
	PHY_INTERFACE_MODE_RGMII_ID,
	PHY_INTERFACE_MODE_RGMII_RXID,
	PHY_INTERFACE_MODE_RGMII_TXID,
	PHY_INTERFACE_MODE_RTBI,
	PHY_INTERFACE_MODE_SMII,
	PHY_INTERFACE_MODE_XGMII,
	PHY_INTERFACE_MODE_MOCA,
	PHY_INTERFACE_MODE_QSGMII,
	PHY_INTERFACE_MODE_MAX,*/
	DUMMY
} phy_interface_t;

#include <lx_emul/completion.h>

struct completion {
};


#include <lx_emul/extern_c_end.h>

#endif /* _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_ */

