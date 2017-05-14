/*
 * \brief  Translation tables
 * \author Stefan Kalkowski
 * \date   2017-05-14
 */

/*
 * Copyright (C) 2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _SRC__LIB__HW__SPEC__ARM64__PAGE_TABLE_H_
#define _SRC__LIB__HW__SPEC__ARM64__PAGE_TABLE_H_

#include <hw/page_flags.h>
#include <hw/page_table_allocator.h>

namespace Hw { struct Page_table; }

struct Hw::Page_table
{
	using Allocator = Hw::Page_table_allocator<4096>;

	enum {
		ALIGNM_LOG2            = 6,
		CORE_TRANS_TABLE_COUNT = 16,
	};

	void insert_translation(Genode::addr_t vo, Genode::addr_t pa,
	                        Genode::size_t size, Hw::Page_flags const & f,
	                        Allocator & alloc) {}
	
	void remove_translation(Genode::addr_t vo, Genode::size_t size,
	                        Allocator & alloc) {}
};

#endif /* _SRC__LIB__HW__SPEC__ARM64__PAGE_TABLE_H_ */
