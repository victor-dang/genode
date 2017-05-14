/*
 * \brief  Cpu driver for core
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__RPI3__CPU_H_
#define _CORE__SPEC__RPI3__CPU_H_

#include <cpu/cpu_state.h>

namespace Genode {
	class Cpu;
	using sizet_arithm_t = __uint128_t;
}

struct Genode::Cpu
{
	struct Pd {};
	struct Context : Genode::Cpu_state {};
	struct User_context : Context
	{
		void user_arg_0(Genode::addr_t const arg) { r[0] = arg; }
		void user_arg_1(Genode::addr_t const arg) { r[1] = arg; }
		void user_arg_2(Genode::addr_t const arg) { r[2] = arg; }
		void user_arg_3(Genode::addr_t const arg) { r[3] = arg; }
		void user_arg_4(Genode::addr_t const arg) { r[4] = arg; }
		Genode::addr_t user_arg_0() const { return r[0]; }
		Genode::addr_t user_arg_1() const { return r[1]; }
		Genode::addr_t user_arg_2() const { return r[2]; }
		Genode::addr_t user_arg_3() const { return r[3]; }
		Genode::addr_t user_arg_4() const { return r[4]; }
	};

	static unsigned executing_id() { return 0; }
	static unsigned primary_id()   { return 0; }

	static void wait_for_interrupt() { asm volatile ("wfi"); }
	static void switch_to(User_context&) { }

	static constexpr Genode::addr_t exception_entry = 0xffff0000;
	static constexpr Genode::size_t mtc_size        = 0x1000;
};

#endif /* _CORE__SPEC__RPI3__CPU_H_ */
