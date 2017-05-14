/*
 * \brief   Raspberry PI 3 specific board definitions
 * \author  Stefan Kalkowski
 * \date    2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _SRC__BOOTSTRAP__SPEC__RPI3__BOARD_H_
#define _SRC__BOOTSTRAP__SPEC__RPI3__BOARD_H_

#include <drivers/defs/rpi.h>
#include <drivers/uart/pl011.h>
#include <hw/spec/arm64/page_table.h>

namespace Board {
	using Serial = Genode::Pl011_uart;

	enum {
		UART_BASE  = 0x3f215040,
		UART_CLOCK = 0,
	};
}

namespace Bootstrap {
	struct Cpu {};
	struct Pic {};
}

#endif /* _SRC__BOOTSTRAP__SPEC__RPI3__BOARD_H_ */
