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
#include <os/attached_ram_dataspace.h>
#include <vm_session/connection.h>
#include <cpu/cpu_state.h>

#include <util/avl_tree.h>
#include <util/mmio.h>

struct Vm_state : Genode::Cpu_state_modes
{
	Genode::addr_t vm_id;
	Genode::addr_t table;
	Genode::addr_t sctrl;
	Genode::addr_t hsr;
	Genode::addr_t hpfar;
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

	Genode::addr_t midr;
	Genode::addr_t ctr;
	Genode::addr_t ccsidr;
	Genode::addr_t clidr;
	Genode::addr_t pfr0;
	Genode::addr_t mmfr0;
	Genode::addr_t isar0;
	Genode::addr_t isar3;
	Genode::addr_t isar4;
	Genode::addr_t csselr;
	Genode::addr_t actrl;

	Genode::addr_t dummy;

	struct Gp_register { Genode::addr_t r[16]; };

	Genode::addr_t * r(unsigned i) {
		return &(reinterpret_cast<Gp_register*>(this)->r[i]); }
};


class Ram {

	private:

		Genode::addr_t const _base;
		Genode::size_t const _size;
		Genode::addr_t const _local;

	public:

		class Invalid_addr : Genode::Exception {};

		Ram(Genode::addr_t const addr, Genode::size_t const sz,
		    Genode::addr_t const local)
		: _base(addr), _size(sz), _local(local) { }

		Genode::addr_t base()  const { return _base;  }
		Genode::size_t size()  const { return _size;  }
		Genode::addr_t local() const { return _local; }

		Genode::addr_t va(Genode::addr_t phys)
		{
			if ((phys < _base) || (phys > (_base + _size)))
				throw Invalid_addr();
			return _local + (phys - _base);
		}
};


class Vm {

	private:

		enum {
			RAM_ADDRESS   = 0x80000000,
			MACH_TYPE     = 2272, /* ARNDALE = 4274; VEXPRESS = 2272 */
			KERNEL_OFFSET = 0x8000,
			DTB_OFFSET    = 64 * 1024 * 1024,
		};

		Genode::Vm_connection          _vm_con;
		Genode::Attached_rom_dataspace _kernel_rom;
		Genode::Attached_rom_dataspace _dtb_rom;
		Genode::Attached_ram_dataspace _vm_ram;
		Ram                            _ram;
		Vm_state                      *_state;

		void _load_kernel()
		{
			Genode::memcpy((void*)(_ram.local() + KERNEL_OFFSET),
			               _kernel_rom.local_addr<void>(),
			               _kernel_rom.size());
			_state->ip = _ram.base() + KERNEL_OFFSET;
		}

		void _load_dtb()
		{
			Genode::memcpy((void*)(_ram.local() + DTB_OFFSET),
			               _dtb_rom.local_addr<void>(),
			               _dtb_rom.size());
			_state->r2 = _ram.base() + DTB_OFFSET;
		}

	public:

		Vm(const char *kernel, const char *dtb, Genode::size_t const ram_size)
		: _kernel_rom(kernel),
		  _dtb_rom(dtb),
		  _vm_ram(Genode::env()->ram_session(), ram_size, Genode::UNCACHED),
		  _ram(RAM_ADDRESS, ram_size, (Genode::addr_t)_vm_ram.local_addr<void>()),
		  _state((Vm_state*)Genode::env()->rm_session()->attach(_vm_con.cpu_state()))
		{
			PINF("ram is at %lx", Genode::Dataspace_client(_vm_ram.cap()).phys_addr());
			_vm_con.attach(_vm_ram.cap(), RAM_ADDRESS);
		}

		void start()
		{
			Genode::memset((void*)_state, 0, sizeof(Genode::Cpu_state_modes));
			_load_kernel();
			_load_dtb();
			_state->r1    = MACH_TYPE;
			_state->cpsr  = 0x93; /* SVC mode and IRQs disabled */

			PINF("ready to run");
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
				{ "nope", "reset", "undefined", "svc", "pf_abort",
			      "data_abort", "irq", "fiq", "trap" };

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
			printf("  sctrl      = %08lx\n",
			       _state->sctrl);
			printf("  hsr        = %08lx\n",
			       _state->hsr);
			printf("  ttbcr      = %08lx\n",
			       _state->ttbcr);
			printf("  ttbr0      = %08lx\n",
			       _state->ttbr0);
			printf("  ttbr1      = %08lx\n",
			       _state->ttbr1);
			printf("  prrr       = %08lx\n",
			       _state->prrr);
			printf("  nmrr       = %08lx\n",
			       _state->nmrr);
			printf("  dacr       = %08lx\n",
			       _state->dacr);
			printf("  dfar       = %08lx\n",
			       _state->dfar);
			printf("  ifar       = %08lx\n",
			       _state->ifar);
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

		struct Hsr : Genode::Register<32>
		{
			struct Ec : Bitfield<26, 6>
			{
				enum {
					CP15 = 0x3,
					HVC  = 0x12,
					DA   = 0x24
				};
			};
		};

		class Coprocessor
		{
			protected:

				struct Iss : Hsr
				{
					struct Direction : Bitfield<0,  1> {};
					struct Crm       : Bitfield<1,  4> {};
					struct Register  : Bitfield<5,  4> {};
					struct Crn       : Bitfield<10, 4> {};
					struct Opcode1   : Bitfield<14, 3> {};
					struct Opcode2   : Bitfield<17, 3> {};

					static access_t value(unsigned crn, unsigned op1,
					                      unsigned crm, unsigned op2)
					{
						access_t v = 0;
						Crn::set(v, crn);
						Crm::set(v, crm);
						Opcode1::set(v, op1);
						Opcode2::set(v, op2);
						return v;
					};

					static access_t mask_encoding(access_t v)
					{
						return Crm::masked(v) |
						       Crn::masked(v) |
						       Opcode1::masked(v) |
						       Opcode2::masked(v);
					}
				};


				class Register : public Genode::Avl_node<Register>
				{
					private:

						const Iss::access_t       _encoding;
						const char               *_name;
						const bool                _writeable;
						Genode::addr_t Vm_state::*_r;
						const Genode::addr_t      _init_value;

					public:

						Register(unsigned crn, unsigned op1,
						         unsigned crm, unsigned op2,
						         const char * name,
						         bool writeable,
						         Genode::addr_t Vm_state::*r,
						         Genode::addr_t v)
						: _encoding(Iss::value(crn, op1, crm, op2)),
						  _name(name),
						  _writeable(writeable), _r(r), _init_value(v) {}

						const char * name()       const { return _name;      }
						const bool   writeable()  const { return _writeable; }
						const Genode::addr_t init_value() const {
							return _init_value; }

						Register * find_by_encoding(Iss::access_t e)
						{
							if (e == _encoding) return this;

							Register * r =
								Avl_node<Register>::child(e > _encoding);
							return r ? r->find_by_encoding(e) : 0;
						}

						void write(Vm_state * state, Genode::addr_t v) {
							PINF("write %08lx to %s", v, name());
							if (Genode::strcmp(name(), "SCTRL") == 0) v &= ~(1 << 13);
							state->*_r = v; }

						Genode::addr_t read(Vm_state * state) const {
							PINF("read %08lx from %s", state->*_r, name());
							return state->*_r; }


						/************************
						 ** Avl node interface **
						 ************************/

						bool higher(Register *r) {
							return (r->_encoding > _encoding); }
				};

				Genode::Avl_tree<Register> _reg_tree;

			public:

				bool handle_trap(Vm_state *state)
				{
					Iss::access_t v = state->hsr;
					Register * reg = _reg_tree.first();
					if (reg) reg = reg->find_by_encoding(Iss::mask_encoding(v));

					if (!reg) {
						PERR("Unknown cp15 access @ ip=%08lx:", state->ip);
						PERR("%s: c15 %d r%d c%d c%d %d",
							 Iss::Direction::get(v) ? "read" : "write",
							 Iss::Opcode1::get(v), Iss::Register::get(v),
							 Iss::Crn::get(v), Iss::Crm::get(v),
							 Iss::Opcode2::get(v));
						return false;
					}

					if (Iss::Direction::get(v)) { /* read access  */
						*(state->r(Iss::Register::get(v))) = reg->read(state);
					} else {                      /* write access */
						if (!reg->writeable()) {
							PERR("Writing to cp15 register %s not allowed!",
							     reg->name());
							return false;
						}
						reg->write(state, *(state->r(Iss::Register::get(v))));
					}
					state->ip += sizeof(Genode::addr_t);
					return true;
				}
		};


		class Cp15 : public Coprocessor
		{
			private:

				Register _regs[25] {
					{  0, 0, 0, 0, "MIDR",   false, &Vm_state::midr,   0x410fc0f4 },
					{  0, 0, 0, 1, "CTR",    false, &Vm_state::ctr,    0x8444c004 },
					{  0, 1, 0, 0, "CCSIDR", false, &Vm_state::ccsidr, 0x701fe00a },
					{  0, 1, 0, 1, "CLIDR",  false, &Vm_state::clidr,  0x0a200023 },
					{  0, 0, 1, 0, "PFR0",   false, &Vm_state::pfr0,   0x00001031 },
					{  0, 0, 1, 4, "MMFR0",  false, &Vm_state::mmfr0,  0x10201105 },
					{  0, 0, 2, 0, "ISAR0",  false, &Vm_state::isar0,  0x02101110 },
					{  0, 0, 2, 3, "ISAR3",  false, &Vm_state::isar3,  0x11112131 },
					{  0, 0, 2, 4, "ISAR4",  false, &Vm_state::isar4,  0x10011142 },
					{  0, 2, 0, 0, "CSSELR", true,  &Vm_state::csselr, 0x00000000 },
					{  1, 0, 0, 0, "SCTRL",  true,  &Vm_state::sctrl,  0 /* 0xc5007a 0x00c5187a*/ },
					{  1, 0, 0, 1, "ACTRL",  true,  &Vm_state::actrl,  0x00000040 },
					{  2, 0, 0, 0, "TTBR0",  true,  &Vm_state::ttbr0,  0x00000000 },
					{  2, 0, 0, 1, "TTBR1",  true,  &Vm_state::ttbr1,  0x00000000 },
					{  2, 0, 0, 2, "TTBCR",  true,  &Vm_state::ttbcr,  0x00000000 },
					{  3, 0, 0, 0, "DACR",   true,  &Vm_state::dacr,   0x55555555 },
					{  5, 0, 0, 0, "DFSR",   true,  &Vm_state::dfsr,   0x00000000 },
					{  5, 0, 0, 1, "IFSR",   true,  &Vm_state::ifsr,   0x00000000 },
					{  5, 0, 1, 0, "ADFSR",  true,  &Vm_state::adfsr,  0x00000000 },
					{  5, 0, 1, 1, "AIFSR",  true,  &Vm_state::aifsr,  0x00000000 },
					{  6, 0, 0, 0, "DFAR",   true,  &Vm_state::dfar,   0x00000000 },
					{  6, 0, 0, 2, "IFAR",   true,  &Vm_state::ifar,   0x00000000 },
					{ 10, 0, 2, 0, "PRRR",   true,  &Vm_state::prrr,   0x00098aa4 },
					{ 10, 0, 2, 1, "NMRR",   true,  &Vm_state::nmrr,   0x44e048e0 },

					{ 13, 6, 0, 6, "DUMMY",  true,  &Vm_state::dummy,  0x00000000 },
				};

			public:

				Cp15(Vm_state *state)
				{
					for (unsigned i = 0; i < (sizeof(_regs) / sizeof(Register));
						 i++) {
						_reg_tree.insert(&_regs[i]);
						_regs[i].write(state, _regs[i].init_value());
					}
				}
		};

		Genode::Signal_receiver _sig_rcv;
		Genode::Signal_context  _vm_context;
		Vm                     &_vm;
		Cp15                    _cp15;

		bool _handle_hyper_call()
		{
			PERR("Unknown hyper call!");
			return false;
		}

		bool _handle_data_abort()
		{
			PINF("DATA ABORT @ %llx", (Genode::uint64_t)_vm.state()->hpfar << 8);
			return false;
		}

		bool _handle_trap()
		{
			/* check device number*/
			switch (Hsr::Ec::get(_vm.state()->hsr)) {
			case Hsr::Ec::HVC:
				return _handle_hyper_call();
			case Hsr::Ec::CP15:
				return _cp15.handle_trap(_vm.state());
			case Hsr::Ec::DA:
				return _handle_data_abort();
			default:
				PERR("Unknown trap: %x", Hsr::Ec::get(_vm.state()->hsr));
			};
			return false;
		}

		bool _handle_vm()
		{
			enum { TRAP = 8 };

			/* check exception reason */
			switch (_vm.state()->cpu_exception) {
			case TRAP:
				if (_handle_trap()) return true;
			default:
				PERR("Curious exception occured");
				_vm.dump();
				return false;
			}
			return true;
		}

	public:

		Vmm(Vm &vm) : _vm(vm), _cp15(_vm.state()) { }

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
	static Vm vm("linux", "dtb", 1024 * 1024 * 128);
	static Vmm vmm(vm);

	PINF("Start virtual machine ...");
	vmm.run();
	Genode::sleep_forever();
	return 0;
}
