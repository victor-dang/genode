/*
 * \brief  Kernel entrypoint for non-SMP systems
 * \author Martin Stein
 * \author Stefan Kalkowski
 * \date   2011-10-20
 */

/*
 * Copyright (C) 2011-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* core includes */
#include <kernel/cpu.h>


extern "C" void kernel()
{
	using namespace Kernel;

	Cpu * const cpu = cpu_pool()->cpu(Cpu::executing_id());
	cpu->scheduled_job().exception(*cpu);
	cpu->scheduled_job().proceed(*cpu);
}


void Kernel::Cpu::Ipi::occurred(Cpu &) { }


void Kernel::Cpu::Ipi::trigger(unsigned const cpu_id) { }
