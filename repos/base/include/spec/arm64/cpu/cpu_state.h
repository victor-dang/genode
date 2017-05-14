/*
 * \brief  CPU state
 * \author Stefan Kalkowski
 * \date   2017-05-13
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__SPEC__ARM64__CPU__CPU_STATE_H_
#define _INCLUDE__SPEC__ARM64__CPU__CPU_STATE_H_

/* Genode includes */
#include <base/stdint.h>

namespace Genode { struct Cpu_state; }

struct Genode::Cpu_state
{
	addr_t r[31]; /* general purpose register 0..31 */
	addr_t sp;    /* stack pointer */
	addr_t ip;    /* instruction pointer */
};

#endif /* _INCLUDE__SPEC__ARM64__CPU__CPU_STATE_H_ */
