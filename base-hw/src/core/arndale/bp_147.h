/*
 * \brief  Driver for the Trustzone Protection Controller BP147
 * \author Stefan Kalkowski
 * \date   2012-07-04
 */

/*
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _BASE_HW__SRC__SERVER__VMM__BP_147_H_
#define _BASE_HW__SRC__SERVER__VMM__BP_147_H_

/* Genode includes */
#include <util/mmio.h>

struct Bp_147 : Genode::Mmio
{
	template <Genode::off_t OFF>
	struct Tzpcdecprot3 : public Register<OFF, 32>
	{
		struct Tzasc_cbx   : Register<OFF, 32>::template Bitfield<0,1> {};
		struct Tzasc_drbx  : Register<OFF, 32>::template Bitfield<1,1> {};
		struct Tzasc_xlbx  : Register<OFF, 32>::template Bitfield<2,1> {};
		struct Tzasc_xr1bx : Register<OFF, 32>::template Bitfield<3,1> {};
	};

	struct Tzpcdecprot3stat : Tzpcdecprot3<0x824> {};
	struct Tzpcdecprot3set  : Tzpcdecprot3<0x828> {};
	struct Tzpcdecprot0clr  : Tzpcdecprot3<0x808> {};
	struct Tzpcdecprot1clr  : Tzpcdecprot3<0x814> {};
	struct Tzpcdecprot2clr  : Tzpcdecprot3<0x820> {};
	struct Tzpcdecprot3clr  : Tzpcdecprot3<0x82c> {};

	Bp_147(Genode::addr_t const base) : Genode::Mmio(base) {}
};

#endif /* _BASE_HW__SRC__SERVER__VMM__BP_147_H_ */
