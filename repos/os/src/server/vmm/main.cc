/*
 * \brief  VMM example for ARM Virtualization
 * \author Stefan Kalkowski
 * \date   2014-07-08
 */

/*
 * Copyright (C) 2014 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/sleep.h>
#include <os/attached_rom_dataspace.h>
#include <vm_session/connection.h>
#include <cpu/cpu_state.h>

struct Vm_state : Genode::Cpu_state_modes
{
	Genode::addr_t dfar;    /* data fault address             */
	Genode::addr_t ttbr[2]; /* translation table base regs    */
	Genode::addr_t ttbrc;   /* translation table base control */
};


class Vm {

	private:

		Genode::Vm_connection          _vm_con;
		Genode::Attached_rom_dataspace _kernel_rom;
		Vm_state                      *_state;

		void _load_kernel()
		{
		}

	public:

		Vm(const char *kernel)
		: _kernel_rom(kernel),
		  _state((Vm_state*)Genode::env()->rm_session()->attach(_vm_con.cpu_state()))
		{ }

		void start()
		{
			Genode::memset((void*)_state, 0, sizeof(Vm_state));
			_load_kernel();
			_state->cpsr = 0x93; /* SVC mode and IRQs disabled */
		}

		void sig_handler(Genode::Signal_context_capability sig_cap) {
			_vm_con.exception_handler(sig_cap); }

		void run()   { _vm_con.run();   }
		void pause() { _vm_con.pause(); }

		void dump()
		{
			using namespace Genode;

			const char * const modes[] =
				{ "und", "svc", "abt", "irq", "fiq" };
			const char * const exc[] =
				{ "invalid", "reset", "undefined", "smc", "pf_abort",
			      "data_abort", "irq", "fiq" };

			printf("Cpu state:\n");
			printf("  Register     Virt     Phys\n");
			printf("---------------------------------\n");
			printf("  r0         = %08lx [%08lx]\n",
			       _state->r0, va_to_pa(_state->r0));
			printf("  r1         = %08lx [%08lx]\n",
			       _state->r1, va_to_pa(_state->r1));
			printf("  r2         = %08lx [%08lx]\n",
			       _state->r2, va_to_pa(_state->r2));
			printf("  r3         = %08lx [%08lx]\n",
			       _state->r3, va_to_pa(_state->r3));
			printf("  r4         = %08lx [%08lx]\n",
			       _state->r4, va_to_pa(_state->r4));
			printf("  r5         = %08lx [%08lx]\n",
			       _state->r5, va_to_pa(_state->r5));
			printf("  r6         = %08lx [%08lx]\n",
			       _state->r6, va_to_pa(_state->r6));
			printf("  r7         = %08lx [%08lx]\n",
			       _state->r7, va_to_pa(_state->r7));
			printf("  r8         = %08lx [%08lx]\n",
			       _state->r8, va_to_pa(_state->r8));
			printf("  r9         = %08lx [%08lx]\n",
			       _state->r9, va_to_pa(_state->r9));
			printf("  r10        = %08lx [%08lx]\n",
			       _state->r10, va_to_pa(_state->r10));
			printf("  r11        = %08lx [%08lx]\n",
			       _state->r11, va_to_pa(_state->r11));
			printf("  r12        = %08lx [%08lx]\n",
			       _state->r12, va_to_pa(_state->r12));
			printf("  sp         = %08lx [%08lx]\n",
			       _state->sp, va_to_pa(_state->sp));
			printf("  lr         = %08lx [%08lx]\n",
			       _state->lr, va_to_pa(_state->lr));
			printf("  ip         = %08lx [%08lx]\n",
			       _state->ip, va_to_pa(_state->ip));
			printf("  cpsr       = %08lx\n", _state->cpsr);
			for (unsigned i = 0;
			     i < Vm_state::Mode_state::MAX; i++) {
				printf("  sp_%s     = %08lx [%08lx]\n", modes[i],
				       _state->mode[i].sp, va_to_pa(_state->mode[i].sp));
				printf("  lr_%s     = %08lx [%08lx]\n", modes[i],
				       _state->mode[i].lr, va_to_pa(_state->mode[i].lr));
				printf("  spsr_%s   = %08lx [%08lx]\n", modes[i],
				       _state->mode[i].spsr, va_to_pa(_state->mode[i].spsr));
			}
			printf("  ttbr0      = %08lx\n", _state->ttbr[0]);
			printf("  ttbr1      = %08lx\n", _state->ttbr[1]);
			printf("  ttbrc      = %08lx\n", _state->ttbrc);
			printf("  dfar       = %08lx [%08lx]\n",
			       _state->dfar, va_to_pa(_state->dfar));
			printf("  exception  = %s\n", exc[_state->cpu_exception]);
		}

		Genode::addr_t va_to_pa(Genode::addr_t va)
		{
			return 0;
		}

		Vm_state *state() const { return  _state; }
};


class Vmm
{
	private:

		Genode::Signal_receiver _sig_rcv;
		Genode::Signal_context  _vm_context;
		Vm                     &_vm;

		void _handle_hypervisor_call()
		{
			/* check device number*/
			switch (_vm.state()->r0) {
			default:
				PERR("Unknown hypervisor call!");
				_vm.dump();
			};
		}

		bool _handle_data_abort()
		{
			_vm.dump();
			return false;
		}

		bool _handle_vm()
		{
			/* check exception reason */
			switch (_vm.state()->cpu_exception) {
			case Genode::Cpu_state::DATA_ABORT:
				if (!_handle_data_abort()) {
					PERR("Could not handle data-abort will exit!");
					return false;
				}
				break;
			case Genode::Cpu_state::SUPERVISOR_CALL:
				_handle_hypervisor_call();
				break;
			default:
				PERR("Curious exception occured");
				_vm.dump();
				return false;
			}
			return true;
		}

	public:

		Vmm(Vm &vm) : _vm(vm) { }

		void run()
		{
			_vm.sig_handler(_sig_rcv.manage(&_vm_context));
			_vm.start();
			_vm.run();

			while (true) {
				Genode::Signal s = _sig_rcv.wait_for_signal();
				if (s.context() == &_vm_context) {
					if (_handle_vm())
						_vm.run();
				} else {
					PWRN("Invalid context");
					continue;
				}
			}
		};

};

int main()
{
	Vm vm("test-kernel");
	Vmm vmm(vm);

	PINF("Start virtual machine ...");
	vmm.run();
	Genode::sleep_forever();
	return 0;
}
