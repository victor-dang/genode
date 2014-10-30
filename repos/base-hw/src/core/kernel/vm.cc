/*
 * \brief  Kernel backend for virtual machines
 * \author Martin Stein
 * \date   2013-09-15
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <kernel/vm.h>

class Gic_vcpu_control : Genode::Mmio
{
	private:

		struct Gich_hcr    : Register<0x00, 32> { };
		struct Gich_vmcr   : Register<0x08, 32> { };
		struct Gich_misr   : Register<0x10, 32> { };
		struct Gich_eisr0  : Register<0x20, 32> { };
		struct Gich_elrsr0 : Register<0x30, 32> { };
		struct Gich_apr    : Register<0xf0, 32> { };

		template <unsigned SLOT>
		struct Gich_lr : Register<0x100 + SLOT*4, 32> { };

	public:

		Gic_vcpu_control() : Genode::Mmio(0x10484000) { }

		void save (Kernel::Vm::Vm_state *s)
		{
			s->gic_hcr    = read<Gich_hcr   >();
			s->gic_misr   = read<Gich_misr  >();
			s->gic_vmcr   = read<Gich_vmcr  >();
			s->gic_apr    = read<Gich_apr   >();
			s->gic_eisr   = read<Gich_eisr0 >();
			s->gic_elrsr0 = read<Gich_elrsr0>();
			s->gic_lr[0]  = read<Gich_lr<0> >();
			s->gic_lr[1]  = read<Gich_lr<1> >();
			s->gic_lr[2]  = read<Gich_lr<2> >();
			s->gic_lr[3]  = read<Gich_lr<3> >();

			write<Gich_hcr>(0);
		}

		void load (Kernel::Vm::Vm_state *s)
		{
			write<Gich_hcr   >(s->gic_hcr );
			write<Gich_misr  >(s->gic_misr);
			write<Gich_vmcr  >(s->gic_vmcr);
			write<Gich_apr   >(s->gic_apr );
			write<Gich_elrsr0>(s->gic_elrsr0);
			write<Gich_lr<0> >(s->gic_lr[0]);
			write<Gich_lr<1> >(s->gic_lr[1]);
			write<Gich_lr<2> >(s->gic_lr[2]);
			write<Gich_lr<3> >(s->gic_lr[3]);
		}
};

static Gic_vcpu_control _vgic;

namespace Kernel
{
	Vm_ids * vm_ids() { return unmanaged_singleton<Vm_ids>(); }
	Vm_pool * vm_pool() { return unmanaged_singleton<Vm_pool>(); }
}


Kernel::Vm::Vm(void                   * const state,
               Kernel::Signal_context * const context,
               void                   * const table)
: Processor_client(processor_pool()->primary_processor(),
                   Priority::MIN),
  _state((Vm_state * const)state),
  _context(context),
  _table(table)
{
	pic()->unmask(Genode::Board::VIRTUAL_MAINTAINANCE_IRQ,
	              processor_pool()->primary_processor()->id());
}



void Kernel::Vm::exception(unsigned const processor_id)
{
	/* save timer values */
	asm volatile("mrc p15, 0, %0, c14, c3, 0 \n"
	             "mrc p15, 0, %1, c14, c3, 1" :
	             "=r" (_state->timer_val),
	             "=r" (_state->timer_ctrl));

	switch(_state->cpu_exception) {
	case Genode::Cpu_state::INTERRUPT_REQUEST:
	case Genode::Cpu_state::FAST_INTERRUPT_REQUEST:
		_interrupt(processor_id);
		break;
	default:
		pause();
		_context->submit(1);
	}

	_vgic.save(_state);
	pic()->mask(Genode::Board::VIRTUAL_TIMER_IRQ);

	/* reset timer */
	asm volatile("mcr p15, 0, %0, c14, c3, 1 \n"
	             "mcr p15, 0, %0, c14, c3, 0" :: "r" (0));
}


void Kernel::Vm::proceed(unsigned const processor_id)
{
	_state->vm_id = id() << 16;
	_state->table = (addr_t)_table;
	_state->hstr  = 0x9c6f;
	_state->hcr   = 0x4306039;

	_vgic.load(_state);

	if (_state->timer_irq)
		pic()->unmask(Genode::Board::VIRTUAL_TIMER_IRQ, processor_id);

	/* restore timer values */
	asm volatile("mcr p15, 0, %0, c14, c3, 1 \n"
	             "mcr p15, 0, %1, c14, c3, 0 \n"
	             "mcr p15, 0, %2, c14, c3, 1" ::
	             "r" (0),
	             "r" (_state->timer_val),
	             "r" (_state->timer_ctrl));

	mtc()->continue_vm(_state, processor_id);
}
