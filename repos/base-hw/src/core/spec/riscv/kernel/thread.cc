/*
 * \brief   Kernel backend for execution contexts in userland
 * \author  Sebastian Sumpf
 * \date    2015-06-02
 */

/*
 * Copyright (C) 2015-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* core includes */
#include <kernel/pd.h>
#include <kernel/thread.h>

using namespace Kernel;

void Kernel::Thread::_init() { }

void Thread::exception(unsigned const cpu)
{
	if (regs->is_irq())
		return;

	switch(regs->cpu_exception) {
	case Cpu::Context::ECALL_FROM_USER:
	case Cpu::Context::ECALL_FROM_SUPERVISOR:
		_call();
		regs->ip += 4; /* set to next instruction */
		break;
	case Cpu::Context::INSTRUCTION_PAGE_FAULT:
	case Cpu::Context::STORE_PAGE_FAULT:
	case Cpu::Context::LOAD_PAGE_FAULT:
		_mmu_exception();
		break;
	default:
		Genode::error(*this, ": unhandled exception ", regs->cpu_exception,
		              " at ip=", (void*)regs->ip,
		              " addr=", Genode::Hex(Cpu::Sbadaddr::read()));
		_die();
	}
}


void Thread::_mmu_exception()
{
	_become_inactive(AWAITS_RESTART);
	_fault_pd   = (addr_t)_pd->platform_pd();
	_fault_addr = Cpu::Sbadaddr::read();

	if (_pager) _pager->submit(1);
}


void Thread::_call_update_pd()
{
	Cpu::sfence();
}


void Thread::_call_update_data_region()
{
	Cpu::sfence();
}


void Thread::_call_update_instr_region() { }


void Kernel::Thread::proceed(unsigned const)
{
	asm volatile("csrw sscratch, %1                                \n"
	             "mv   x31, %0                                     \n"
	             "ld   x30, (x31)                                  \n"
	             "csrw sepc, x30                                   \n"
	             ".irp reg,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,"
	                      "18,19,20,21,22,23,24,25,26,27,28,29,30  \n"
	             "  ld x\\reg, 8 * (\\reg + 1)(x31)                \n"
	             ".endr                                            \n"
	             "csrrw x31, sscratch, x31                         \n"
	             "sret                                             \n"
	             :: "r" (&*regs), "r" (regs->t6) : "x30", "x31");
}
