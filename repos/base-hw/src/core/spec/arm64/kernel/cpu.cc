/*
 * \brief   Kernel cpu driver implementations specific to ARM64
 * \author  Stefan Kalkowski
 * \date    2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/log.h>
#include <kernel/cpu.h>

using namespace Kernel;


Cpu_idle::Cpu_idle(Cpu * const cpu) : Cpu_job(Cpu_priority::MIN, 0)
{
	Genode::error("FIXME: Cpu_idle::Cpu_idle");
}


void Cpu_idle::exception(unsigned const cpu)
{
	Genode::error("FIXME: Cpu_idle::exception");
}


void Kernel::Cpu_context::_init(size_t const stack_size, addr_t const table)
{
	Genode::error("Kernel::Cpu_context::_init");
}


void Kernel::Cpu::init(Kernel::Pic &pic)
{
	Genode::error("Kernel::Cpu::init");
}


