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

#include <lx_emul/extern_c_begin.h>

typedef unsigned long atomic_long_t;
typedef int clockid_t;

enum { HZ = 100UL };

#include <lx_emul/compiler.h>
#include <lx_emul/printf.h>
#include <lx_emul/types.h>
#include <lx_emul/irq.h>
#include <lx_emul/kernel.h>
#include <lx_emul/jiffies.h>
#include <lx_emul/time.h>
#include <lx_emul/timer.h>

#include <lx_emul/extern_c_end.h>

#endif /* _SRC__DRIVERS__NIC__FEC__LX_EMUL_H_ */

