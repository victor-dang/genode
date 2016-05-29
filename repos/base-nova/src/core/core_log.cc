/*
 * \brief  Kernel-specific core's 'log' backend
 * \author Stefan Kalkowski
 * \date   2016-10-10
 */

/*
 * Copyright (C) 2016-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* core includes */
#include <core_log.h>

/* Genode includes */
#include <bios_data_area.h>
#include <drivers/uart/x86_pc.h>

/**
 * Memory for core log during early boot
 */
Genode::addr_t Genode::Core_log::core_log = 0;
Genode::size_t Genode::Core_log::core_log_size = 0;

void Genode::Core_log::out(char const c)
{
	enum { CLOCK = 0, BAUDRATE = 115200 };

	static X86_uart uart(Bios_data_area::singleton()->serial_port(),
	                     CLOCK, BAUDRATE);
	if (c == '\n')
		uart.put_char('\r');
	uart.put_char(c);

	if (core_log) {
		static unsigned pos = 0;

		*reinterpret_cast<unsigned *>(core_log) = pos;

		*reinterpret_cast<char *>(core_log + sizeof(pos) + pos) = c;

		pos = (pos + 1) % (core_log_size - sizeof(pos));
	}
}
