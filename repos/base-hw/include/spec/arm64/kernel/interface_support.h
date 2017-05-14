/*
 * \brief  Interface between kernel and userland
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _INCLUDE__SPEC__AARCH64__KERNEL__INTERFACE_SUPPORT_H_
#define _INCLUDE__SPEC__AARCH64__KERNEL__INTERFACE_SUPPORT_H_

/* Genode includes */
#include <base/stdint.h>

namespace Kernel {
	using Call_arg = Genode::addr_t;
	using Call_ret = Genode::addr_t;
}

#endif /* _INCLUDE__SPEC__AARCH64__KERNEL__INTERFACE_SUPPORT_H_ */

