/*
 * \brief   Kernel backend for protection domains
 * \author  Stefan Kalkowski
 * \date    2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/log.h>
#include <kernel/pd.h>


Kernel::Pd::Pd(Hw::Page_table * const table,
               Genode::Platform_pd * const platform_pd)
: _table(table), _platform_pd(platform_pd)
{
	Genode::error("FIXME: Kernel::Pd::Pd");
}


Kernel::Pd::~Pd()
{
	Genode::error("FIXME: Kernel::Pd::~Pd");
}


void Kernel::Pd::admit(Kernel::Cpu::Context * const c)
{
	Genode::error("FIXME: Kernel::Pd::admit");
}

