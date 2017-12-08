/*
 * \brief  Kernel backend for core log messages
 * \author Norman Feske
 * \author Stefan Kalkowski
 * \date   2016-10-10
 */

/*
 * Copyright (C) 2016-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE_LOG_H_
#define _CORE_LOG_H_

#include <util/string.h>

namespace Genode {
	struct Core_log;
	Core_log &core_log();
}


struct Genode::Core_log
{
	static Genode::addr_t core_log;
	static Genode::size_t core_log_size;

	void out(char const c);

	void output(char const * str) {
		for (unsigned i = 0; i < Genode::strlen(str); i++) out(str[i]); }
};

#endif /* _CORE_LOG_H_ */
