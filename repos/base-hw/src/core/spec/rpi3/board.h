/*
 * \brief  Board driver for core
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__RPI3__BOARD_H_
#define _CORE__SPEC__RPI3__BOARD_H_

#include <drivers/defs/rpi.h>
#include <drivers/uart/pl011.h>

namespace Board {

	using Serial = Genode::Pl011_uart;

	enum {
		UART_BASE  = Rpi::PL011_0_MMIO_BASE,
		UART_CLOCK = Rpi::PL011_0_CLOCK,
	};
};

#endif /* _CORE__SPEC__RPI3__BOARD_H_ */
