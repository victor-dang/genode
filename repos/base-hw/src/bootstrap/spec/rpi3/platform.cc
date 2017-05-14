/*
 * \brief   Platform implementations specific for base-hw and Raspberry Pi 3
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
#include <hw/assert.h>
#include <platform.h>

/**
 * Leave out the first page (being 0x0) from bootstraps RAM allocator,
 * some code does not feel happy with addresses being zero
 */
Bootstrap::Platform::Board::Board()
: early_ram_regions(Memory_region { Rpi::RAM_0_BASE + 0x1000,
                                    Rpi::RAM_0_SIZE - 0x1000 }),
  late_ram_regions(Memory_region { Rpi::RAM_0_BASE, 0x1000 }),
  core_mmio(Memory_region { Rpi::PL011_0_MMIO_BASE,
                            Rpi::PL011_0_MMIO_SIZE }) {}


void Bootstrap::Platform::enable_mmu()
{
	Genode::error("FIXME: Bootstrap::Platform::enable_mmu");
}

