/*
 * \brief  CPU driver for core
 * \author Martin stein
 * \author Stefan Kalkowski
 * \date   2012-09-11
 */

/*
 * Copyright (C) 2012-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__ARM__CPU_SUPPORT_H_
#define _CORE__SPEC__ARM__CPU_SUPPORT_H_

/* Genode includes */
#include <util/register.h>
#include <cpu/cpu_state.h>
#include <base/internal/align_at.h>

#include <hw/spec/arm/cpu.h>

/* local includes */
#include <kernel/interface_support.h>
#include <kernel/kernel.h>
#include <board.h>
#include <util.h>

namespace Genode {
	using sizet_arithm_t = Genode::uint64_t;
	struct Arm_cpu;
}

struct Genode::Arm_cpu : public Hw::Arm_cpu
{
	/**
	 * Translation table base register 0
	 */
	struct Ttbr0 : Hw::Arm_cpu::Ttbr0
	{
		enum Memory_region { NON_CACHEABLE = 0, CACHEABLE = 1 };

		/**
		 * Return initialized value
		 *
		 * \param table  base of targeted translation table
		 */
		static access_t init(addr_t const table)
		{
			access_t v = Ttbr::Ba::masked((addr_t)table);
			Ttbr::Rgn::set(v, CACHEABLE);
			Ttbr::S::set(v, Board::SMP ? 1 : 0);
			if (Board::SMP) Ttbr::Irgn::set(v, CACHEABLE);
			else Ttbr::C::set(v, 1);
			return v;
		}
	};

	struct alignas(4) Context : Cpu_state
	{
		Context(bool privileged);
	};

	/**
	 * This class comprises ARM specific protection domain attributes
	 */
	struct Mmu_context
	{
		Cidr::access_t  cidr;
		Ttbr0::access_t ttbr0;

		Mmu_context(addr_t page_table_base);
		~Mmu_context();

		uint8_t id() { return cidr; }
	};

	/**
	 * Returns true if current execution context is running in user mode
	 */
	static bool is_user() { return Psr::M::get(Cpsr::read()) == Psr::M::USR; }

	/**
	 * Invalidate all entries of all instruction caches
	 */
	static void invalidate_instr_cache() {
		asm volatile ("mcr p15, 0, %0, c7, c5, 0" :: "r" (0) : ); }

	/**
	 * Flush all entries of all data caches
	 */
	static void clean_invalidate_data_cache();

	/**
	 * Invalidate all branch predictions
	 */
	static void invalidate_branch_predicts() {
		asm volatile ("mcr p15, 0, r0, c7, c5, 6" ::: "r0"); };

	static constexpr addr_t line_size = 1 << Board::CACHE_LINE_SIZE_LOG2;
	static constexpr addr_t line_align_mask = ~(line_size - 1);

	/**
	 * Clean and invalidate data-cache for virtual region
	 * 'base' - 'base + size'
	 */
	void clean_invalidate_data_cache_by_virt_region(addr_t base,
	                                                size_t const size)
	{
		addr_t const top = base + size;
		base &= line_align_mask;
		for (; base < top; base += line_size) { Dccimvac::write(base); }
	}

	/**
	 * Invalidate instruction-cache for virtual region
	 * 'base' - 'base + size'
	 */
	void invalidate_instr_cache_by_virt_region(addr_t base,
	                                           size_t const size)
	{
		addr_t const top = base + size;
		base &= line_align_mask;
		for (; base < top; base += line_size) { Icimvau::write(base); }
	}

	void switch_to(Context&, Mmu_context & o)
	{
		if (o.cidr == 0) return;

		Cidr::access_t cidr = Cidr::read();
		if (cidr != o.cidr) {
			Cidr::write(o.cidr);
			Ttbr0::write(o.ttbr0);
		}
	}

	static bool in_fault(Context & c, addr_t & va, addr_t & w, bool & p)
	{
		/* translation fault on section */
		static constexpr Fsr::access_t section    = 5;
		/* translation fault on page */
		static constexpr Fsr::access_t page       = 7;
		/* permission fault on page */
		static constexpr Fsr::access_t permission = 0xf;

		if (c.cpu_exception == Context::PREFETCH_ABORT) {
			/* check if fault was caused by a translation miss */
			Ifsr::access_t const fs = Fsr::Fs::get(Ifsr::read());

			if (fs == permission) {
				w = 0;
				va = Ifar::read();
				p = true;
				return true;
			}

			if (fs != section && fs != page)
				return false;

			/* fetch fault data */
			w = 0;
			va = Ifar::read();
			p = false;
			return true;
		} else {
			/* check if fault is of known type */
			Dfsr::access_t const fs = Fsr::Fs::get(Dfsr::read());
			if (fs != permission && fs != section && fs != page)
				return false;

			/* fetch fault data */
			Dfsr::access_t const dfsr = Dfsr::read();
			w = Dfsr::Wnr::get(dfsr);
			va = Dfar::read();
			p = false;
			return true;
		}
	}

	/*************
	 ** Dummies **
	 *************/

	bool retry_undefined_instr(Context&) { return false; }

	/**
	 * Return kernel name of the executing CPU
	 */
	static unsigned executing_id() { return 0; }

	/**
	 * Return kernel name of the primary CPU
	 */
	static unsigned primary_id() { return 0; }
};

#endif /* _CORE__SPEC__ARM__CPU_SUPPORT_H_ */
