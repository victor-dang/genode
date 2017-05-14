/*
 * \brief  Timer driver for core
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__RPI3__TIMER_H_
#define _CORE__SPEC__RPI3__TIMER_H_

/* base-hw includes */
#include <kernel/types.h>

namespace Genode { class Timer; }

class Genode::Timer
{
	public:

		Timer() {}

		static unsigned interrupt_id(unsigned const) {
			return 0; }

		void   start_one_shot(Kernel::time_t const tics, unsigned const) {}
		Kernel::time_t tics_to_us(Kernel::time_t const tics) const { return 0; }
		Kernel::time_t us_to_tics(Kernel::time_t const us) const { return 0; }
		Kernel::time_t max_value() { return 0; }
		Kernel::time_t value(unsigned const) { return 0; }
};

namespace Kernel { using Genode::Timer; }

#endif /* _CORE__SPEC__RPI3__TIMER_H_ */
