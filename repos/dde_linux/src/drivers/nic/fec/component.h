/*
 * \brief  Freescale ethernet driver component
 * \author Stefan Kalkowski
 * \date   2017-11-01
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

#ifndef _SRC__DRIVERS__NIC__FEC__COMPONENT_H_
#define _SRC__DRIVERS__NIC__FEC__COMPONENT_H_

/* Genode includes */
#include <nic/root.h>

struct Session_component : Nic::Session_component
{
	using Nic::Session_component::Session_component;

	Nic::Mac_address mac_address() { return Nic::Mac_address(); }
	bool link_state()              { return false; }
	void _handle_packet_stream()   {}
};

#endif /* _SRC__DRIVERS__NIC__FEC__COMPONENT_H_ */
