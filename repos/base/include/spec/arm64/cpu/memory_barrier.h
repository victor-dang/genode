/*
 * \brief  Memory barrier
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__SPEC__ARM64__CPU__MEMORY_BARRIER_H_
#define _INCLUDE__SPEC__ARM64__CPU__MEMORY_BARRIER_H_

namespace Genode {
	static inline void memory_barrier() {
		asm volatile ("dmb SY" ::: "memory"); }
}

#endif /* _INCLUDE__SPEC__ARM64__CPU__MEMORY_BARRIER_H_ */
