/*
 * \brief   Parts of platform that are specific to Arndale
 * \author  Martin Stein
 * \date    2012-04-27
 */

/*
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <board.h>
#include <platform.h>
#include <pic.h>
#include <cpu.h>
#include <timer.h>

using namespace Genode;


Native_region * Platform::_ram_regions(unsigned const i)
{
	static Native_region _regions[] =
	{
		{ Board::RAM_0_BASE, Board::RAM_0_SIZE },
	};
	return i < sizeof(_regions)/sizeof(_regions[0]) ? &_regions[i] : 0;
}


Native_region * Platform::_mmio_regions(unsigned const i)
{
	static Native_region _regions[] =
	{
		{ Board::MMIO_0_BASE, Board::MMIO_0_SIZE },
	};
	return i < sizeof(_regions)/sizeof(_regions[0]) ? &_regions[i] : 0;
}


Native_region * Platform::_core_only_mmio_regions(unsigned const i)
{
	static Native_region _regions[] =
	{
		{ Board::GIC_CPU_MMIO_BASE, Board::GIC_CPU_MMIO_SIZE },
		{ Board::MCT_MMIO_BASE, Board::MCT_MMIO_SIZE },
		{ Board::UART_2_MMIO_BASE, 0x1000 },
	};
	return i < sizeof(_regions)/sizeof(_regions[0]) ? &_regions[i] : 0;
}


Cpu::User_context::User_context() { cpsr = Psr::init_user(); }


/* hypervisor exception vector address */
extern void* _hyp_kernel_entry;

void Genode::Board::prepare_kernel()
{
	using Cpsr = Cpu::Psr;

	/* if not in HYP mode, get into it */
	if (Cpsr::M::get(Cpsr::read()) != 0b11010)
	{
		/* we assume u-boot put us into secure supervisor mode */
		unsigned long nsacr = 0x43fff;    /* allow coprocessor accesses      */
		unsigned long scr = 0b1110110001; /* SCR bits: NS, FW, AW, SCD, HCD,
		                                     and SIF                         */
		asm volatile (
			"mov r3, sp                      \n"  /* copy current mode's sp  */
			"mov r4, lr                      \n"  /* copy current mode's lr  */
			"mcr p15, 0, %[nsacr], c1, c1, 2 \n"  /* set NSACR register      */
			"cps #22                         \n"  /* switch to monitor mode  */
			"mov sp, r3                      \n"  /* restore sp              */
			"mov lr, r4                      \n"  /* restore lr              */
			"mcr p15, 0, %[scr], c1, c1, 0   \n"  /* set SCR register        */
			"isb" :: [scr] "r" (scr), [nsacr] "r" (nsacr) : "r3");
	}

	/* set hyp exception vector */
	void *v = &_hyp_kernel_entry;
	asm volatile ("mcr p15, 4, %[v], c12, c0, 0" :: [v]"r"(v));

	/* hyp return to supervisor mode */
	asm volatile (
		"mov r0, sp        \n"
		"mov r1, lr        \n"
		"adr r2, 1f        \n"
		"msr elr_hyp, r2   \n"
		"mov r2, #211      \n"
		"msr spsr_cxfs, r2 \n"
		"eret              \n"
		"1: mov sp, r0     \n"
		"mov lr, r1        \n" ::: "r0", "r1", "r2");
}
