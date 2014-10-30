/*
 * \brief   Kernel backend for virtual machines
 * \author  Martin Stein
 * \date    2013-10-30
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _KERNEL__VM_H_
#define _KERNEL__VM_H_

/* Genode includes */
#include <cpu/cpu_state.h>

/* core includes */
#include <kernel/kernel.h>
#include <kernel/pd.h>
#include <kernel/processor_pool.h>
#include <kernel/signal_receiver.h>
#include <pic.h>

namespace Kernel
{
	/**
	 * Kernel backend for a virtual machine
	 */
	class Vm;

	class Vm_ids : public Id_allocator<MAX_VMS> { };
	typedef Object_pool<Vm> Vm_pool;

	Vm_ids  * vm_ids();
	Vm_pool * vm_pool();

	Pic * pic();
}


class Kernel::Vm : public Object<Vm, MAX_VMS, Vm_ids, vm_ids, vm_pool>,
                   public Processor_client
{
	public:

		struct Vm_state : Genode::Cpu_state_modes
		{
			Genode::addr_t table;
			Genode::addr_t vm_id;
			Genode::addr_t stclr;
			Genode::addr_t hstr; /* hsr */
			Genode::addr_t hcr;  /* hpfar */
			Genode::addr_t hdfar;
			Genode::addr_t hifar;
			Genode::addr_t ttbcr;
			Genode::addr_t ttbr0;
			Genode::addr_t ttbr1;
			Genode::addr_t prrr;
			Genode::addr_t nmrr;
			Genode::addr_t dacr;
			Genode::addr_t dfsr;
			Genode::addr_t ifsr;
			Genode::addr_t adfsr;
			Genode::addr_t aifsr;
			Genode::addr_t dfar;
			Genode::addr_t ifar;
			Genode::addr_t cidr;
			Genode::addr_t tls1;
			Genode::addr_t tls2;
			Genode::addr_t tls3;
			Genode::addr_t cpacr;

			Genode::addr_t timer_ctrl;
			Genode::addr_t timer_val;
			bool           timer_irq;

			Genode::addr_t gic_hcr;
			Genode::addr_t gic_vmcr;
			Genode::addr_t gic_misr;
			Genode::addr_t gic_apr;
			Genode::addr_t gic_eisr;
			Genode::addr_t gic_elrsr0;
			Genode::addr_t gic_lr[4];
			unsigned       gic_irq;
		};

	private:

		enum State { ACTIVE, INACTIVE };

		Vm_state       * const _state;
		Signal_context * const _context;
		void           * const _table;
		State                  _scheduled = INACTIVE;

	public:

		/**
		 * Constructor
		 *
		 * \param state    initial CPU state
		 * \param context  signal for VM exceptions other than interrupts
		 * \param table    translation table for guest to host physical memory
		 */
		Vm(void           * const state,
		   Signal_context * const context,
		   void           * const table);

		void inject_irq(unsigned irq)
		{
			_state->gic_irq = irq;
			pause();
			_context->submit(1);
		}


		/****************
		 ** Vm_session **
		 ****************/

		void run()
		{
			if (_scheduled != ACTIVE)
				Processor_client::_schedule();
			_scheduled = ACTIVE;
		}

		void pause()
		{
			if (_scheduled != INACTIVE)
				Processor_client::_unschedule();
			_scheduled = INACTIVE;
		}


		/**********************
		 ** Processor_client **
		 **********************/

		void exception(unsigned const processor_id);
		void proceed(unsigned const processor_id);
};

#endif /* _KERNEL__VM_H_ */
