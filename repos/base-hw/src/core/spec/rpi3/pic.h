/*
 * \brief  Programmable interrupt controller for core
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _CORE__SPEC__RPI3__PIC_H_
#define _CORE__SPEC__RPI3__PIC_H_

namespace Genode { class Pic; }

class Genode::Pic
{
	public:

		enum {
			NR_OF_IRQ = 64,
			IPI,
		};

		Pic() {}

		void init_cpu_local() { }
		bool take_request(unsigned &irq) { return false; }
		void finish_request() { }
		void mask() {}
		void unmask(unsigned const i, unsigned) {}
		void mask(unsigned const i) {}
};

namespace Kernel { using Genode::Pic; }

#endif /* _CORE__SPEC__RPI3__PIC_H_ */
