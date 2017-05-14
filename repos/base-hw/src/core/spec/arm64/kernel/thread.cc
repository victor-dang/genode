/*
 * \brief   Kernel backend for execution contexts in userland
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
#include <kernel/thread.h>

using namespace Kernel;

void Kernel::Thread::_init()
{
	Genode::error("FIXME: Kernel::Thread::_init");
}


void Thread::exception(unsigned const cpu)
{
	Genode::error("FIXME: Thread::exception");
}


void Thread::_mmu_exception()
{
	Genode::error("FIXME: Thread::_mmu_exception");
}


void Kernel::Thread::_call_update_data_region()
{
}


void Kernel::Thread::_call_update_instr_region()
{
}

