/*
 * \brief  TrustZone specific functions for non-TZ case
 * \author Stefan Kalkowski
 * \date   2012-10-10
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <trustzone.h>
#include <pic.h>
#include <cpu.h>
#include <board.h>

/* monitor exception vector address */
extern int _mon_kernel_entry;

void Kernel::trustzone_initialization(Pic *pic) {

	/* set exception vector entry */
	Genode::Cpu::mon_exception_entry_at((Genode::addr_t)&_mon_kernel_entry);

	/* enable coprocessor access for TZ VMs */
	Genode::Cpu::allow_coprocessor_nonsecure();
}
