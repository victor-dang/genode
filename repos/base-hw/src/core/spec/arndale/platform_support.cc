/*
 * \brief   Parts of platform that are specific to Arndale
 * \author  Martin Stein
 * \author  Stefan Kalkowski
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
#include <kernel/kernel.h>
#include <platform_pd.h>
#include <unmanaged_singleton.h>
#include <long_translation_table.h>
#include <util/mmio.h>

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
		{ 0x10440000, 0x1000 }, /* IRQ Combiner */
	};
	return i < sizeof(_regions)/sizeof(_regions[0]) ? &_regions[i] : 0;
}


Cpu::User_context::User_context() { cpsr = Psr::init_user(); }


struct Irq_combiner : Genode::Mmio
{
		struct Iesr1 : Register<0x10, 32> { };

		Irq_combiner() : Mmio(0x10440000)
		{
			write<Iesr1>(1 << 24 | 1 << 25); /* enable SYSMMU MDMA0 */
		}
};


/* hypervisor exception vector address */
extern void* _hyp_kernel_entry;
extern void* _mt_vm_entry_pic;

static unsigned char hyp_mode_stack[1024];

Genode::addr_t Genode::Board::vm_entry() {
	return (addr_t) &_mt_vm_entry_pic; }

void Genode::Board::prepare_kernel()
{
	static Irq_combiner combiner;

	using Cpsr = Cpu::Psr;

	/* ARM generic timer counter freq needs to be set in secure mode */
	volatile unsigned long * mct_control  = (unsigned long*) 0x101C0240;
	*mct_control  = 0x100;
	asm volatile ("mcr p15, 0, %0, c14, c0, 0" :: "r" (24000000));

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
			"mov lr, #218                    \n"  /* set PSR to hyp mode     */
			"msr spsr_cxfs, lr               \n"  /* prepare saved PSR       */
			"mcr p15, 0, %[scr], c1, c1, 0   \n"  /* set SCR register        */
			"adr lr, 1f                      \n"  /* set return address      */
			"subs pc, lr, #0                 \n"  /* exception return        */
			"1: mov sp, r3                   \n"  /* restore sp              */
			"mov lr, r4                      \n"  /* restore lr              */
			:: [scr] "r" (scr), [nsacr] "r" (nsacr) : "r3", "r4");
	}

	/* set hyp exception vector */
	void *v = &_hyp_kernel_entry;
	asm volatile ("mcr p15, 4, %[v], c12, c0, 0" :: [v]"r"(v));

	void * pt_phys =
		Kernel::core_pd()->platform_pd()->translation_table_phys();

//	using Ttable = Genode::Level_1_stage_2_translation_table;
//	constexpr int tt_align  = 1 << Ttable::ALIGNM_LOG2;
//	Ttable            * tt   = unmanaged_singleton<Ttable, tt_align>();
//	tt->insert_translation(0, 0, 0x80000000,
//						   Page_flags::apply_mapping(true, UNCACHED, true), 0);
//	tt->insert_translation(0x80000000, 0x80000000, 0x40000000,
//						   Page_flags::apply_mapping(true, CACHED, false), 0);
//	tt->insert_translation(0xc0000000, 0xc0000000, 0x40000000,
//						   Page_flags::apply_mapping(true, UNCACHED, true), 0);

	/* set HTTBR */
	asm volatile ("mcrr p15, 4, %[v0], %[v1], c2"
	              :: [v0]"r"(pt_phys), [v1]"r"(0));

	/* set HTCR */
	asm volatile ("mcr p15, 4, %[v], c2, c0, 2"
	              :: [v] "r" (Cpu::Ttbcr::init_virt_kernel()));

//	/* set VTTBR */
//	asm volatile ("mcrr p15, 6, %[v0], %[v1], c2"
//	              :: [v0]"r"(tt), [v1]"r"(0));

	/* set VBAR for Linux temporarily */
//	asm volatile("MCR p15, 0, %0, c12, c0, 0" :: "r" (0x8029e660));

	/* HCPTR */
	asm volatile("MCR p15, 4, %0, c1, c1, 2" :: "r" (0x4010b3ff));

	/* set VTCR */
	asm volatile ("mcr p15, 4, %[v], c2, c1, 2"
	              :: [v] "r" (Cpu::Ttbcr::init_virt_kernel() | (1 << 6) /* SL0 == 1 */));

	/* set HMAIR0 */
	asm volatile ("mcr p15, 4, %[v], c10, c2, 0" :: [v] "r" (0xff0044) : );

	/* set HSCTRL (enable caches and MMU) */
	asm volatile ("mcr p15, 4, %[v], c1, c0, 0" ::
	              [v] "r" ((1 << 12) | (1 << 2) | 1) : );

//	unsigned long hcr = 1;
//	asm volatile ("mcr p15, 4, %[v], c1, c1, 0" :: [v] "r" (hcr));

	/* hyp return to supervisor mode */
	asm volatile (
		"mov r0, sp        \n"
		"mov r1, lr        \n"
		"mov sp, %[stack]  \n"
		"adr r2, 1f        \n"
		"msr elr_hyp, r2   \n"
		"mov r2, #211      \n"
		"msr spsr_cxfs, r2 \n"
		"eret              \n"
		"1: mov sp, r0     \n"
		"mov lr, r1        \n"
		:: [stack] "r" (&hyp_mode_stack) : "r0", "r1", "r2");
}
