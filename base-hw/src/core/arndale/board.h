/*
 * \brief  Board definitions for core
 * \author Stefan Kalkowski
 * \date   2014-02-14
 */

/*
 * Copyright (C) 2014 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _ARNDALE__BOARD_H_
#define _ARNDALE__BOARD_H_

/**
 * Genode includes
 */
#include <platform/arndale/drivers/board_base.h>
#include <tsc_380.h>
#include <bp_147.h>

namespace Arndale
{
	struct Board : Genode::Board_base
	{
		/**
		 * Configure this module appropriately for the first kernel run
		 */
		static void prepare_kernel()
		{
			PDBG("Do something");
			static Tsc_380 tsc0(0x10D40000);
			static Tsc_380 tsc1(0x10D50000);
			static Tsc_380 tsc2(0x10D60000);
			static Tsc_380 tsc3(0x10D70000);
			static Tsc_380 tsc4(0x10D80000);
			static Tsc_380 tsc5(0x10D90000);
			static Tsc_380 tsc6(0x10Da0000);
			static Tsc_380 tsc7(0x10Db0000);
			static Bp_147  tzpc1(0x10110000);
			static Bp_147  tzpc2(0x10120000);
			static Bp_147  tzpc3(0x10130000);
			static Bp_147  tzpc4(0x10140000);
			static Bp_147  tzpc5(0x10150000);
			static Bp_147  tzpc6(0x10160000);
			static Bp_147  tzpc7(0x10170000);
			static Bp_147  tzpc8(0x10180000);
			tzpc1.write<Bp_147::Tzpcdecprot3set>(255);
			tzpc2.write<Bp_147::Tzpcdecprot3set>(255);
			tzpc3.write<Bp_147::Tzpcdecprot3set>(255);
			tzpc4.write<Bp_147::Tzpcdecprot3set>(255);
			tzpc5.write<Bp_147::Tzpcdecprot3set>(255);
			tzpc6.write<Bp_147::Tzpcdecprot3set>(255);
			tzpc7.write<Bp_147::Tzpcdecprot3set>(255);
			tzpc8.write<Bp_147::Tzpcdecprot3set>(255);
			static Bp_147  tzpc9(0x10190000);
			tzpc9.write<Bp_147::Tzpcdecprot3set>(
				Bp_147::Tzpcdecprot3set::Tzasc_cbx::bits(0)               |
				Bp_147::Tzpcdecprot3set::Tzasc_drbx::bits(1)               |
				Bp_147::Tzpcdecprot3set::Tzasc_xlbx::bits(1) |
				Bp_147::Tzpcdecprot3set::Tzasc_xr1bx::bits(1));
			PDBG("works");
		}
	};
}

namespace Genode { class Board : public Arndale::Board { }; }

#endif /* _ARNDALE_BOARD_H_ */

