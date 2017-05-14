/*
 * \brief  Constants definitions for the ARM64 architecture.
 * \author Stefan Kalkowski
 * \date   2017-05-13
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__SPEC__ARM64__CPU__CONSTS_H_
#define _INCLUDE__SPEC__ARM64__CPU__CONSTS_H_

#include <base/stdint.h>

namespace Abi {

	/*
	 * On ARM we align the stack top to 16-byte. As a call (or branch) will not
	 * change the stack pointer, we need no further stack adjustment.
	 */
	inline Genode::addr_t stack_align(Genode::addr_t addr) {
		return (addr & ~0xf); }

	/**
	 * Do ABI specific initialization to a freshly created stack
	 */
	inline void init_stack(Genode::addr_t) { }
}

#endif /* _INCLUDE__SPEC__ARM64__CPU__CONSTS_H_ */
