/*
 * \brief  Programmable interrupt controller for core
 * \author Stefan Kalkowski
 * \date   2012-10-24
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <pic.h>

using namespace Genode;

void Arm_gic::_init()
{
	_distr.write<Distr::Ctlr>(0);

	/* configure every shared peripheral interrupt */
	for (unsigned i = min_spi; i <= _max_irq; i++) {
		_distr.write<Distr::Igroupr::Group_status>(1, i);
		_distr.write<Distr::Icfgr::Edge_triggered>(0, i);
		_distr.write<Distr::Ipriorityr::Priority>(0, i);
	}

	/* enable device */
	_distr.write<Distr::Ctlr>(0x3);
}
